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
#include <string>

using namespace std;


int main(int argc, char *argv[]) {
    BlockDevice* device = new BlockDevice();
    device->create(argv[1]);
    MyFS filesystem ;
    size_t size;
    filesystem.blockDevice = device;
    fuse_file_info* dummyinfo = nullptr;
    mode_t mode;



    filesystem.writeROOT(0, ".", 0, "\0", "\0", "\0", "\0", "\0", "\0", 0);
    filesystem.writeROOT(1, "..", 0, "\0", "\0", "\0", "\0", "\0", "\0", 0);
    char* memblock;
    for (int i = 2; i < argc; ++i)
    {

        ifstream file (argv[i], ios::in|ios::binary|ios::ate);
        if (file.is_open())
        {
            size = file.tellg();
            memblock = new char [size];
            file.seekg (0, ios::beg);
            file.read (memblock, size);
            file.close();
        }
        string nameasString = argv[i];
        int pathsize = nameasString.size();
        cout<<getFilePath(argv[i], pathsize)<<endl;

     int errorCode =   filesystem.fuseCreate(getFilePath(argv[i], pathsize), mode, dummyinfo);
     cout<<errorCode<<endl;
       if(errorCode!= EEXIST) {
           filesystem.fuseOpen(getFilePath(argv[i], pathsize), dummyinfo);
           filesystem.fuseWrite(getFilePath(argv[i], pathsize), memblock, size, 0, dummyinfo);
           filesystem.fuseRelease(getFilePath(argv[i], pathsize), dummyinfo);
       }else{
           delete[] memblock;
           cout<<"error"<<endl;
           error("File alreasy exists");
       }
        delete[] memblock;
    }


       filesystem.fuseDestroy();
}

char* getFilePath(char* path, int size){
    bool found = false;
    int count = size-1;
    while(!found){
        if(path[count] == '/'){
            found = true;
            count++;
        }
        count--;
    }
    char* pointer;
    pointer = &path[count];


    return pointer;

}

