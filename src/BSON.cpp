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

int ExtractInt32(std::vector<char>::iterator &it){
    int32_t size ;
    
    char buffer[sizeof(int32_t)];
    
    for(int i = 0; i<4; i++){
        cout << "bufer " << i << ": " << (int)*it << "\n";
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&size, buffer, sizeof(int32_t));
    
    cout << "extractint32 " << size << "\n";
    return size;
}

Value &BSON::ParseIteratorArray(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType &allocator){
    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
        
        
        
        if(DEBUG_APP){
            cout << "ii"<< i << ":" << (int)*it << "\n";
        }
        
        switch((int)*it) {
            case 1:
            {
                if (DEBUG)
                    cout << "CASE 1\n";
                
                string e_string_double = ParseE_String(++it);
                Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
                //it += e_string_double.size() + 1;
                
                double num = ParseDouble(it);
                //it += 7; // this could be 7 as well.
                
                Value numV(num);
                
                doc.PushBack(numV, allocator);
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
                
                string e_string_string = ParseE_String(++it);
                Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);
                //it += e_string_string.size() + 1;  // string + \0
                //it += 1;
                string stringVal = ParseString(++it);
                //it += stringVal.size() + 1 + 3; // string + \0 + size_int
                it--;
                Value stringV(stringVal.c_str(), (int)stringVal.size(), allocator);
                
                doc.PushBack(stringV, allocator);
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
                string e_string_doc = ParseE_String(++it);
                Value key_doc(e_string_doc.c_str(), (int)e_string_doc.size(), allocator);
                //it += e_string_doc.size() + 1;  //  string + \0
                
                int doc_size = ExtractInt32(it);
                
                Value object_json(kObjectType);
                
                object_json = ParseObject(it, it+doc_size -4, allocator);
                it += doc_size -5;
                doc.PushBack(object_json, allocator);
                
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
                
                string e_string = ParseE_String(++it);
                Value key(e_string.c_str(), (int)e_string.size(), allocator);
                // we move the pointer the size of the e_string
                //it = it + e_string.size() + 1;
                
                string oid = ExtractObjectId(it);
                //it = it + 11; // 12 bytes // test: 55a4d16cb3d4e1919101a856
                
                Value oidV(oid.c_str(), (int)oid.size(), allocator);
                
                doc.PushBack(oidV, allocator);
                
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
                
                string e_string_datetime = ParseE_String(++it);
                cout << "date_key: " << e_string_datetime << "\n";
                Value key_datetime(e_string_datetime.c_str(), (int)e_string_datetime.size(), allocator);
                //it += e_string_datetime.size() + 1;  // string + \0
                
                string dateVal = parseDatetime(it);
                cout << "date: " << dateVal << "\n";
                Value dateV(dateVal.c_str(), (int)dateVal.size(), allocator);
                it += 7;
                
                if(DEBUG_APP){
                    cout << "date_key: " << e_string_datetime << "\n";
                    cout << "date: " << dateVal << "\n";
                }
                
                doc.PushBack(dateV, allocator);
                
                
                
                break;
            }
                
        }
    }
    
    return doc;
}


Value &BSON::ParseIterator(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType &allocator){
    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
        
        
        
        if(DEBUG_APP){
            cout << "ii"<< i << ":" << (int)*it << "\n";
        }
        
        switch((int)*it) {
            case 1:
            {
                if (DEBUG)
                    cout << "CASE 1\n";
                
                string e_string_double = ParseE_String(++it);
                Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
                //it += e_string_double.size() + 1;
                
                double num = ParseDouble(it);
                //it += 7; // this could be 7 as well.
                
                Value numV(num);
                
                doc.AddMember(key_double, numV.Move(), allocator);
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
                
                string e_string_string = ParseE_String(++it);
                Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);
                //it += e_string_string.size() + 1;  // string + \0
                //it += 1;
                string stringVal = ParseString(++it);
                //it += stringVal.size() + 1 + 3; // string + \0 + size_int
                it--;
                Value stringV(stringVal.c_str(), (int)stringVal.size(), allocator);
                
                doc.AddMember(key_string, stringV.Move(), allocator);
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
                string e_string_doc = ParseE_String(++it);
                Value key_doc(e_string_doc.c_str(), (int)e_string_doc.size(), allocator);
                //it += e_string_doc.size() + 1;  //  string + \0
                
                int doc_size = ExtractInt32(it);
                
                Value object_json(kObjectType);
                
                Value inner_doc(kObjectType);
                object_json = ParseIterator(it, it+doc_size -4, inner_doc, allocator).GetObject();
                
                cout << "object_json: " << &object_json << "\n";
                it += doc_size -5;
                doc.AddMember(key_doc, object_json.Move(), allocator);
                
                if(DEBUG){
                    cout << "END DOCUMENT: " << i << "\n";
                }
                
                break;
            }
            case 4:
            {
                if (DEBUG) {
                    cout << "CASE 4\n";
                    cout << "INIT ARRAY: " << i << "\n";
                }
                string e_string_array = ParseE_String(++it);
                Value key_doc(e_string_array.c_str(), (int)e_string_array.size(), allocator);
                
                int doc_size = ExtractInt32(it);
                
                Value array_json(kArrayType);
                
                array_json = ParseArray(it, it+doc_size -4, allocator);
                it += doc_size -5;
                doc.AddMember(key_doc, array_json, allocator);
                
                
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
                
                string e_string = ParseE_String(++it);
                Value key(e_string.c_str(), (int)e_string.size(), allocator);
                // we move the pointer the size of the e_string
                //it = it + e_string.size() + 1;
                
                string oid = ExtractObjectId(it);
                //it = it + 11; // 12 bytes // test: 55a4d16cb3d4e1919101a856
                
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
                
                string e_string_datetime = ParseE_String(++it);
                cout << "date_key: " << e_string_datetime << "\n";
                Value key_datetime(e_string_datetime.c_str(), (int)e_string_datetime.size(), allocator);
                //it += e_string_datetime.size() + 1;  // string + \0
                
                string dateVal = parseDatetime(it);
                cout << "date: " << dateVal << "\n";
                Value dateV(dateVal.c_str(), (int)dateVal.size(), allocator);
                it += 7;
                
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


Value BSON::ParseObject(std::vector<char>::iterator begin, std::vector<char>::iterator end, Document::AllocatorType& allocator){
    Value doc(kObjectType);
    //Value &cp = ParseIterator(begin, end, doc, allocator).GetObject();
    return ParseIterator(begin, end, doc, allocator).GetObject();
}

Value &BSON::ParseArray(std::vector<char>::iterator begin, std::vector<char>::iterator end, Document::AllocatorType& allocator){
    Value doc(kArrayType);
    //Value cp = ParseIteratorArray(begin, end, doc, allocator); //.GetArray();
    return ParseIteratorArray(begin, end, doc, allocator);
}



Value BSON::parseDocument(std::vector<char>::iterator begin, std::vector<char>::iterator end){

    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    return ParseIterator(begin, end, doc, allocator).GetObject();
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
        

        Document doc;
        Document::AllocatorType& allocator = doc.GetAllocator();
        doc.SetObject();
        
        documents.push_back(ParseIterator(it, it+current_size-4, doc, allocator).GetObject());
        it += current_size -5;  // the next byte to process -(4 + 1)

    }
}







string BSON::ParseE_String(std::vector<char>::iterator &it){
    string output;
    while(*it != 0){
        output += *it;
        it++;
    }
    it++;
    return output;
}

string BSON::parseDatetime(std::vector<char>::iterator it){  //09
    int64_t mid;
    char buffer[sizeof(int64_t)];
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
        cout << "buffer[i]:" << buffer[i] << "\n";
    }
    std::memcpy(&mid, buffer, sizeof(int64_t));
    ostringstream os;
    os << mid;
    return os.str();
}

double BSON::ParseDouble(std::vector<char>::iterator &it){  //01
    double output;
    char buffer[sizeof(double)];
    
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
    }
    it--;
    std::memcpy(&output, buffer, sizeof(double));
    return output;
}

string BSON::ParseString(std::vector<char>::iterator &it){ // 02
    string output;
    //int size = extractint32(it);
    it += 3;
    while(*it != 0){
        output += *it;
        it++;
    }
    it++;
    return output;
}



string BSON::ExtractObjectId(std::vector<char>::iterator &it){ // 07
    char buffer[25];
    for(int i =0; i <12; i++){
        sprintf(&buffer[2*i], "%02x", (unsigned char)*it);
        cout << "it: " << (int)*it << "\n";
        it++;
    }
    it--;
    
    return string(buffer);
}


    // Let's continue with the first byte of the bson: the 5 element of the array.

