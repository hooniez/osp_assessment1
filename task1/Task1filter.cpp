#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

//
// Created by Myeonghoon Sun on 6/8/2022.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15

void Task1Filter(const std::string &readFileName, const std::string &writeFileName) {
    std::string line;
    std::vector<std::string> wordVec;
    std::ifstream input(readFileName);
    std::ofstream output(writeFileName);
    
    while (std::getline(input, line)) {
        if (line.size() >= MIN_WORD_LENGTH && line.size() <= MAX_WORD_LENGTH) {
            // Only insert words that contain English lower characters
            bool isAllLower = true;
            for (char ch: line) {
                if (!islower(ch)) {
                    isAllLower = false;
                }
            }
            if (isAllLower) {
                wordVec.push_back(line);
            }
        }
    }

    // Find out the square root of the size of wordVec
    int numRowsAndCols = (int)sqrt(wordVec.size());

    std::vector<std::vector<std::string>> matrix;
    // Store the words in a 2d array and transpose it to shuffle the words
    int index = 0;
    for (int row = 0; row <= numRowsAndCols; ++row) {
        std::vector<std::string> sVec;
        for (int col = 0; col <= numRowsAndCols; ++col) {
            if (index < wordVec.size()) {
                sVec.push_back(wordVec[index++]);
            } else {
                sVec.emplace_back("");
            }
        }
        matrix.push_back(sVec);
    }

    std::vector<std::string> finalVec;
    index = 0;
    for (size_t i = 0; i <= numRowsAndCols; ++i) {
        for (size_t j = 0; j < numRowsAndCols; ++j) {
            std::string word = matrix[j][i];
            if (!word.empty()) {
                finalVec.push_back(word);
            }
        }
    }


//    std::stable_sort(wordVec.begin(), wordVec.end(),
//		    [](const std::string &a, const std::string &b) {
//			return a.substr(MIN_WORD_LENGTH - 1) < b.substr(MIN_WORD_LENGTH - 1);
//		    });

    for (const auto& word : finalVec) {
	    output << word << std::endl;
    }
}


int main(int argc, char *argv[]) {
    Task1Filter("../input.txt", "../output.txt");



}
