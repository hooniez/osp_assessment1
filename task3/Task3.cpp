//
// Created by hooniesun on 26/08/22.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define NUM_MAP_THREADS 13
#define NUM_INDEX_ARRAYS 13

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
#include "../task1/Task1Filter.cpp"
#include <algorithm>
#include <sstream>
#include <cstring>

pthread_mutex_t mutexFileNames;
pthread_mutex_t mutexCond;
pthread_mutex_t mutexTest;
pthread_cond_t condFileNameRead;
pthread_cond_t condFIFOWritten;
pthread_cond_t condFIFORead;
int wordLength = 0;
int numWords = 0;

std::vector<std::string> fileNames;
std::vector<std::string> wordVec;

inline int getBytesToRead(const std::string& fileName) {
    // Every file to be passed as an argument will start with 'fifo'.
    // The line below returns any number that follows the name fifo.
    // The number 4 below stands for the starting pos from which to read the number
    return stoi(fileName.substr(4,fileName.size() - 4)) + 1;
}

void* sort3(void *arg) {
    auto* iVec = (std::vector<int>*)arg;
    // Sort the index array
    std::sort(iVec->begin(), iVec->end(), [] (const int a, const int b) {
        return wordVec[a].substr(MIN_WORD_LENGTH - 1) < wordVec[b].substr(MIN_WORD_LENGTH - 1);
    });

    // Create a FIFO file for write
    pthread_mutex_lock(&mutexFileNames);
    std::ostringstream oss;
    oss << "fifo" << wordVec[*iVec->begin()].length();
    const char *fileName = oss.str().c_str();
    if (mkfifo(fileName, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
        }
    }

    // Store the file names and send a signal to the reduce3 thread
    fileNames.emplace_back(fileName);
    pthread_mutex_unlock(&mutexFileNames);
    pthread_cond_signal(&condFileNameRead);

    int fd = open(fileName, O_WRONLY );

    if (fd == -1) {
        perror("Error in opening a FIFO file");
    } else {
        std::cout << "Inside map " << fileName << std::endl;
    }

    // Update the number of words and the word length
    numWords = iVec->size();
    wordLength = wordVec[*iVec->begin()].length();

    // Provide the number of words to read
//    std::cout << <<std::endl;

    char msg[wordLength];
    for (auto const i : *iVec) {
//        strcat(msg, wordVec[i].c_str());
        if (write(fd, wordVec[i].c_str(), wordVec[i].length() + 1) == -1) {
            std::ostringstream errorMsg;
            errorMsg << "Could not write to a FIFO" << fd;
            perror(errorMsg.str().c_str());
            break;
        } else {
//            std::cout << wordVec[i] << std::endl;
//            std::ostringstream errorMsg;
//            errorMsg << "Could write to a FIFO" << fd;
//            perror(errorMsg.str().c_str());
        }
    }
    std::cout << "Before second signaling: " << fileName << std::endl;
//    pthread_mutex_unlock(&mutexFileNames);
//    pthread_cond_signal(&condFIFOWritten);
    std::cout << "After second signaling: " << fileName << std::endl;


    std::cout << "Written " << fileName << std::endl;


    std::cout << std::endl;


//    const char* fifoFileName = str.c_str();


//    int fd = open(fifoFileName, O_RDWR);


    return arg;
}

void* reduce3(void *arg) {
    std::vector<std::vector<std::string>> s2dVec;

    pthread_mutex_lock(&mutexFileNames);
    // Repeat the while loop as long as there is a file to read from map3()
    while (fileNames.size() != NUM_MAP_THREADS) {
        pthread_cond_wait(&condFileNameRead, &mutexFileNames);
        std::cout << "Not all the files have been read" << std::endl;
        // pthread_cond_wait is equivalent to:
        // pthread_mutex_unlock(&mutexFileNames);
        // wait for signal on condFileNameRead;
        // pthread_mutex_lock(&mutexFileNames);
    }
    // Convert each file name into a file descriptor and store it in fds
    std::vector<int> fds;
    for (const auto &fileName: fileNames) {
        fds.push_back(open(fileName.c_str(), O_RDONLY));
//        read(fd, word,
//        std::cout << word << std::endl;
    }

    // As with Task 2, store the first word from each FIFO in words
    std::string line;
    std::vector<std::string> words;
    char word[MAX_WORD_LENGTH + 1];
    std::vector<int> indicesToErase;

    for (int i = 0; i < fds.size(); ++i) {
        // IF there is no line to read from a file
        if (read(fds[i], word, getBytesToRead(fileNames[i])) == 0) {
            indicesToErase.push_back(i);
        }
        words.emplace_back(word);
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
            if (words[i].substr(MIN_WORD_LENGTH - 1) < words[lowestOrderIdx].substr(MIN_WORD_LENGTH - 1)) {
                lowestOrderIdx = i;
            }
        }

        outputFile << words[lowestOrderIdx] << std::endl;

        // Read another word from the FIFO from which the lowest word was just found
        if (read(fds[lowestOrderIdx], word, getBytesToRead(fileNames[lowestOrderIdx])) != 0) {
            words[lowestOrderIdx] = word;
        } else {
            close(fds[lowestOrderIdx]);
            fds.erase(fds.begin() + lowestOrderIdx);
            fileNames.erase(fileNames.begin() + lowestOrderIdx);
            words.erase(words.begin() + lowestOrderIdx);
            lowestOrderIdx = 0;
        }
    }

    outputFile.close();

//        std::vector<std::string> sVec;
//        char word[wordLength];
//        std::cout <<  "before opening a readonly file " << fileName << std::endl;
//        fd = open(fileName, O_RDONLY);
//        pthread_cond_wait(&condFIFOWritten, &mutexCond);

//        for (int i = 0; i < numWords; ++i) {
//            read(fd, word, wordLength + 1);
//            sVec.push_back(std::string(word));
//        }

//        for (auto const &s : sVec) {
//            std::cout << s << std::endl;
//        }
//        numFilesRead++;



    pthread_mutex_unlock(&mutexCond);

//    }



    return arg;
}


void* map3(void* a) {
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

    // Create 13 threads for map3
    pthread_t th[NUM_MAP_THREADS];
    for (int i = 0; i < NUM_MAP_THREADS; ++i) {
        if (pthread_create(th + i, nullptr, &sort3, &iVecs[i]) != 0) {
            perror("Failed to create thread");
        }
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
    if (argc != 3) {
        std::cout << "Correct Usage: ./task2 DirtyFile CleanFile" << std::endl;
        exit(1);
    }
    // Assign the global vector of strings, wordVec
    wordVec = Task1Filter(argv[1]);

    // Create two threads for map3() and reduce3()
    pthread_t mapThread, reduceThread;
    pthread_mutex_init(&mutexFileNames, nullptr);
    pthread_mutex_init(&mutexCond, nullptr);
    pthread_mutex_init(&mutexTest, nullptr);
    pthread_cond_init(&condFileNameRead, nullptr);
    pthread_cond_init(&condFIFOWritten, nullptr);
    pthread_cond_init(&condFIFORead, nullptr);
    if (pthread_create(&mapThread, nullptr, &map3, nullptr) != 0) {
        perror("Failed to create map3 thread");
    }
    if (pthread_create(&reduceThread, nullptr, &reduce3, argv[2]) != 0) {
        perror("Failed to create reduce3 thread");
    }

    // Join two of the threads above
    if (pthread_join(mapThread, nullptr) != 0) {
        perror("Failed to join map3 thread");
    }
    if (pthread_join(reduceThread, nullptr) != 0) {
        perror("Failed to join reduce3 thread");
    }
    pthread_mutex_destroy(&mutexFileNames);
    pthread_mutex_destroy(&mutexCond);
    pthread_mutex_destroy(&mutexTest);
    pthread_cond_destroy(&condFileNameRead);
    pthread_cond_destroy(&condFIFOWritten);
    pthread_cond_destroy(&condFIFORead);


}