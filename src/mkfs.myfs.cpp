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

    BlockDevice test;
    test.create(argv[0]);


    // TODO: Implement file system generation & copying of files here

    return 0;
}

