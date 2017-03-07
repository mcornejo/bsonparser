//
//  main.cpp
//  bsonparser
//

#include <iostream>
#include "BSON.hpp"
#include <fstream>

int main(int argc, const char * argv[]) {
    // insert code here...
    
    //std::ifstream input("identities.bson", std::ios::binary );
    std::ifstream input("clerks.bson", std::ios::binary );
    std::vector<char> buffer((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
    
    BSON parser(buffer);
    
    parser.Parse();
    
    cout << "Finished\n";
    
    cout << "Dumping\n";
    parser.Dump();
    
    return 0;
}
