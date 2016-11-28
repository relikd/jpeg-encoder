//
//  MarcelHuffmann.hpp
//  jpegenc
//
//  Created by Marcel Groß on 22.11.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef MarcelHuffmann_hpp
#define MarcelHuffmann_hpp

#include <stdio.h>
#include <vector>
#include <algorithm>

struct InputWord {
    int word;
    int amount;
    
    InputWord(int word) : word(word), amount(1) {}
    
    void increase();
    
    bool operator < (const InputWord& input) const {
        return (amount < input.amount);
    }
    
    bool operator == (const InputWord& input) const {
        return (amount == input.amount);
    }
};

class MarcelHuffmann {
    std::vector<InputWord> words;
    
public:
    MarcelHuffmann() {}
    
    void addToWords(std::vector<int>);
    
private:

    void sortByAppearance();
};







#endif /* MarcelHuffmann_hpp */
