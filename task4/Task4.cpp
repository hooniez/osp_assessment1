//
// Created by hooniesun on 26/08/22.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define NUM_MAP_THREADS 13
#define NUM_INDEX_ARRAYS 13
#define NICEST_VALUE 19


#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/resource.h>

#include <vector>
#include "../task1/Task1Filter.cpp"
#include <algorithm>
#include <sstream>
#include <map>

pthread_mutex_t mutexFileNames;
pthread_cond_t condFileNameRead;

std::vector<std::string> fileNames;
std::vector<std::string> wordVec;

std::map<int, int> niceValuesBeforeFIFO = {{3, 19}, {4, 7}, {5, 3}, {6, 1},
                                           {7, 0}, {8, 1}, {9, 0}, {10, 3},
                                           {11, 6}, {12, 7 }, {13, 13}, {14, 14},
                                           {15, 19}};

std::map<int, int> initialNiceValues = {{3, 12}, {4, 7}, {5, 2}, {6, 1},
                                           {7, 0}, {8, 1}, {9, 1}, {10, 2},
                                           {11, 3}, {12, 4 }, {13, 7}, {14, 7},
                                           {15, 9}};

inline int getBytesToRead(const std::string& fileName) {
    // Every file to be passed as an argument will start with 'fifo'.
    // The line below returns any number that follows the name fifo.
    // The number 4 below stands for the starting pos from which to read the number
    return stoi(fileName.substr(4,fileName.size() - 4)) + 1;
}

void* sort4(void *arg) {
    auto* iVec = (std::vector<int>*)arg;
    int wordLength = wordVec[*iVec->begin()].length();

    pid_t tid = syscall(__NR_gettid);
    std::cout << "sort4 | the thread id for fifo " << wordLength << " is: " << tid << std::endl;

    if (nice(initialNiceValues.at(wordLength)) == -1) {
        perror("Failed to set the nice value");
    } else {
        std::cout << "Thread " << wordLength << ": " << getpriority(PRIO_PROCESS, tid) << std::endl;
    }

    // Sort the index array
    std::sort(iVec->begin(), iVec->end(), [] (const int a, const int b) {
        return wordVec[a].compare(MIN_WORD_LENGTH - 1, wordVec[a].size() - (MIN_WORD_LENGTH - 1), wordVec[b], MIN_WORD_LENGTH - 1, wordVec[b].size() - (MIN_WORD_LENGTH - 1)) < 0;
    });


    // Create a FIFO file for write
    pthread_mutex_lock(&mutexFileNames);
    std::ostringstream oss;
    oss << "fifo" << wordLength;
    const char *fileName = oss.str().c_str();
    if (mkfifo(fileName, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
        }
    }

    // Store the file names and send a signal to the reduce4 thread
    fileNames.emplace_back(fileName);
    pthread_mutex_unlock(&mutexFileNames);
    pthread_cond_signal(&condFileNameRead);

    if (nice(NICEST_VALUE) == -1) {
        perror("Failed to set the nice value");
    }

    int fd = open(fileName, O_WRONLY );
    if (fd == -1) {
        perror("Error in opening a FIFO file");
    }

    // Write words to fifo
    for (auto const i : *iVec) {
        if (write(fd, wordVec[i].c_str(), wordVec[i].length() + 1) == -1) {
            std::ostringstream errorMsg;
            errorMsg << "Could not write to fifo" << fd;
            perror(errorMsg.str().c_str());
            break;
        }
    }

    close(fd);
    return arg;
}

void* reduce4(void *arg) {
    pid_t tid = syscall(__NR_gettid);
    std::cout << "reduce4 | the thread id " << " is: " << tid << std::endl;

    pthread_mutex_lock(&mutexFileNames);
    // Repeat the while loop as long as there is a file to read from map4()
    while (fileNames.size() != NUM_MAP_THREADS) {
        pthread_cond_wait(&condFileNameRead, &mutexFileNames);
        // pthread_cond_wait is equivalent to:
        // pthread_mutex_unlock(&mutexFileNames);
        // wait for signal on condFileNameRead;
        // pthread_mutex_lock(&mutexFileNames);
    }

    // Convert each file name into a file descriptor and store it in fds
    std::vector<int> fds;
    for (const auto &fileName: fileNames) {
        fds.push_back(open(fileName.c_str(), O_RDONLY));
    }

    // As with Task 2, store the first word from each FIFO in words
    std::string line;
    std::vector<std::string> words;
    char word[MAX_WORD_LENGTH + 1];
    std::vector<int> indicesToErase;
    int numWordsToOutput = 0;

    for (int i = 0; i < fds.size(); ++i) {
        // IF there is no line to read from a file
        if (read(fds[i], word, getBytesToRead(fileNames[i])) == 0) {
            indicesToErase.push_back(i);
        }
        words.emplace_back(word);
        ++numWordsToOutput;
    }

    // If there were any empty files, remove the corresponding positions from inputFiles and words
    for (auto indexToErase: indicesToErase) {
        close(fds[indexToErase]);
        fds.erase(fds.begin() + indexToErase);
        fileNames.erase(fileNames.begin() + indexToErase);
        words.erase(words.begin() + indexToErase);
    }

    // Write the lowest word in lexical order to a file
    int lowestOrderIdx = 0;
    std::string outputFileName((char *)arg);
    std::ofstream outputFile(outputFileName);
    while (!fds.empty()) {
        // Find the actual index that contains the lowest word in lexical order
        for (int i = 0; i < words.size(); ++i) {
            if (words[i].compare(MIN_WORD_LENGTH - 1, words[i].size() - (MIN_WORD_LENGTH - 1), words[lowestOrderIdx], MIN_WORD_LENGTH - 1, words[lowestOrderIdx].size() - (MIN_WORD_LENGTH - 1)) < 0) {
                lowestOrderIdx = i;
            }
        }

        outputFile << words[lowestOrderIdx] << std::endl;

        // Read another word from the FIFO from which the lowest word was just found
        if (read(fds[lowestOrderIdx], word, getBytesToRead(fileNames[lowestOrderIdx])) != 0) {
            words[lowestOrderIdx] = word;
            numWordsToOutput++;
        } else {
            close(fds[lowestOrderIdx]);
            fds.erase(fds.begin() + lowestOrderIdx);
            fileNames.erase(fileNames.begin() + lowestOrderIdx);
            words.erase(words.begin() + lowestOrderIdx);
            lowestOrderIdx = 0;
        }
    }

    outputFile.close();

    pthread_mutex_unlock(&mutexFileNames);

    return arg;
}


void* map4(void* a) {
    pid_t x = syscall(__NR_gettid);
    std::cout << "map4 | the thread id " << " is: " << x << std::endl;
    // Create 13 index arrays
    std::vector<std::vector<int>> iVecs;
    for (int i = 0; i < NUM_INDEX_ARRAYS; ++i) {
        std::vector<int> iVec;
        iVecs.push_back(iVec);
    }

    // Index the global array
    for (int i = 0; i < wordVec.size(); ++i) {
        size_t wordLength = wordVec[i].size();
        if (wordLength >= MIN_WORD_LENGTH && wordLength <= MAX_WORD_LENGTH) {
            iVecs[wordLength - MIN_WORD_LENGTH].push_back(i);
        }
    }

    // Create threads such that the longer it takes for an index array to be sorted, the earlier its thread starts
    std::vector<int> orderOfThreads = {4, 6, 3, 5, 2, 7, 8, 9, 1, 10, 11, 12, 0};

    // Create 13 threads for map4
    pthread_t th[NUM_MAP_THREADS];
    for (int i = 0; i < NUM_MAP_THREADS; ++i) {
        if (pthread_create(th + i, nullptr, &sort4, &iVecs[orderOfThreads[i]]) != 0) {
            perror("Failed to create thread");
        }
    }

    // After spawning the threads above, map4() has now done its duties. Set its nice value to 10.
    if (nice(NICEST_VALUE) == -1) {
        perror("Failed to set the nice value");
    }

    // Join the threads
    for (int i = 0; i < NUM_MAP_THREADS; ++i) {
        if (pthread_join(th[i], nullptr) != 0) {
            perror("Failed to join thread");
        }
    }
    return a;
}



int main(int argc, char* argv[]) {
    pid_t x = syscall(__NR_gettid);
    std::cout << "main | the thread id " << " is: " << x << std::endl;
    if (argc != 3) {
        std::cout << "Correct Usage: ./task2 DirtyFile CleanFile" << std::endl;
        exit(1);
    }
    // Assign the global vector of strings, wordVec
    wordVec = Task1Filter(argv[1]);

    // Create two threads for map4() and reduce4()
    pthread_t mapThread, reduceThread;
    pthread_mutex_init(&mutexFileNames, nullptr);
    pthread_cond_init(&condFileNameRead, nullptr);
    if (pthread_create(&mapThread, nullptr, &map4, nullptr) != 0) {
        perror("Failed to create map4 thread");
    }
    if (pthread_create(&reduceThread, nullptr, &reduce4, argv[2]) != 0) {
        perror("Failed to create reduce4 thread");
    }

    // Join two of the threads above
    if (pthread_join(mapThread, nullptr) != 0) {
        perror("Failed to join map4 thread");
    }
    if (pthread_join(reduceThread, nullptr) != 0) {
        perror("Failed to join reduce4 thread");
    }
    pthread_mutex_destroy(&mutexFileNames);
    pthread_cond_destroy(&condFileNameRead);

}