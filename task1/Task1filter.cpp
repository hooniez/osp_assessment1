#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include <memory>
//
// Created by Myeonghoon Sun on 6/8/2022.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15

void Task1Filter(const std::string &readFileName, const std::string &writeFileName) {
    std::string line;
    std::vector<std::string> wordVec;
    std::ifstream input(readFileName);
    while (std::getline(input, line)) {
        if (line.size() >= MIN_WORD_LENGTH &&
            line.size() <= MAX_WORD_LENGTH &&
            std::regex_match(line, std::regex("^[a-zA-Z]+$"))) {
            wordVec.push_back(line);
        }
    }
    for (auto word : wordVec) {
        std::cout << word << std::endl;
    }
}

int main(int argc, char *argv[]) {
    Task1Filter("../input.txt", "shit");
}

