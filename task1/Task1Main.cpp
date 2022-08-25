//
// Created by hooniesun on 25/08/22.
//
#include <iostream>
#include <vector>
#include "Task1Filter.cpp"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Correct Usage: ./Task1filter DirtyFile CleanFile" << std::endl;
        exit(1);
    }
    std::vector<std::string> finalVec = Task1Filter(argv[1]);

    const std::string writeFileName = argv[2];
    std::ofstream output(writeFileName);
    for (const auto& word : finalVec) {
        output << word << std::endl;
    }
}