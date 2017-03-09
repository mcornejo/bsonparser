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

#define DEBUG_APP false

using namespace std;
using namespace rapidjson;

/*
 * Constructor. Takes a vector of chars as input and sets the bson property.
 */
BSON::BSON(std::vector<char> input){
    if(DEBUG_APP) {
        cout << "Object BSON created\n";
    }
    bson = input;
    
}


/*
 * ParseIterator: It Parses each document and converts it in the repective JSON object
 * From BSON specs: It reads the first byte and pass a pointer to the respective function
 * to parse the payload of each element.
 */

void BSON::ParseIterator(std::vector<char>::iterator begin, std::vector<char>::iterator end, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    int i = 0;
    for (std::vector<char>::iterator it = begin ; it != end; ++it, i++){
    
        
        
        if(DEBUG_APP){
            cout << "ii"<< i << ":" << (int)*it << "\n";
            cout << "ii size: " << end - it << "\n";
        }
        
        switch((int)*it) {
            
            case 0:
            {
                continue;
                break;
            }
            case 1:
            {
                ParseDouble(it, doc, allocator, IsArray);
                break;
            }
            case 2:
            {
                ParseString(it, doc, allocator, IsArray);
                break;
            }
            case 3:
            {
                ParseDocument(it, doc, allocator, IsArray);
                break;
            }
            case 4:
            {
                ParseArray(it, doc, allocator, IsArray);
                break;
            }
            case 5:
            {
                ParseBinary(it, doc, allocator, IsArray);
                break;
            }
            case 6:
            {
                // DEPRECATED
                break;
            }
            case 7:
            {
                ParseObjectID(it, doc, allocator, IsArray);
                break;
            }
            case 8:
            {
                ParseBoolean(it, doc, allocator, IsArray);
                break;
            }
            case 9:
            {
                ParseUTCDatetime(it, doc, allocator, IsArray);
                break;
            }
                case 10:
            {
                ParseNull(it, doc, allocator, IsArray);
                break;
            }
            case 16:
            {
                ParseInt32(it, doc, allocator, IsArray);
                break;
            }
            case 17:
            {
                ParseTimestamp(it, doc, allocator, IsArray);
                break;
            }
            case 18:
            {
                ParseInt64(it, doc, allocator, IsArray);
                break;
            }
            default:
            {
                ParseBinary(it, doc, allocator, IsArray);
                break;
            }
        }
        
        if(end - it < 0)
            return;
    }
}



/* Auxiliary Functions:
 * Helpers to extract information from the payload.
 */
int ExtractInt32(std::vector<char>::iterator &it){
    int32_t size ;
    
    char buffer[sizeof(int32_t)];
    
    for(int i = 0; i<4; i++){
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&size, buffer, sizeof(int32_t));
    if(DEBUG_APP)
        cout << "extractint32 " << size << "\n";
    return size;
}

int64_t ExtractInt64(std::vector<char>::iterator &it){
    int64_t size ;
    
    char buffer[sizeof(int64_t)];
    
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&size, buffer, sizeof(int64_t));
    if(DEBUG_APP)
        cout << "extractint64 " << size << "\n";
    return size;
}

uint64_t ExtractUint64(std::vector<char>::iterator &it){
    uint64_t size ;
    
    char buffer[sizeof(uint64_t)];
    
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
    }
    std::memcpy(&size, buffer, sizeof(uint64_t));
    if(DEBUG_APP)
        cout << "extract_uint64 " << size << "\n";
    return size;
}

string ExtractObjectId(std::vector<char>::iterator &it){
    char buffer[25];
    for(int i =0; i <12; i++){
        sprintf(&buffer[2*i], "%02x", (unsigned char)*it);
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

string ExtractDatetime(std::vector<char>::iterator &it){
    int64_t mid;
    char buffer[sizeof(int64_t)];
    for(int i = 0; i<8; i++){
        buffer[i] = (int)*it;
        it++;
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
/*
 * End of helpers
 */



/*
 * Dump: It prints the content of the JSON parsed
 */
void BSON::Dump() {
    for(auto & doc : documents){
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);
        std::cout << buffer.GetString() << std::endl;
    }
}

/*
 * Parse: It parses the array of bytes, converting each top level
 * document into a entry in the final JSON object.
 */

void BSON::Parse() {
    int total_size = (int)bson.size();
    int partial_size = 0;

    if (total_size == 0)
        return;
    
    
    int i = 0;  // counter.
    
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
        
        
        if(DEBUG_APP){
            for(int i = 0; i < current_size; i++){
                cout << "i" << i << ":" << (int)bson[i] << "\n";
            }
            cout << "THE END\n";
        }

        // Iterates through the document
        ParseIterator(it, it+current_size-5, doc, allocator);
        
        //Pushes each document parsed into the final JSON object.
        documents.push_back(doc.GetObject());
        
        
        it += current_size -5;  // the next byte to process -(4 + 1)

    }
}




/*
 * ExtractDatetime: It parses the e_name of each element
 */



/*
 * ParseEName: It parses the e_name of each element
 */
string BSON::ParseEName(std::vector<char>::iterator &it){
    string output;
    while(*it != 0){
        output += *it;
        it++;
    }
    it++; // move away from \0
    return output;
}

/*
 * ParseDouble: It parses a double value of an element
 */
void BSON::ParseDouble(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP)
        cout << "CASE 1\n";
    
    string e_string_double = ParseEName(++it);
    Value key_double(e_string_double.c_str(), (int)e_string_double.size(), allocator);
    
    double num = ExtractDouble(it);
    
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

/*
 * ParseString: It parses a string value of an element
 */
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

/*
 * ParseInt32: It parses a int32 value of an element
 */
void BSON::ParseInt32(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP)
        cout << "CASE 16\n";

    string e_string_string = ParseEName(++it);
    Value key_string(e_string_string.c_str(), (int)e_string_string.size(), allocator);
    
    int32_t intVal = ExtractInt32(it);
    Value intV(intVal);
    it--;
    if(IsArray){
        doc.PushBack(Value(intV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_string, allocator).Move(), Value(intV, allocator).Move(), allocator);
    }
}

/*
 * ParseDocument: It parses an embedded document value of an element
 * This method calls ParseIterator _recursively_. Then it moves the pointer to the final 
 * of the block parsed
 */
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
    
    if(IsArray){
        doc.PushBack(Value(inner_doc, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_doc, allocator).Move(), Value(inner_doc, allocator).Move(), allocator);
    }
}

/*
 * ParseArray: It parses an array value of an element
 * This method calls ParseIterator _recursively_ with the IsArray flag = true. 
 * Then it moves the pointer to the final of the block/array parsed
 */
void BSON::ParseArray(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP) {
        cout << "CASE 4\n";
    }
    
    string e_string_array = ParseEName(++it);
    Value key_array(e_string_array.c_str(), (int)e_string_array.size(), allocator);
    
    int doc_size = ExtractInt32(it);
    
    Value array_json(kArrayType);
    
    ParseIterator(it, it+doc_size -5, array_json, allocator, true);
    it += doc_size -4;
    
    if(IsArray){
        doc.PushBack(Value(array_json, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_array, allocator).Move(), Value(array_json, allocator).Move(), allocator);
        it--;
    }
}

/*
 * ParseBinary: It parses a binary value of an element
 */
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


/*
 * ParseObjectID: It parses a ObjectID value of an element. The value is stored as String
 */
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

/*
 * ParseBoolean: It parses a boolean value of an element.
 */
void BSON::ParseBoolean(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    if(DEBUG_APP)
        cout << "CASE 8\n";

    string e_string = ParseEName(++it);
    Value key(e_string.c_str(), (int)e_string.size(), allocator);
    
    bool res = ExtractBoolean(it);
    Value resV(res);
    
    if(DEBUG_APP){
        cout << "e_string: " << e_string << "\n";
        cout << "bool: " << res << "\n";
    }
    
    if(IsArray){
        doc.PushBack(Value(resV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key, allocator).Move(), Value(resV, allocator).Move(), allocator);
    }
}

/*
 * ParseUTCDatetime: It parses a UTC datatime value of an element.
 */
void BSON::ParseUTCDatetime(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    if (DEBUG_APP)
        cout << "CASE 9\n";
    
    string e_string_datetime = ParseEName(++it);
    Value key_datetime(e_string_datetime.c_str(), (int)e_string_datetime.size(), allocator);
    
    string dateVal = ExtractDatetime(it);
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

/*
 * ParseNull: It parses a null "value" of an element.
 */
void BSON::ParseNull(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){ // 10
    
    if(DEBUG_APP)
        cout << "CASE 10\n";
    
    string e_string = ParseEName(++it);
    it--;
    Value key(e_string.c_str(), (int)e_string.size(), allocator);
    
    Value resV;
    
    if(IsArray){
        doc.PushBack(Value(resV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key, allocator).Move(), Value(resV, allocator).Move(), allocator);
    }
}

/*
 * ParseTimestamp: It parses a timestamp value of an element.
 */
void BSON::ParseTimestamp(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    if (DEBUG_APP)
        cout << "CASE 17\n";
    
    string e_string = ParseEName(++it);
    Value key_double(e_string.c_str(), (int)e_string.size(), allocator);
    
    uint64_t num = ExtractUint64(it);
    it--;
    Value numV(num);
    
    if(IsArray){
        doc.PushBack(Value(numV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_double, allocator).Move(), Value(numV, allocator).Move(), allocator);
    }
    
    if(DEBUG_APP){
        cout << "key_timestamp: " << e_string << "\n";
        cout << "value: " << num << "\n";
    }
}

/*
 * ParseInt64: It parses a int64 value of an element.
 */
void BSON::ParseInt64(std::vector<char>::iterator &it, Value &doc, Document::AllocatorType &allocator, bool IsArray){
    
    if (DEBUG_APP)
        cout << "CASE 18\n";
    
    string e_string = ParseEName(++it);
    Value key_double(e_string.c_str(), (int)e_string.size(), allocator);
    
    int64_t num = ExtractInt64(it);
    it--;
    Value numV(num);
    
    if(IsArray){
        doc.PushBack(Value(numV, allocator).Move(), allocator);
    } else {
        doc.AddMember(Value(key_double, allocator).Move(), Value(numV, allocator).Move(), allocator);
    }
    
    if(DEBUG_APP){
        cout << "key_int64: " << e_string << "\n";
        cout << "value: " << num << "\n";
    }
}

