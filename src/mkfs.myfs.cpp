//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//
#include <iostream>
#include "myfs.h"
#include "blockdevice.h"
#include "macros.h"
#include "mkfs.myfs.h"
#include  <fstream>


int main(int argc, char *argv[]) {
/**
    BlockDevice device;
    device.create(argv[1]);
    if(device.open(argv[1]) < 0)
    {
        std::cout<<"Cannot open Container file\n";
        return 1;
    }else{

            for (int i = 2; i < argc; i++) {
              //  char *array = readFile(argv[i]);
              //  error("test");

        }
    }

    return 0;
    */
}

/**

int getsize(std::string &fileURL){
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
*/