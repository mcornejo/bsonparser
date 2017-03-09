//
// Mario Cornejo
//
//

#include <iostream>
#include "BSON.hpp"
#include <fstream>

int main(int argc, const char * argv[]) {
    /*
     * Some tests with different fields to parse
     */
    
    //std::ifstream input("identities.bson", std::ios::binary );
    //std::ifstream input("clerks.bson", std::ios::binary );
    //std::ifstream input("jobs.bson", std::ios::binary );
    
    
    if(argc < 2) {
       argv[1] = "mongodump.airpair.tags.bson";
    }
    
    /* Reading the file */
    std::ifstream input(argv[1], std::ios::binary );
    std::vector<char> buffer((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));

    /* Calling the constructor */
    BSON parser(buffer);
    
    /* Parse the file */
    parser.Parse();
    parser.Dump();
    
    return 0;
}
