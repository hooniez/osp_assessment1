//
// Created by Myeonghoon Sun on 6/8/2022.
//

#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Task1Filter returns a vector of words that are filtered by the rule mentioned in README.md
// This function signature is required to implement Task 3 as specified in its first bullet point.
// Task1Filter utilises named return value optimisation
std::vector<std::string> Task1Filter(std::istream& input) {
    std::string line;
    std::vector<std::string> wordVec;

    while (std::getline(input, line)) {
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

    std::sort(wordVec.begin(), wordVec.end());
    wordVec.erase(unique(wordVec.begin(), wordVec.end()), wordVec.end());


    // I have spent extra hours to ensure the script's shuffled outcome is identical to that of its equivalent program in C++.
    // It was only after my great effort that I found out the code below is useless because of the requirement to shuffle has turned out to be unnecessary at odds with what the specification initially required.

    // Find out the square root of the size of wordVec
//    int numRowsAndCols = (int)sqrt(wordVec.size());
//
//    std::vector<std::vector<std::string>> matrix;
//    // Store the words in a 2d array and transpose it to shuffle the words
//    int index = 0;
//    for (int row = 0; row <= numRowsAndCols; ++row) {
//        std::vector<std::string> sVec;
//        for (int col = 0; col <= numRowsAndCols; ++col) {
//            if (index < wordVec.size()) {
//                sVec.push_back(wordVec[index++]);
//            } else {
//                sVec.emplace_back("");
//            }
//        }
//        matrix.push_back(sVec);
//    }
//
//    std::vector<std::string> finalVec;
//    index = 0;
//    for (size_t i = 0; i <= numRowsAndCols; ++i) {
//        for (size_t j = 0; j < numRowsAndCols; ++j) {
//            std::string word = matrix[j][i];
//            if (!word.empty()) {
//                finalVec.push_back(word);
//            }
//        }
//    }

    return wordVec;
}
