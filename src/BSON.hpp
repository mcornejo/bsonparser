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
    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    
    void parseDocument(std::vector<char> input);
    string extractObjectId(std::vector<char>::iterator it);
    string parseestring(std::vector<char>::iterator it);
    double parsedouble(std::vector<char>::iterator it);
public:
    BSON(std::vector<char> input);
    void parse();
    void dump();
};


#endif /* BSON_hpp */
