#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

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

    std::stable_sort(wordVec.begin(), wordVec.end(),
		    [](const std::string &a, const std::string &b) {
			return a.substr(MIN_WORD_LENGTH - 1) < b.substr(MIN_WORD_LENGTH - 1);
		    }); 

    for (auto word : wordVec) {
	    output << word << std::endl;
    }
}


int main(int argc, char *argv[]) {
    Task1Filter("../input.txt", "output-cpp.txt");
}
