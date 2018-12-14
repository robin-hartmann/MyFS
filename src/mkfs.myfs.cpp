//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//
#include <fstream>
#include "myfs.h"
#include "blockdevice.h"
#include "macros.h"

int main(int argc, char *argv[]) {

    //BlockDevice test;
    //test.create(argv[0]);


    // TODO: Implement file system generation & copying of files here
    char buffer[2];
    buffer[0] = 'b';
    buffer[1] = 'a';

    printf("%d\n", ((int) buffer[1]));
    printf("%d\n", ((int) buffer[0]));
    printf("%d\n", (((int) buffer[0])<<8) + ((int) buffer[1]));
    printf("TEst");
    return 0;
}

