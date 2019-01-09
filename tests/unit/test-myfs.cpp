//
//  tests-myfs.cpp
//  testing
//
//  Created by Oliver Waldhorst on 15.12.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include "catch.hpp"


#include "helper.hpp"
#include "myfs.h"

#include <stdio.h>
#include <string.h>

// TODO: Implement your helper functions here!


TEST_CASE( "getSizeOfCharArray" ) {

    MyFS myfs;

    char test[5] = {"test"};
    REQUIRE(myfs.getSizeOfCharArray(test) == 4);

    char test2[0];
    REQUIRE(myfs.getSizeOfCharArray(test2) == 0);

    char test3[28];
    REQUIRE(myfs.getSizeOfCharArray(test3) == 0);

    const char test4[6] = "Hello";
    REQUIRE(myfs.getSizeOfCharArray(test4) == 5);

    const char test5[10] = {"test"};
    REQUIRE(myfs.getSizeOfCharArray(test5) == 4);
}


TEST_CASE( "intToChar / charToInt" ) {

    MyFS myfs;

    REQUIRE(myfs.charToInt("a") == 97);
    REQUIRE(myfs.charToInt("b") == 98);
    REQUIRE(myfs.charToInt("K") == 75);
    REQUIRE(myfs.charToInt("aa") == 24929);
    REQUIRE(myfs.charToInt("\0") == 0);

    REQUIRE(strcmp(myfs.intToChar(24929), "aa") == 0);
    REQUIRE(strcmp(myfs.intToChar(97), "a") == 0);
    REQUIRE(strcmp(myfs.intToChar(0), "\0") == 0);
}


TEST_CASE( "isFileExisting / getFilePosition" ) {

}


TEST_CASE( "readBlock" ) {

}


TEST_CASE( "getFileSize" ) {

}


TEST_CASE( "transferBytes" ) {

}