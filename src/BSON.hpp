//
//  BsonParser.hpp
//  bsonparser
//
//  Created by Mario Cornejo on 04/03/2017.
//  Copyright © 2017 Mario Cornejo. All rights reserved.
//

#ifndef BSON_hpp
#define BSON_hpp
#include <vector>
#include <stdio.h>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

class BSON {
private:
    std::vector<char> bson;
    std::vector<Value> documents;
    Value parseDocument(std::vector<char>::iterator begin, std::vector<char>::iterator end);
    
    Value &ParseIterator(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType& allocator);
    
    Value &ParseIteratorArray(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType& allocator);
    
    string ParseE_String(std::vector<char>::iterator &it);
    
    Value ParseObject(std::vector<char>::iterator begin, std::vector<char>::iterator end, Document::AllocatorType& allocator);
    
    Value &ParseArray(std::vector<char>::iterator begin, std::vector<char>::iterator end, Document::AllocatorType& allocator);
    string parseDatetime(std::vector<char>::iterator it);
    
    double ParseDouble(std::vector<char>::iterator &it); // 01
    string ParseString(std::vector<char>::iterator &it); // 02
    
    string ExtractObjectId(std::vector<char>::iterator &it); // 07
    
public:
    BSON(std::vector<char> input);
    void parse();
    void dump();
};


#endif /* BSON_hpp */
