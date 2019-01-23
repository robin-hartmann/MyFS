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

int main(int argc, char *argv[]) {
    BlockDevice device;
    device.create(argv[1]);
    const char* filename;
    char *array;
    MyFS filesystem;

    filesystem.blockDevice = &device;
    fuse_file_info* dummyinfo = nullptr;
    mode_t mode;

    if(device.open(argv[1]) < 0)
    {
        std::cout<<"Cannot open Container file\n";
        return 1;
    }else{

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

    return 0;

}


int getsize(std::string fileURL){
    std::ifstream file( fileURL, std::ios::binary | std::ios::ate);
    return file.tellg();
}


char* readFile(std::string fileURL){
    int size = getsize(fileURL);
    std::cout<<size<<std::endl;
    std::ifstream inputFile (fileURL);
    char* arr ;
    arr = new char[size];

    if (inputFile.good()) {
        for (int i = 0; i < size; i++) {
            inputFile >> arr[i];
        }

        inputFile.close();

    }else {
        return nullptr;
    }

    return arr;
}
/**
mode_t getFilePermission(char* filename){
    struct stat sb;

    return sb.st_mode;
}
*/