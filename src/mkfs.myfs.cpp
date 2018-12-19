//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//
#include <fstream>
#include "myfs.h"

//zum testen vorübergenen eingefügt
#include "myfs.cpp"

#include "blockdevice.h"
#include "macros.h"

MyFS *my;

int main(int argc, char *argv[]) {

    //BlockDevice test;
    //test.create(argv[0]);


    // TODO: Implement file system generation & copying of files here
   const char test[] = {"/./../.././Hallo"};
   const char *a = test + 3;
   //printf("%c\n", *(a));
   //printf("%s\n", a);
   //printf("%d", my->getSizeOfCharArray(a));
   printf("%d", my->isFileExisting(test));
   return 0;
}

