## BSON library

bsonparser is a library to help parsing BSON to JSON.
It is written in C++ for performance purporses.

## Overview

It parses all the common cases of BSON:
- Double
- UTF-8 string
- Embedded document
- Array
- Binary
- ObjectId
- Boolean
- UTC datetime
- Null
- 32-bit integer
- Timestamp
- 64-bit integer

## Usage
To get the code, first clone this repository, then compile 
the code using `make`. The binary is generated in the bin folder 
along with some sample files.

### Get The Code
```bash
$ git clone https://github.com/mcornejo/bsonparser
$ cd bsonparser
$ make 
```

### Usage
There is a hardcoded test in the folder `samples/`. To 
test the library just run:

```bash
$ cd bin
$ ./bsonparser
```

### Testing
Inside the `samples/` folder there are other files to test the library which 
are copied automatically
by the `Makefile`.


```bash
$ ./bsonparser <file>
$ ./bsonparser test.bson
$ ./bsonparser mongodump.airpair.tags.bson
$ ./bsonparser clerks.bson
$ ./bsonparser identities.bson
$ ./bsonparser jobs.bson
```

## Internal
The `BSON` constructor takes as input a `vector<char>` and it contains two methods:

- `BSON::Parse()`: This method parses the binary byte by byte. It does not iterate 
more than once over a BSON file once it is loaded into memory. It pushes each document 
found into a `vector<Document>`.

- `BSON::Dump()`: This method prints the value of the BSON in JSON format in the terminal.

#### Importing to Xcode
The repository contains all the Xcode project files to import it easily.

#### Dependencies
This library relies on the [RapidJSON](http://rapidjson.org) library 
to store the parsed JSON. The version 1.1.0 of RapidJSON is along the code in the 
`include` folder.