//
//  tests-myfs.cpp
//  testing
//
//  Created by Oliver Waldhorst on 15.12.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include "catch.hpp"

#include <iostream>
#include "helper.hpp"
#include "myfs.h"

#include <stdio.h>
#include <string.h>


using namespace std;

// TODO: Implement your helper functions here!


TEST_CASE( "intToChar / charToInt" ) {

    MyFS myfs;

    char test[3] = "a";
    REQUIRE(myfs.charToInt(test, 1) == 97);

    strcpy(test, "b");
    REQUIRE(myfs.charToInt(test, 1) == 98);

    strcpy(test, "K");
    REQUIRE(myfs.charToInt(test, 1) == 75);

    strcpy(test, "aa");
    REQUIRE(myfs.charToInt(test, 2) == 24929);

    strcpy(test, "\0");
    REQUIRE(myfs.charToInt(test, 1) == 0);

    strcpy(test, "ba");
    REQUIRE(myfs.charToInt(test, 2) == 24930);


    char buffer[5];
    buffer[2] = '\0';
    myfs.intToChar(24929, buffer, 2);
    REQUIRE(strcmp(buffer, "aa") == 0);

    buffer[1] = '\0';
    myfs.intToChar(97, buffer, 1);
    REQUIRE(strcmp(buffer, "a") == 0);

    buffer[1] = '\0';
    myfs.intToChar(0, buffer, 1);
    REQUIRE(strcmp(buffer, "\0") == 0);

    buffer[2] = '\0';
    myfs.intToChar(24930, buffer, 2);
    REQUIRE(strcmp(buffer, "ba") == 0);
    
    char testBuffer[4];
    int testnumber = 1981;
    
    myfs.intToChar(testnumber, testBuffer, 4);
    REQUIRE(testnumber == myfs.charToInt(testBuffer, 4));

    testnumber = 61601;
    myfs.intToChar(testnumber, testBuffer, 4);
    REQUIRE(testnumber == myfs.charToInt(testBuffer, 4));

    testnumber = (int) time(NULL);
    myfs.intToChar(testnumber, testBuffer, 4);
    REQUIRE(testnumber == myfs.charToInt(testBuffer, 4));

    testnumber = (int) S_IFREG | 0644;
    myfs.intToChar(testnumber, testBuffer, 4);
    REQUIRE(testnumber == myfs.charToInt(testBuffer, 4));
}

TEST_CASE("remDirPath"){
    MyFS myfs;

    char* testDirectoryPath = "./test/lukas/newfile";
    REQUIRE(strcmp(myfs.remDirPath(testDirectoryPath), "newfile") == 0);


}

TEST_CASE("sizeToBlocks"){
    MyFS myfs;
    REQUIRE(myfs.sizeToBlocks(1000) == 2);

    REQUIRE(myfs.sizeToBlocks(2000) == 4);

}

