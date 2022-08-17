#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <memory>
#include <sstream>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define NUM_CHILDREN 13

void reduce() {

    // Store pointers to ifstream objects (files) in a vector
    std::vector<std::shared_ptr<std::ifstream>> inputFiles;
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        std::ostringstream filename;
        filename << "output" << i << ".txt";
        inputFiles.push_back(std::make_shared<std::ifstream>(filename.str()));
    }

    // Store the first word from each ifstream in words
    std::string line;
    std::vector<std::string> words;
    std::vector<int> indicesToErase;
    for (int i = 0; i < inputFiles.size(); ++i) {
        // IF there is no line to read from a file
        if (!std::getline(*inputFiles[i], line)) {
            indicesToErase.push_back(i);
        }
        words.push_back(line);
    }

    // If there were any empty files, remove the corresponding positions from inputFiles and words
    for (auto indexToErase: indicesToErase) {
        inputFiles.erase(inputFiles.begin() + indexToErase);
        words.erase(words.begin() + indexToErase);
        inputFiles[indexToErase]->close();
    }

    // Write the lowest word in lexical order to a file
    int lowestOrderIdx = 0;
    std::ofstream outputFile("finalOutput.txt");
    while (!inputFiles.empty()) {
        // Find the actual index that contains the lowest word in lexical order
        for (int i = 0; i < words.size(); ++i) {
            if (words[i].substr(MIN_WORD_LENGTH - 1) < words[lowestOrderIdx].substr(MIN_WORD_LENGTH - 1)) {
                lowestOrderIdx = i;
            }
        }

        outputFile << words[lowestOrderIdx] << std::endl;

        // Read another word from the ifstream from which the lowest word was just found
        if (std::getline(*inputFiles[lowestOrderIdx], line)) {
            words[lowestOrderIdx] = line;
        } else {
            inputFiles.erase(inputFiles.begin() + lowestOrderIdx);
            words.erase(words.begin() + lowestOrderIdx);
            lowestOrderIdx = 0;
            inputFiles[lowestOrderIdx]->close();
        }
    }

    outputFile.close();
}

void map2(const std::string &readFileName) {
    std::string line;
    std::vector<std::string> wordVec3;
    std::vector<std::string> wordVec4;
    std::vector<std::string> wordVec5;
    std::vector<std::string> wordVec6;
    std::vector<std::string> wordVec7;
    std::vector<std::string> wordVec8;
    std::vector<std::string> wordVec9;
    std::vector<std::string> wordVec10;
    std::vector<std::string> wordVec11;
    std::vector<std::string> wordVec12;
    std::vector<std::string> wordVec13;
    std::vector<std::string> wordVec14;
    std::vector<std::string> wordVec15;
    std::ifstream input(readFileName);

    while (std::getline(input, line)) {
        size_t lineLength = line.size();
        if (lineLength == 3) {
            wordVec3.push_back(line);
        } else if (lineLength == 4) {
            wordVec4.push_back(line);
        } else if (lineLength == 5) {
            wordVec5.push_back(line);
        } else if (lineLength == 6) {
            wordVec6.push_back(line);
        } else if (lineLength == 7) {
            wordVec7.push_back(line);
        } else if (lineLength == 8) {
            wordVec8.push_back(line);
        } else if (lineLength == 9) {
            wordVec9.push_back(line);
        } else if (lineLength == 10) {
            wordVec10.push_back(line);
        } else if (lineLength == 11) {
            wordVec11.push_back(line);
        } else if (lineLength == 12) {
            wordVec12.push_back(line);
        } else if (lineLength == 13) {
            wordVec13.push_back(line);
        } else if (lineLength == 14) {
            wordVec14.push_back(line);
        } else if (lineLength == 15) {
            wordVec15.push_back(line);
        }
    }

    input.close();

    std::vector<std::vector<std::string>> wordVec;
    wordVec.push_back(wordVec3);
    wordVec.push_back(wordVec4);
    wordVec.push_back(wordVec5);
    wordVec.push_back(wordVec6);
    wordVec.push_back(wordVec7);
    wordVec.push_back(wordVec8);
    wordVec.push_back(wordVec9);
    wordVec.push_back(wordVec10);
    wordVec.push_back(wordVec11);
    wordVec.push_back(wordVec12);
    wordVec.push_back(wordVec13);
    wordVec.push_back(wordVec14);
    wordVec.push_back(wordVec15);

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pid_t pid = fork();
        if (pid <0) {
            perror("fork failed.");
            exit(1);
        } else if (pid == 0) {
            std::stable_sort(wordVec[i].begin(), wordVec[i].end(),
                             [](const std::string &a, const std::string &b) {
			return a.substr(MIN_WORD_LENGTH - 1) < b.substr(MIN_WORD_LENGTH - 1);
		    });

            std::ofstream output("output" + std::to_string(i) + ".txt");
            for (auto word : wordVec[i]) {
                output << word << std::endl;
            }
            output.close();
            exit(0);
        }

    }

    printf("Parent waits on child process\n");
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        wait(NULL);
    }
    reduce();

}

int main(int argc, char *argv[]) {
    map2("../output.txt");

}