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

#define DEVICEPATH "/home/robin/Schreibtisch/MyFs/test"
int main(int argc, char *argv[]) {
    BlockDevice device;
    device.create(argv[1]);
    const char* filename;
     char *array;
    MyFS filesystem;

int main(int argc, char *argv[]) {
    BlockDevice* device = new BlockDevice();
    device->create(argv[1]);
    MyFS filesystem ;
    size_t size;
    filesystem.blockDevice = device;
    fuse_file_info* dummyinfo = nullptr;
    mode_t mode;


            for (int i = 2; i < argc; i++) {
                filesystem.fuseOpendir("/", nullptr);
                filename = "/English";
                array = readFile(argv[i]);
                filesystem.fuseCreate(filename,mode,dummyinfo ); //
                size_t size = getsize(argv[i]);
                filesystem.fuseOpen(filename, nullptr);
                filesystem.fuseWrite(filename, array,size, 0, dummyinfo );
                filesystem.fuseRelease(filename, nullptr);
            }
    }


       filesystem.fuseDestroy();
}


/**
int getsize(std::string fileURL){
    std::ifstream file( fileURL, std::ios::binary | std::ios::ate);
    return file.tellg();
}


*/