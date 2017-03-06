//
//  BsonParser.cpp
//  bsonparser
//
//  Created by Mario Cornejo on 04/03/2017.
//  Copyright © 2017 Mario Cornejo. All rights reserved.
//



#include <vector>
#include <iostream>
#include <sstream>
#include "BSON.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define DEBUG_APP true

using namespace std;
using namespace rapidjson;

BSON::BSON(std::vector<char> input){
    cout << "Object BSON created\n";
    bson = input;
    
}

int extractint32(std::vector<char>::iterator it){
    int size ;
    
    char buffer[sizeof(int)];
    
    for(int i = 0; i<4; i++){
        cout << "bufer " << i << ": " << (int)*it << "\n";
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&size, buffer, sizeof(size));
    
    cout << "extractint32 " << size << "\n";
    return size;
}


Value BSON::parseObject(std::vector<char>::iterator begin, std::vector<char>::iterator end, Document::AllocatorType& allocator){
    Value doc(kObjectType);
    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
        
        if(i > 1000)
            return doc;
        
        if(DEBUG_APP){
            cout << "ii"<< i << ":" << (int)*it << "\n";
        }
        
        switch((int)*it) {
            case 1:
            {
                if (DEBUG)
                    cout << "CASE 1\n";
                
                string e_string_double = parseestring(++it);
                Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
                it += e_string_double.size() + 1;
                
                double num = parsedouble(it);
                it += 7; // this could be 7 as well.
                
                Value numV(num);
                
                doc.AddMember(key_double, numV, allocator);
                if(DEBUG_APP){
                    cout << "doub: " << e_string_double << "\n";
                    cout << "value: " << num << "\n";
                }
                break;
            }
            case 2:
            {
                if (DEBUG)
                    cout << "CASE 2\n";
                
                string e_string_string = parseestring(++it);
                Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);
                it += e_string_string.size() + 1;  // string + \0
                
                string stringVal = parsestring(it);
                it += stringVal.size() + 1 + 3; // string + \0 + size_int
                
                Value stringV(stringVal.c_str(), (int)stringVal.size(), allocator);
                
                doc.AddMember(key_string, stringV, allocator);
                if(DEBUG_APP){
                    cout << "key: " << e_string_string << "\n";
                    cout << "value: " << stringVal << "\n";
                }
                
                break;
            }
            case 3:
            {
                if (DEBUG){
                    cout << "CASE 3\n";
                    cout << "INIT DOCUMENT: " << i << "\n";
                }
                string e_string_doc = parseestring(++it);
                Value key_doc(e_string_doc.c_str(), (int)e_string_doc.size(), allocator);
                it += e_string_doc.size() + 1;  //  string + \0
                
                int doc_size = extractint32(it);
                it += 4;
                
                Value object_json(kObjectType);
                
                object_json = parseObject(it, it+doc_size -4, allocator);
                it += doc_size -5;
                doc.AddMember(key_doc, object_json, allocator);
                
                if(DEBUG){
                    cout << "END DOCUMENT: " << i << "\n";
                }
                
                break;
            }
            case 4:
            {
                if (DEBUG)
                    cout << "CASE 4\n";
                break;
            }
            case 5:
            {
                if (DEBUG)
                    cout << "CASE 5\n";
                break;
            }
            case 6:
            {
                if (DEBUG)
                    cout << "CASE 6\n";
                break;
            }
            case 7:  // ObjectId
            {
                
                if (DEBUG)
                    cout << "CASE 7\n";
                
                string e_string = parseestring(++it);
                Value key(e_string.c_str(), (int)e_string.size(), allocator);
                // we move the pointer the size of the e_string
                it = it + e_string.size() + 1;
                
                string oid = extractObjectId(it);
                it = it + 11; // 12 bytes // test: 55a4d16cb3d4e1919101a856
                
                Value oidV(oid.c_str(), (int)oid.size(), allocator);
                
                doc.AddMember(key, oidV, allocator);
                
                if(DEBUG_APP){
                    cout << "oid: " << oid << "\n";
                    cout << "e_string: " << e_string << "\n";
                }
                break;
            }
        }
    }
    
    return doc;
}



Document BSON::parseDocument(std::vector<char>::iterator begin, std::vector<char>::iterator end){

    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
        
        if(DEBUG_APP){
            cout << "i"<< i << ":" << (int)*it << "\n";
        }
        
        if(i > 1000)
            return doc;
        
        switch((int)*it) {
            case 1:
            {
                if (DEBUG)
                    cout << "CASE 1\n";
                
                string e_string_double = parseestring(++it);
                Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
                it += e_string_double.size() + 1;
                
                double num = parsedouble(it);
                it += 7; // this could be 7 as well.
                
                Value numV(num);
                
                doc.AddMember(key_double, numV, allocator);
                if(DEBUG_APP){
                    cout << "double: " << e_string_double << "\n";
                    cout << "value: " << num << "\n";
                }
                break;
            }
            case 2:
            {
                if (DEBUG)
                    cout << "CASE 2\n";
                
                string e_string_string = parseestring(++it);
                Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);
                it += e_string_string.size() + 1;  // string + \0
                
                string stringVal = parsestring(it);
                it += stringVal.size() + 1 + 3; // string + \0 + size_int
                
                Value stringV(stringVal.c_str(), (int)stringVal.size(), allocator);
                
                doc.AddMember(key_string, stringV, allocator);
                if(DEBUG_APP){
                    cout << "key: " << e_string_string << "\n";
                    cout << "value: " << stringVal << "\n";
                }
                
                break;
            }
            case 3:
            {
                if (DEBUG){
                    cout << "CASE 3\n";
                    cout << "DOC START: " << i << "\n";
                
                }
                string e_string_doc = parseestring(++it);
                Value key_doc(e_string_doc.c_str(), (int)e_string_doc.size(), allocator);
                it += e_string_doc.size() + 1;  //  string + \0
                
                int doc_size = extractint32(it);
                it += 4;
                Value object_json(kObjectType);
                
                object_json = parseObject(it, it+doc_size -4, allocator);
                it += doc_size - 5;
                doc.AddMember(key_doc, object_json, allocator);
                
                if(DEBUG){
                    cout << "DOC FINISH: " << i << "\n";
                }
                
                break;
            }
            case 4:
            {
                if (DEBUG)
                    cout << "CASE 4\n";
                break;
            }
            case 5:
            {
                if (DEBUG)
                    cout << "CASE 5\n";
                break;
            }
            case 6:
            {
                if (DEBUG)
                    cout << "CASE 6\n";
                break;
            }
            case 7:  // ObjectId
            {
                
                if (DEBUG)
                    cout << "CASE 7\n";
                
                string e_string = parseestring(++it);
                Value key(e_string.c_str(), (int)e_string.size(), allocator);
                // we move the pointer the size of the e_string
                it = it + e_string.size() + 1;
                
                string oid = extractObjectId(it);
                it = it + 11; // 12 bytes // test: 55a4d16cb3d4e1919101a856
                
                Value oidV(oid.c_str(), (int)oid.size(), allocator);
                
                doc.AddMember(key, oidV, allocator);
                
                if(DEBUG_APP){
                    cout << "oid: " << oid << "\n";
                    cout << "e_string: " << e_string << "\n";
                }
                break;
            }
            case 8:
            {
                break;
            }
            case 9:
            {
                if (DEBUG)
                    cout << "CASE 9\n";

                string e_string_datetime = parseestring(++it);
                cout << "date_key: " << e_string_datetime << "\n";
                Value key_datetime(e_string_datetime.c_str(), (int)e_string_datetime.size(), allocator);
                it += e_string_datetime.size() + 1;  // string + \0

                string dateVal = parseDatetime(it);
                cout << "date: " << dateVal << "\n";
                Value dateV(dateVal.c_str(), (int)dateVal.size(), allocator);
                it += dateVal.size() +1;
                
                if(DEBUG_APP){
                    cout << "date_key: " << e_string_datetime << "\n";
                    cout << "date: " << dateVal << "\n";
                }
                
                doc.AddMember(key_datetime, dateV, allocator);
                


                break;
            }
        }
    }

    return doc;
}

void BSON::dump() {
    
    for(auto &&doc : documents){
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);
        std::cout << buffer.GetString() << std::endl;
    }
}




void BSON::parse() {
    
    int total_size = (int)bson.size();
    int partial_size = 0;

    if (total_size == 0)
        return;
    else
        cout << "total_size: " << total_size << "\n";
    
    int i = 0;  // counter.
    
    cout << "SIZE size64: " << sizeof(int64_t) << "\n";
    
    
    // main loop

    for (std::vector<char>::iterator it = bson.begin() ; partial_size < total_size; ++it, i++){
        // First process the first 4 bytes to get the int32 with the size:
        int current_size = extractint32(it);
        partial_size += current_size;
        
        if(current_size > total_size)
            return;
        
        it += 4;
        
        if(DEBUG_APP == false){
            for(int i = 0; i < current_size; i++){
                cout << "i" << i << ":" << (int)bson[i] << "\n";
            }
            cout << "THE END\n";
        }
        
        cout << "current size: " << current_size << "\n";
        


        documents.push_back(parseDocument(it, it+current_size-4));
        
        it += current_size -5;  // the next byte to process -(4 + 1)

    }
}







string BSON::parseestring(std::vector<char>::iterator it){
    string output;
    while(*it != 0){
        output += *it;
        it++;
    }
    return output;
}

string BSON::parseDatetime(std::vector<char>::iterator it){  //09
    int64_t mid;
    char buffer[sizeof(int64_t)];
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
        cout << "datebuff" << i << ": " << (int)*it << "\n";
    }
    std::memcpy(&mid, buffer, sizeof(int64_t));
    cout << "output: " << mid << "\n";
    ostringstream os;
    os << mid;
    return os.str();
}

double BSON::parsedouble(std::vector<char>::iterator it){  //01
    double output;
    char buffer[sizeof(double)];
    
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&output, buffer, sizeof(double));
    return output;
}

string BSON::parsestring(std::vector<char>::iterator it){ // 02
    string output;
    //int size = extractint32(it);
    it += 4;
    output = parseestring(it);
    return output;
}



string BSON::extractObjectId(std::vector<char>::iterator it){ // 07
    char buffer[25];
    for(int i =0; i <12; i++){
        sprintf(&buffer[2*i], "%02x", (unsigned char)*it);
        cout << "it: " << (int)*it << "\n";
        it++;
    }
    
    return string(buffer);
}


    // Let's continue with the first byte of the bson: the 5 element of the array.

