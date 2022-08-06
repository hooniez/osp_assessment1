#include <fstream>
#include <iostream>
//
// Created by Myeonghoon Sun on 6/8/2022.
//

int main(int argc, char *argv[]) {
    std::string word;
    std::ifstream readFile(argv[1]);
    while (std::getline(readFile, word)) {
        std::cout << word << std::endl;
    }


}