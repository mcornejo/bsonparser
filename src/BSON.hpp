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
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

class BSON {
private:
    std::vector<char> bson;
    std::vector<Value> documents;

    
    void ParseIterator(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType& allocator, bool IsArray = false);
    
    void ParseIteratorArray(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType& allocator);
    
    string ParseEName(std::vector<char>::iterator &it);
    
    void ParseDouble(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray); // 01

    void ParseString(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray); //02
    
    void ParseDocument(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray); //03
    
    void ParseArray(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray); // 04
    
    void ParseBinary(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray); // 05
    
    void ParseObjectID(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 07
    
    void ParseBoolean(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 08 
    
    void ParseUTCDatetime(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 09
    
    void ParseNull(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 10
    
    void ParseInt32(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 16
    
    void ParseTimestamp(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 17
    
    void ParseInt64(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray = false); // 18
    
    
public:
    BSON(std::vector<char> input);
    void Parse();
    void Dump();
};


#endif /* BSON_hpp */
