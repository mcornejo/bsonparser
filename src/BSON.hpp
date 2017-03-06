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
    std::vector<Document> documents;
    Document parseDocument(std::vector<char>::iterator begin, std::vector<char>::iterator end);
    string parseestring(std::vector<char>::iterator it);
    
    double parsedouble(std::vector<char>::iterator it); // 01
    string parsestring(std::vector<char>::iterator it); // 02
    
    string extractObjectId(std::vector<char>::iterator it); // 07
    
public:
    BSON(std::vector<char> input);
    void parse();
    void dump();
};


#endif /* BSON_hpp */
