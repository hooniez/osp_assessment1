//
// Created by hooniesun on 26/08/22.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define NUM_MAP_THREADS 13
#define NUM_INDEX_ARRAYS 13
#define NICEST_VALUE 19
#define STREAM_SIZE 1000000
//#define STREAM_SIZE 100
#define DELAY_MICROSECONDS 100000


#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <chrono>
#include <random>
#include <string.h>

#include <vector>
#include "../task1/Task1Filter.cpp"
#include <algorithm>
#include <sstream>
#include <map>
#include <memory>

struct Argument {
    int identifier;
    std::vector<std::vector<std::string>>* streamingWords;
};

pthread_mutex_t mutexNumWords;
unsigned long numWords;
bool fileAllRead = false;

std::vector<std::string> sVec;

pthread_mutex_t mutexIsFinished;
pthread_cond_t condIsFinished;

// All worker threads have 0 task to begin with
std::vector<int> numTasks (NUM_MAP_THREADS, 0);

pthread_mutex_t mutexNumTasks;
pthread_cond_t condNumTasks;

bool streamAvailable = false;

enum Finished {
    none,
    stream,
    map,
};

Finished isFinished = none;

std::ofstream output("output.txt");


pthread_mutex_t mutexFileNames;
pthread_cond_t condFileNameRead;

std::vector<std::string> fileNames;
std::vector<std::string> wordVec;

std::vector<std::vector<std::string>> words3;
std::vector<std::string> reducedWords3;
std::vector<std::vector<std::string>> words4;
std::vector<std::string> reducedWords4;
std::vector<std::vector<std::string>> words5;
std::vector<std::string> reducedWords5;
std::vector<std::vector<std::string>> words6;
std::vector<std::string> reducedWords6;
std::vector<std::vector<std::string>> words7;
std::vector<std::string> reducedWords7;
std::vector<std::vector<std::string>> words8;
std::vector<std::string> reducedWords8;
std::vector<std::vector<std::string>> words9;
std::vector<std::string> reducedWords9;
std::vector<std::vector<std::string>> words10;
std::vector<std::string> reducedWords10;
std::vector<std::vector<std::string>> words11;
std::vector<std::string> reducedWords11;
std::vector<std::vector<std::string>> words12;
std::vector<std::string> reducedWords12;
std::vector<std::vector<std::string>> words13;
std::vector<std::string> reducedWords13;
std::vector<std::vector<std::string>> words14;
std::vector<std::string> reducedWords14;
std::vector<std::vector<std::string>> words15;
std::vector<std::string> reducedWords15;




//std::map<int, int> tweakedNiceValues = {{3,  19}, {4, 7}, {5, 3}, {6, 1},
//                                        {7,  0}, {8, 1}, {9, 0}, {10, 3},
//                                        {11, 6}, {12, 7 }, {13, 13}, {14, 14},
//                                        {15, 19}};

//std::map<int, int> initialNiceValues = {{3, 12}, {4, 7}, {5, 2}, {6, 1},
//                                           {7, 0}, {8, 1}, {9, 1}, {10, 2},
//                                           {11, 3}, {12, 4 }, {13, 7}, {14, 7},
//                                           {15, 9}};

inline int getBytesToRead(const std::string& fileName) {
    // Every file to be passed as an argument will start with 'fifo'.
    // The line below returns any number that follows the name fifo.
    // The number 4 below stands for the starting pos from which to read the number
    return stoi(fileName.substr(4,fileName.size() - 4)) + 1;
}

void* sort5(void *arg) {
    auto argument = (Argument *)arg;
    std::cout << "argument identifier: " << argument->identifier<< std::endl;

    unsigned long totalNumWords = 0;
    pthread_mutex_lock(&mutexIsFinished);
    while (!fileAllRead) {
        pthread_mutex_unlock(&mutexIsFinished);

        std::vector<std::string>* wordsToProcess;
        pthread_mutex_lock(&mutexNumTasks);
        while (numTasks[argument->identifier] == 0) {
            pthread_cond_wait(&condNumTasks, &mutexNumTasks);
        }
        auto& wordsVec = *argument->streamingWords;
        wordsToProcess = &wordsVec[wordsVec.size() - numTasks[argument->identifier]];
        totalNumWords += wordsToProcess->size();

        // First decrement the number of tasks for the thread
        numTasks[argument->identifier] -= 1;
        pthread_mutex_unlock(&mutexNumTasks);

        // Sort the given vector
        std::sort(wordsToProcess->begin(), wordsToProcess->end(), [](const auto& a, const auto& b) {
           return a.compare(MIN_WORD_LENGTH - 1, a.size() - (MIN_WORD_LENGTH - 1), b, MIN_WORD_LENGTH - 1, b.size() - (MIN_WORD_LENGTH) - 1) < 0;
        });
//        std::cout << argument->identifier << ": " <<  wordsToProcess->size() << std::endl;

        pthread_mutex_lock(&mutexIsFinished);
    }
    pthread_mutex_unlock(&mutexIsFinished);

    // Merge the intermediary results
    std::vector<std::string> reducedWords;
    reducedWords.reserve(totalNumWords);
    for (const auto & vec: *argument->streamingWords) {
        reducedWords.insert(reducedWords.end(), vec.begin(), vec.end());
    }
    // Sort it
    std::sort(reducedWords.begin(), reducedWords.end(), [](const auto& a, const auto& b) {
        return a.compare(MIN_WORD_LENGTH - 1, a.size() - (MIN_WORD_LENGTH - 1), b, MIN_WORD_LENGTH - 1, b.size() - (MIN_WORD_LENGTH) - 1) < 0;
    });

    int wordLength = argument->identifier + MIN_WORD_LENGTH;

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

    // Store the file names and send a signal to the reduce5 thread
    fileNames.emplace_back(fileName);
    pthread_mutex_unlock(&mutexFileNames);
    pthread_cond_signal(&condFileNameRead);

    int fd = open(fileName, O_WRONLY );
    if (fd == -1) {
        perror("Error in opening a FIFO file");
    }

    // Write words to fifo
    for (auto const &word : reducedWords) {
        if (write(fd, word.c_str(), word.length() + 1) == -1) {
            std::ostringstream errorMsg;
            errorMsg << "Could not write to fifo" << fd;
            perror(errorMsg.str().c_str());
            break;
        }
    }

    close(fd);

    // Free later in main
    free(arg);

    return arg;
}

void* reduce5(void *arg) {
    pid_t tid = syscall(__NR_gettid);
    std::cout << "reduce5 | the thread id " << " is: " << tid << std::endl;

    pthread_mutex_lock(&mutexFileNames);
    // Repeat the while loop as long as there is a file to read from map5()
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


void* map5(void* arg) {
    pid_t x = syscall(__NR_gettid);
    std::cout << "map5 | the thread id " << " is: " << x << std::endl;

    // Create worker threads and pass to each fo them a vector it is to sort

    // First store the vectors inside another vector for distribution
    std::vector<std::vector<std::vector<std::string>>> wordsVec;
    wordsVec.push_back(words3);
    wordsVec.push_back(words4);
    wordsVec.push_back(words5);
    wordsVec.push_back(words6);
    wordsVec.push_back(words7);
    wordsVec.push_back(words8);
    wordsVec.push_back(words9);
    wordsVec.push_back(words10);
    wordsVec.push_back(words11);
    wordsVec.push_back(words12);
    wordsVec.push_back(words13);
    wordsVec.push_back(words14);
    wordsVec.push_back(words15);

    pthread_t th[NUM_MAP_THREADS];
    for (int i = 0; i < NUM_MAP_THREADS; ++i) {
        auto *argument = static_cast<Argument *>(malloc(sizeof(struct Argument)));
        (*argument).identifier = i;
        (*argument).streamingWords = &wordsVec[i];

        if (pthread_create(th + i, nullptr, &sort5, (void*)argument) != 0) {
            perror("Failed to create thread");
        }
    }

    const char *fifoName = (char *)arg;
    // word is of length MAX_WORD_LENGTH + 2 as it needs to contain the null terminating character and as well as
    // another character to check whether the word is longer than 15 characters.
    char word[MAX_WORD_LENGTH + 2];
    size_t wordLength;
    int fd;

    while (!fileAllRead) {
        fd = open(fifoName, O_RDONLY);
        if (fd == -1) {
            perror("Error in opening a FIFO file for reading");
        }

        // Create vectors to pass to worker threads to sort
        std::vector<std::string> tempWord3;
        std::vector<std::string> tempWord4;
        std::vector<std::string> tempWord5;
        std::vector<std::string> tempWord6;
        std::vector<std::string> tempWord7;
        std::vector<std::string> tempWord8;
        std::vector<std::string> tempWord9;
        std::vector<std::string> tempWord10;
        std::vector<std::string> tempWord11;
        std::vector<std::string> tempWord12;
        std::vector<std::string> tempWord13;
        std::vector<std::string> tempWord14;
        std::vector<std::string> tempWord15;
        std::vector<std::vector<std::string>> tempWordVec;
        tempWordVec.push_back(tempWord3);
        tempWordVec.push_back(tempWord4);
        tempWordVec.push_back(tempWord5);
        tempWordVec.push_back(tempWord6);
        tempWordVec.push_back(tempWord7);
        tempWordVec.push_back(tempWord8);
        tempWordVec.push_back(tempWord9);
        tempWordVec.push_back(tempWord10);
        tempWordVec.push_back(tempWord11);
        tempWordVec.push_back(tempWord12);
        tempWordVec.push_back(tempWord13);
        tempWordVec.push_back(tempWord14);
        tempWordVec.push_back(tempWord15);

        // Read words
        for (int i = 0; i < STREAM_SIZE; ++i) {
            if (read(fd, word, MAX_WORD_LENGTH + 2) == -1) {
                perror("Couldn't read from FIFO");
            } else {
                // Map words to appropriate vectors
                wordLength = strlen(word);
                if (wordLength >= MIN_WORD_LENGTH && wordLength <= MAX_WORD_LENGTH) {

                    tempWordVec[wordLength - MIN_WORD_LENGTH].push_back(word);
                }
            }
        }

        // Update the global vectors that store words and the number of tasks for each thread
        pthread_mutex_lock(&mutexNumTasks);

        for (int i = 0; i < wordsVec.size(); ++i) {
            wordsVec[i].push_back(tempWordVec[i]);
        }

        for (auto &num: numTasks) {
            ++num;
        }
        pthread_cond_broadcast(&condNumTasks);
        pthread_mutex_unlock(&mutexNumTasks);

        close(fd);

        pthread_mutex_lock(&mutexIsFinished);
        // If the map thread finishes first, wait until the main thread finishes processing and then sends a cond_signal
        if (isFinished == none) {
            std::cout << "Map thread has finished processing FIFO first" << std::endl;
            isFinished = map;
            if (!fileAllRead) {
                pthread_cond_wait(&condIsFinished, &mutexIsFinished);
            }
        } else if (isFinished == stream) {
            // If the main thread finishes first, send a cond_signal to the main thread
            std::cout << "Main thread has finished processing FIFO first and Map wakes up Main" << std::endl;
            pthread_mutex_unlock(&mutexIsFinished);
            pthread_cond_signal(&condIsFinished);
            isFinished = none;
        }
        pthread_mutex_unlock(&mutexIsFinished);
    }

    // Join the threads
    for (int i = 0; i < NUM_MAP_THREADS; ++i) {

        if (pthread_join(th[i], nullptr) != 0) {
            perror("Failed to join thread");
        }
    }





    return arg;
}



int main(int argc, char* argv[]) {
    pid_t x = syscall(__NR_gettid);
    std::cout << "main | the thread id " << " is: " << x << std::endl;

    /*
     * As Ron said in the discussion forum, this program reads a dirty file using std::in
     * Please execute this program like this: cat DirtyFile | ./Task5 CleanFile
     */
    if (argc != 2) {
        std::cout << "Correct Usage: cat DirtyFile | ./Task5 CleanFile" << std::endl;
        exit(1);
    }

    // Read the whole original (dirty) file into a string array within Task1Filter and then
    // apply the filtering rule as well as remove duplicates. After that, store the result in sVec.
    sVec = Task1Filter(std::cin);

    // For outputting a random entry to a FIFO file, shuffle sVec.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(sVec.begin(), sVec.end(), std::default_random_engine(seed));

    // Create a fifo file to enable communication with map5 thread
    const char *fifoName = "stream";
    if (mkfifo(fifoName, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
        }
    }

    pthread_t mapThread;
    pthread_t reduceThread;
    pthread_mutex_init(&mutexNumWords, nullptr);
    pthread_mutex_init(&mutexIsFinished, nullptr);
    pthread_mutex_init(&mutexNumTasks, nullptr);
    pthread_cond_init(&condIsFinished, nullptr);
    pthread_cond_init(&condNumTasks, nullptr);

    // Create map thread
    if (pthread_create(&mapThread, nullptr, &map5, (void *)fifoName) != 0) {
        perror("Failed to create map5 thread");
    }

    // Create reduce thread
    if (pthread_create(&reduceThread, nullptr, &reduce5, argv[1]) != 0) {
        perror("Failed to create reduce5 thread");
    }


    int fd = open(fifoName, O_WRONLY);
    if (fd == -1) {
        perror("Error in opening a FIFO file for writing");
    }

    unsigned long currIdx = 0;
    unsigned long maxIdx = sVec.size() - 1;
    std::string invalidWord;
    while (currIdx <= maxIdx) {
        for (int i = 0; i < STREAM_SIZE; i++) {
            // If there is a word in a vector to write
            if (currIdx <= maxIdx) {
                if (write(fd, sVec[currIdx].c_str(), MAX_WORD_LENGTH + 2) == -1) {
                    perror("Could not write to FIFO");
                }
            } else {
                // Write an invalid word to be processed as invalid
                if (write(fd, invalidWord.c_str(), MAX_WORD_LENGTH + 2) == -1) {
                    perror("Could not write to FIFO");
                }
            }
            currIdx += 1;
        }

        pthread_mutex_lock(&mutexIsFinished);
        // If the main thread finishes first, wait until the map thread finishes processing and then wait for DELAY_MICROSECONDS
        if (isFinished == none) {
            std::cout << "Main thread has finished processing FIFO first" << std::endl;
            isFinished = stream;
            if (currIdx > maxIdx) {
                fileAllRead = true;
            }
            pthread_cond_wait(&condIsFinished, &mutexIsFinished);
            usleep(DELAY_MICROSECONDS);
        } else if (isFinished == map) {
            // If the map thread finishes first, send a cond_signal to the map thread and then wait for DELAY_MICROSECONDS
            std::cout << "Map thread has finished processing FIFO first and Main wakes up Map" << std::endl;
            if (currIdx > maxIdx) {
                fileAllRead = true;
            }
            pthread_mutex_unlock(&mutexIsFinished);
            pthread_cond_signal(&condIsFinished);
            isFinished = none;
            usleep(DELAY_MICROSECONDS);
        }
        pthread_mutex_unlock(&mutexIsFinished);


        std::cout << currIdx << " words have been processed" << std::endl;
    }
    close(fd);

    if (pthread_join(mapThread, nullptr) != 0) {
        perror("Failed to join map5 thread");
    }

    if (pthread_join(reduceThread, nullptr) != 0) {
        perror("Failed to join reduce5 thread");
    }

    pthread_mutex_destroy(&mutexNumWords);
    pthread_mutex_destroy(&mutexIsFinished);
    pthread_mutex_destroy(&mutexNumTasks);
    pthread_cond_destroy(&condIsFinished);
    pthread_cond_destroy(&condNumTasks);

}