//
// Created by Lukas Hügle on 12.12.18.
//

#ifndef MYFS_MKFS_MYFS_H
#define MYFS_MKFS_MYFS_H

#endif //MYFS_MKFS_MYFS_H

#include <string>


int getsize(std::string &fileURL);
char* readFile(std::string fileURL);
int writeToDevice(char *array);