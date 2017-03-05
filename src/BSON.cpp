//
//  BsonParser.cpp
//  bsonparser
//
//  Created by Mario Cornejo on 04/03/2017.
//  Copyright © 2017 Mario Cornejo. All rights reserved.
//



#include <vector>
#include <iostream>
#include "BSON.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define DEBUG true

using namespace std;
using namespace rapidjson;

BSON::BSON(std::vector<char> input){
    cout << "Object BSON created\n";
    bson = input;
    doc.SetObject();
}

void BSON::parseDocument(std::vector<char> input){
    Value v;
    Value document(kArrayType);
    
    // RECEIVE THE ARRAY CUT ALREADY. AND PARSE FROM BEGINING AND END.
    
    //return v;
}

void BSON::dump() {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::cout << buffer.GetString() << std::endl;
}

void BSON::parse() {
    
    if (bson.size() == 0)
        return;
    
    int size = 0;
    int i = 0;  // counter.
    // First process the first 4 bytes to get the int32 with the size:
    
    for (int i = 0; i < 4; ++i) {
        size += bson[i] << (i * 8);    // |= could have also been used
    }
    
    if(DEBUG){
    cout << "size: " << size << "\n";
    
    for(int i : bson){
        cout << "i: " << i << "\n";
    }
    cout << "END \n";
    }
    
    // main loop

    for (std::vector<char>::iterator it = bson.begin()+4 ; it != bson.end(); ++it, i++){
        
        if(DEBUG){
        cout << "i: "<< i << ":" << (int)*it << "\n";
        }
        
        switch((int)*it) {
            case 1:
            {
                string e_string_double = parseestring(++it);
                Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
                it = it + e_string_double.size() + 1;
                
                double num = parsedouble(it);
                Value numV(num);
                
                doc.AddMember(key_double, numV, allocator);
                if(DEBUG){
                    cout << "doub: " << e_string_double << "\n";
                    cout << "value: " << num << "\n";
                }
                break;
            }
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:  // ObjectId
            {
                string e_string = parseestring(++it);
                Value key(e_string.c_str(), (int)e_string.size(), allocator);
                // we move the pointer the size of the e_string
                it = it + e_string.size() + 1;
                
                string oid = extractObjectId(it);
                it = it + 11; // 12 bytes // test: 55a4d16cb3d4e1919101a856
                
                Value oidV(oid.c_str(), (int)oid.size(), allocator);
                
                doc.AddMember(key, oidV, allocator);
                
                if(DEBUG){
                cout << "oid: " << oid << "\n";
                cout << "e_string: " << e_string << "\n";
                }
                break;
            }
        }
    }
}


string BSON::extractObjectId(std::vector<char>::iterator it){
    char buffer[25];
    for(int i =0; i <12; i++){
        sprintf(&buffer[2*i], "%02x", (unsigned char)*it);
        cout << "it: " << (int)*it << "\n";
        it++;
    }
    
    return string(buffer);
}

string BSON::parseestring(std::vector<char>::iterator it){
    string output;
    while(*it != 0){
        output += *it;
        it++;
    }
    return output;
}

double BSON::parsedouble(std::vector<char>::iterator it){
    double output;
    char buffer[sizeof(double)];
    
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&output, buffer, sizeof(double));
    return output;
}


    
    // Let's continue with the first byte of the bson: the 5 element of the array.

