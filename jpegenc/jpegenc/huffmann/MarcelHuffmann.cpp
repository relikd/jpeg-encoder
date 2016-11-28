//
//  MarcelHuffmann.cpp
//  jpegenc
//
//  Created by Marcel Groß on 22.11.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "MarcelHuffmann.hpp"

void InputWord::increase() {
    ++amount;
}



void MarcelHuffmann::addToWords(std::vector<int> input) {
    for (int i = 0; i < input.size(); ++i) {
        bool notFount = true;
        for (int j = 0; j < words.size(); ++j) {
            if (input[i] == words[j].word) {
                words[j].increase();
                notFount = false;
                break;
            }
        }
        if (notFount) {
            words.push_back(InputWord(input[i]));
        }
    
    }
    
    sortByAppearance();
}

void MarcelHuffmann::sortByAppearance() {
    std::sort(words.begin(), words.end());
}
