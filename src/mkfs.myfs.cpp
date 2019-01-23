//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//
#include <iostream>
#include "myfs.h"
#include "myfs.cpp"

#include "blockdevice.h"
#include "macros.h"
#include "mkfs.myfs.h"
#include  <fstream>

using namespace std;


int main(int argc, char *argv[]) {
    BlockDevice* device = new BlockDevice();
    device->create(argv[1]);
      char *array;
    MyFS filesystem ;
    size_t size;
    filesystem.blockDevice = device;
    fuse_file_info* dummyinfo = nullptr;
    mode_t mode;


    for (int i = 2; i < argc; ++i)
    {
        ifstream file (argv[i], ifstream::binary);
        size = file.tellg();
        char array[size];
        file.read(array, size);
        file.close();
        cout<<array<<endl;
        filesystem.fuseCreate(argv[i], mode, dummyinfo);
        filesystem.fuseWrite(argv[i], array,size, 0, dummyinfo );
    }


       filesystem.fuseDestroy();
}


/**
int getsize(std::string fileURL){
    std::ifstream file( fileURL, std::ios::binary | std::ios::ate);
    return file.tellg();
}


*/