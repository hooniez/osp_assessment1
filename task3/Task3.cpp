//
// Created by hooniesun on 26/08/22.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define NUM_MAP_THREADS 13
#define NUM_INDEX_ARRAYS 13
#define MAX_RUN_TIME 10

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fstream>

#include <vector>
#include "../task1/Task1Filter.cpp"
#include <algorithm>
#include <sstream>
#include <cstring>

pthread_mutex_t mutexFileNames;
pthread_cond_t condFileNameRead;

std::vector<std::string> fileNames;
std::vector<std::string> wordVec;

pthread_mutex_t mutexNeedsToExit;
bool needsToExit = false;

inline int getBytesToRead(const std::string& fileName) {
    // Every file to be passed as an argument will start with 'fifo'.
    // The line below returns any number that follows the name fifo.
    // The number 4 below stands for the starting pos from which to read the number
    return stoi(fileName.substr(4,fileName.size() - 4)) + 1;
}

void* sort3(void *arg) {
    // If the graceful exiting is required
    pthread_mutex_lock(&mutexNeedsToExit);
    if (needsToExit) {
        std::cout << "worker thread exiting" << std::endl;
        pthread_mutex_unlock(&mutexNeedsToExit);
    } else {
        // Normal workflow
        pthread_mutex_unlock(&mutexNeedsToExit);
        auto* iVec = (std::vector<int>*)arg;
        std::cout << "Map's worker thread sorts the index array" << std::endl;
        // Sort the index array
        std::sort(iVec->begin(), iVec->end(), [] (const int a, const int b) {
            return wordVec[a].compare(MIN_WORD_LENGTH - 1, wordVec[a].size() - (MIN_WORD_LENGTH - 1), wordVec[b], MIN_WORD_LENGTH - 1, wordVec[b].size() - (MIN_WORD_LENGTH - 1)) < 0;
        });

        auto tid = (pid_t)syscall(__NR_gettid);
        std::cout << "sort3 | the thread id for fifo " << wordVec[*iVec->begin()].length() << " is: " << tid << std::endl;

        // Create a FIFO file for write
        pthread_mutex_lock(&mutexFileNames);
        std::ostringstream oss;
        oss << "fifo" << wordVec[*iVec->begin()].length();
        std::string tmp = oss.str();
        const char *fileName = tmp.c_str();
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
        }

        std::cout << "Map's worker thread writes to a FIFO" << std::endl;
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
    }
    return arg;
}

void* reduce3(void *arg) {
    // If the graceful exiting is required
    pthread_mutex_lock(&mutexNeedsToExit);
    if (needsToExit) {
        std::cout << "reduce3 exiting" << std::endl;
        pthread_mutex_unlock(&mutexNeedsToExit);
    } else {
        pthread_mutex_unlock(&mutexNeedsToExit);
        // Normal workflow
        auto tid = (pid_t)syscall(__NR_gettid);
        std::cout << "reduce3 | the thread id " << " is: " << tid << std::endl;

        pthread_mutex_lock(&mutexFileNames);
        // Repeat the while loop as long as there is a file to read from map5()
        while (fileNames.size() != NUM_MAP_THREADS) {
            pthread_cond_wait(&condFileNameRead, &mutexFileNames);
            // Graceful exiting
            pthread_mutex_lock(&mutexNeedsToExit);
            if (needsToExit) {
                std::cout << "reduce3 exiting" << std::endl;
                pthread_mutex_unlock(&mutexNeedsToExit);
                return nullptr;
            }
            pthread_mutex_unlock(&mutexNeedsToExit);
            std::cout << "reduce3() checks whether words can be reduced" << std::endl;
            // pthread_cond_wait is equivalent to:
            // pthread_mutex_unlock(&mutexFileNames);
            // wait for signal on condFileNameRead;
            // pthread_mutex_lock(&mutexFileNames);
        }
        std::cout << "reduce3() now reduces" << std::endl;
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
            // If there is no line to read from a file
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
        std::cout << "reduce3 reads from a FIFO" << std::endl;
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
    }

    return arg;
}


void* map3(void* a) {
    // If the graceful exiting is required
    pthread_mutex_lock(&mutexNeedsToExit);
    if (needsToExit) {
        std::cout << "map3 exiting" << std::endl;
        pthread_mutex_unlock(&mutexNeedsToExit);
    } else {
        pthread_mutex_unlock(&mutexNeedsToExit);
        // Normal workflow
        auto tid = (pid_t)syscall(__NR_gettid);
        std::cout << "map3 | the thread id " << " is: " << tid << std::endl;
        // Create 13 index arrays
        std::cout << "Create 13 index arrays" << std::endl;
        std::vector<std::vector<int>> iVecs;
        for (int i = 0; i < NUM_INDEX_ARRAYS; ++i) {
            std::vector<int> iVec;
            iVecs.push_back(iVec);
        }

        // Index the global array
        std::cout << "Index the global array" << std::endl;
        for (int i = 0; i < wordVec.size(); ++i) {
            size_t wordLength = wordVec[i].size();
            if (wordLength >= MIN_WORD_LENGTH && wordLength <= MAX_WORD_LENGTH) {
                iVecs[wordLength - MIN_WORD_LENGTH].push_back(i);
            }
        }

        // Create 13 threads for map5
        pthread_t th[NUM_MAP_THREADS];
        for (int i = 0; i < NUM_MAP_THREADS; ++i) {
            if (pthread_create(th + i, nullptr, &sort3, &iVecs[i]) != 0) {
                perror("Failed to create thread");
            } else {
                std::cout << "Worker thread " << i << " is created " << std::endl;
            }
        }

        // Join the threads
        for (int i = 0; i < NUM_MAP_THREADS; ++i) {
            if (pthread_join(th[i], nullptr) != 0) {
                perror("Failed to join thread");
            } else {
                std::cout << "Worker thread " << i << " is joined " << std::endl;
            }
        }
    }
    return a;
}



int main(int argc, char* argv[]) {
    auto tid = (pid_t)syscall(__NR_gettid);
    std::cout << "main | the thread id " << " is: " << tid << std::endl;
    if (argc != 3) {
        std::cout << "Correct Usage: ./Task3 DirtyFile CleanFile" << std::endl;
        exit(1);
    }
    // Assign the global vector of strings, wordVec
    std::ifstream input(argv[1]);
    std::cout << "Task1Filter filters input" << std::endl;
    wordVec = Task1Filter(input);
    input.close();

    // Create two threads for map5() and reduce3()
    pthread_t mapThread, reduceThread;
    pthread_mutex_init(&mutexFileNames, nullptr);
    pthread_mutex_init(&mutexNeedsToExit, nullptr);
    pthread_cond_init(&condFileNameRead, nullptr);
    if (pthread_create(&mapThread, nullptr, &map3, nullptr) != 0) {
        perror("Failed to create map5 thread");
    } else {
        std::cout << "Map thread is created" << std::endl;
    }
    if (pthread_create(&reduceThread, nullptr, &reduce3, argv[2]) != 0) {
        perror("Failed to create reduce3 thread");
    } else {
        std::cout << "Reduce thread is created" << std::endl;
    }

    // Sleep for graceful exiting
    sleep(MAX_RUN_TIME);
    pthread_mutex_lock(&mutexNeedsToExit);
    needsToExit = true;
    pthread_mutex_unlock(&mutexNeedsToExit);
    pthread_cond_signal(&condFileNameRead);

    // Join two of the threads above
    if (pthread_join(mapThread, nullptr) != 0) {
        perror("Failed to join map5 thread");
    } else {
        std::cout << "Map thread is joined" << std::endl;
    }
    if (pthread_join(reduceThread, nullptr) != 0) {
        perror("Failed to join reduce3 thread");
    } else {
        std::cout << "Reduce thread is joined" << std::endl;
    }
    pthread_mutex_destroy(&mutexFileNames);
    pthread_mutex_destroy(&mutexNeedsToExit);
    pthread_cond_destroy(&condFileNameRead);
}