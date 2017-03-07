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
    if(DEBUG_APP) {
        cout << "Object BSON created\n";
    }
    bson = input;
    
}



void BSON::ParseIteratorArray(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType &allocator){
    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
        
        
        
        if(DEBUG_APP){
            cout << "iai"<< i << ":" << (int)*it << "\n";
        }
        
        switch((int)*it) {
            case 1:
            {
                ParseDouble(it, doc, allocator, true);
                break;
            }
            case 2:
            {
                cout << "here:\n";
                ParseString(it, doc, allocator, true);
                break;
            }
            case 3:
            {
                ParseDocument(it, doc, allocator, true);
                break;
            }
            case 4:
            {
                if (DEBUG_APP)
                    cout << "CASE 4\n";
                break;
            }
            case 5:
            {
                ParseBinary(it, doc, allocator, true);
                break;
            }
            case 6:
            {
                if (DEBUG_APP)
                    cout << "CASE 6\n";
                // Deprecated
                break;
            }
            case 7:  // ObjectId
            {
                
                ParseObjectID(it, doc, allocator, true);
                break;
            }
            case 8:
            {
                break;
            }
            case 9:
            {
                ParseUTCDatetime(it, doc, allocator, true);
                break;
            }
                
        }
    }
    
}


void BSON::ParseIterator(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType &allocator){
    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
        
        
        
        if(DEBUG_APP){
            cout << "ii"<< i << ":" << (int)*it << "\n";
        }
        
        switch((int)*it) {
            case 1:
            {
                ParseDouble(it, doc, allocator, false);
                break;
            }
            case 2:
            {
                ParseString(it, doc, allocator, false);
                break;
            }
            case 3:
            {
                ParseDocument(it, doc, allocator, false);
                cout << "parseDoc \n";
                break;
            }
            case 4:
            {
                ParseArray(it, doc, allocator, false);
                cout << "parseArra \n";
                break;
            }
            case 5:
            {
                ParseBinary(it, doc, allocator, false);
                break;
            }
            case 6:
            {
                if (DEBUG_APP)
                    cout << "CASE 6\n";
                // DEPRECATED
                break;
            }
            case 7:  // ObjectId
            {
                ParseObjectID(it, doc, allocator, false);
                break;
            }
            case 8:
            {
                break;
            }
            case 9: // Datetime
            {
                ParseUTCDatetime(it, doc, allocator, false);
                break;
            }
                
        }
    }
}



// Auxiliary Functios

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

string ExtractObjectId(std::vector<char>::iterator &it){
    char buffer[25];
    for(int i =0; i <12; i++){
        sprintf(&buffer[2*i], "%02x", (unsigned char)*it);
        cout << "it: " << (int)*it << "\n";
        it++;
    }
    it--;
    
    return string(buffer);
}

string ExtractString(std::vector<char>::iterator &it){
    string output;
    it += 4;
    while(*it != 0){
        output += *it;
        it++;
    }
    return output;
}

string ExtractBinaryAsString(std::vector<char>::iterator &it, int size){
    std::stringstream output;
    
    for(int i =0; i < size; i++){
        output << std::hex << (int)*it;
        it++;
    }
    return output.str();
}



void BSON::Dump() {
    
    for(auto & doc : documents){
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);
        std::cout << buffer.GetString() << std::endl;
    }
}




void BSON::Parse() {
    
    int total_size = (int)bson.size();
    int partial_size = 0;

    if (total_size == 0)
        return;
    else
        cout << "total_size: " << total_size << "\n";
    
    int i = 0;  // counter.
    
    cout << "SIZE size64: " << sizeof(int64_t) << "\n";
    
    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    // main loop

    for (std::vector<char>::iterator it = bson.begin() ; partial_size < total_size; ++it, i++){
        // First process the first 4 bytes to get the int32 with the size:
        int current_size = ExtractInt32(it);
        partial_size += current_size;
        
        if(current_size > total_size)
            return;
        
        //it += 4;
        
        if(DEBUG_APP == false){
            for(int i = 0; i < current_size; i++){
                cout << "i" << i << ":" << (int)bson[i] << "\n";
            }
            cout << "THE END\n";
        }
        
        cout << "current size: " << current_size << "\n";
        

        ParseIterator(it, it+current_size-4, doc, allocator);
        
        cout << "out[_id]: " << doc["_id"].GetString() << "\n";
        
        documents.push_back(doc.GetObject());
        
        
        it += current_size -5;  // the next byte to process -(4 + 1)

    }
}



string BSON::ParseEName(std::vector<char>::iterator &it){
    string output;
    while(*it != 0){
        output += *it;
        it++;
    }
    it++; // move away from \0
    return output;
}

string ExtractDatetime(std::vector<char>::iterator &it){
    int64_t mid;
    char buffer[sizeof(int64_t)];
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
        cout << "buffer[i]:" << buffer[i] << "\n";
    }
    it--;
    std::memcpy(&mid, buffer, sizeof(int64_t));
    ostringstream os;
    os << mid;
    return os.str();
}

double ExtractDouble(std::vector<char>::iterator &it){
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

bool ExtractBoolean(std::vector<char>::iterator &it){
    
    if ((int)*it == 0)
        return false;
    else
        return true;
}


void BSON::ParseDouble(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP)
        cout << "CASE 1\n";
    
    string e_string_double = ParseEName(++it);
    Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
    //it += e_string_double.size() + 1;
    
    double num = ExtractDouble(it);
    //it += 7; // this could be 7 as well.
    
    Value numV(num);
    
    if(IsArray){
        doc.PushBack(Value(numV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_double, allocator).Move(), Value(numV, allocator).Move(), allocator);
    }
    
    if(DEBUG_APP){
        cout << "doub: " << e_string_double << "\n";
        cout << "value: " << num << "\n";
    }
    
}


void BSON::ParseString(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    if (DEBUG_APP)
        cout << "CASE 2\n";
    
    string e_string_string = ParseEName(++it);
    Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);
    
    
    string stringVal = ExtractString(it);
    
    Value stringV(stringVal.c_str(), (int)stringVal.size(), allocator);
    
     if(IsArray){
         doc.PushBack(Value(stringV, allocator).Move(), allocator);
     } else {
         doc.AddMember(Value(key_string, allocator).Move(), Value(stringV, allocator).Move(), allocator);
     }
    
    if(DEBUG_APP){
        cout << "key: " << e_string_string << "\n";
        cout << "value: " << stringVal << "\n";
    }
}


void BSON::ParseDocument(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    if (DEBUG_APP){
        cout << "CASE 3\n";
    }
    
    string e_string_doc = ParseEName(++it);
    Value key_doc(e_string_doc.c_str(), (int)e_string_doc.size(), allocator);
    
    int doc_size = ExtractInt32(it);
    
    Value object_json(kObjectType);
    Value inner_doc(kObjectType);
    
    ParseIterator(it, it+doc_size -5, inner_doc, allocator);
    it += doc_size -5;
    //it++;
    if(IsArray){
        doc.PushBack(Value(inner_doc, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_doc, allocator).Move(), Value(inner_doc, allocator).Move(), allocator);
    }
}


void BSON::ParseArray(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP) {
        cout << "CASE 4\n";
    }
    
    string e_string_array = ParseEName(++it);
    Value key_array(e_string_array.c_str(), (int)e_string_array.size(), allocator);
    
    int doc_size = ExtractInt32(it);
    
    Value array_json(kArrayType);
    
    ParseIteratorArray(it, it+doc_size -5, array_json, allocator);
    it += doc_size -4;
    
    if(IsArray){
        doc.PushBack(Value(array_json, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_array, allocator).Move(), Value(array_json, allocator).Move(), allocator);
    }
}

void BSON::ParseBinary(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP)
        cout << "CASE 5\n";
    
    string e_string_string = ParseEName(++it);
    Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);

    int size = ExtractInt32(it);
    
    string output = ExtractBinaryAsString(it, size);
    
    Value stringV(output.c_str(), (int)output.size(), allocator);
    
    if(IsArray){
        doc.PushBack(Value(stringV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_string, allocator).Move(), Value(stringV, allocator).Move(), allocator);
    }

    
}


void BSON::ParseObjectID(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if(DEBUG_APP)
        cout << "CASE 7\n";
    
    string e_string = ParseEName(++it);
    Value key(e_string.c_str(), (int)e_string.size(), allocator);
    string oid = ExtractObjectId(it);
    Value oidV(oid.c_str(), (int)oid.size(), allocator);
    
    if(DEBUG_APP){
        cout << "e_string: " << e_string << "\n";
        cout << "oid: " << oid << "\n";
    }
    if(IsArray){
        doc.PushBack(Value(oidV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key, allocator).Move(), Value(oidV, allocator).Move(), allocator);
    }
}

void BSON::ParseBoolean(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    if(DEBUG_APP)
        cout << "CASE 8\n";

    string e_string = ParseEName(++it);
    Value key(e_string.c_str(), (int)e_string.size(), allocator);
    
    bool res = ExtractBoolean(it);
    Value resV(res);
    
    if(IsArray){
        doc.PushBack(Value(resV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key, allocator).Move(), Value(resV, allocator).Move(), allocator);
    }

    
}

void BSON::ParseUTCDatetime(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP)
        cout << "CASE 9\n";
    
    string e_string_datetime = ParseEName(++it);
    cout << "date_key: " << e_string_datetime << "\n";
    Value key_datetime(e_string_datetime.c_str(), (int)e_string_datetime.size(), allocator);
    
    string dateVal = ExtractDatetime(it);
    cout << "date: " << dateVal << "\n";
    Value dateV(dateVal.c_str(), (int)dateVal.size(), allocator);
    
    if(DEBUG_APP){
        cout << "date_key: " << e_string_datetime << "\n";
        cout << "date: " << dateVal << "\n";
    }
    
    if(IsArray){
        doc.PushBack(Value(dateV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_datetime, allocator).Move(), Value(dateV, allocator).Move(), allocator);
    }
}




