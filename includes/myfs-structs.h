//
//  myfs-structs.h
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//
#include <math.h>

#ifndef myfs_structs_h
#define myfs_structs_h

#define NAME_LENGTH 255
#define BLOCK_SIZE 512
#define NUM_DIR_ENTRIES 64
#define NUM_OPEN_FILES 64

// TODO: Add structures of your file system here
#define NAME_FILESYSTEM "MYFS"

#define NUM_SUPER_BLOCKS  1
#define NUM_DMAP_BLOCKS 16
#define NUM_FAT_BLOCKS 256
#define NUM_ROOT_BLOCKS NUM_DIR_ENTRIES
#define NUM_DATA_BLOCKS 65536

#define START_SUPER_BLOCKS 0
#define START_DMAP_BLOCKS START_SUPER_BLOCKS + NUM_SUPER_BLOCKS
#define START_FAT_BLOCKS START_DMAP_BLOCKS + NUM_DMAP_BLOCKS
#define START_ROOT_BLOCKS START_FAT_BLOCKS + NUM_FAT_BLOCKS
#define START_DATA_BLOCKS START_ROOT_BLOCKS + NUM_ROOT_BLOCKS

//SBLOCK
#define NUM_NAME_FILESYSTEM_BYTE 4
#define NUM_RESERVED_ENTRIES_BYTE 1
#define NUM_RESERVED_BLOCKS_BYTE 3
#define NUM_RESERVED_DATA_BYTES_BYTE 4

#define START_NAME_FILESYSTEM_BYTE 0
#define START_RESERVED_ENTRIES_BYTE START_NAME_FILESYSTEM_BYTE + NUM_NAME_FILESYSTEM_BYTE
#define START_RESERVED_BLOCKS_BYTE START_RESERVED_ENTRIES_BYTE + NUM_RESERVED_ENTRIES_BYTE
#define START_RESERVED_DATA_BYTES_BYTE START_RESERVED_BLOCKS_BYTE + NUM_RESERVED_BLOCKS_BYTE

//FAT
#define ADRESS_LENGTH_BYTE 2
#define NUM_ADRESS_PER_BLOCK 512 / 2

//ROOT
#define NUM_FILENAME_BYTE NAME_LENGTH
#define NUM_FILE_SIZE_BYTE 4
#define NUM_USERID_BYTE 1
#define NUM_GROUPID_BYTE 1
#define NUM_ACCESS_RIGHT_BYTE 2
#define NUM_TIMESTAMP_BYTE 6
#define NUM_POINTER_BYTE 2

#define START_FILENAME_BYTE 0
#define START_FILE_SIZE_BYTE  START_FILENAME_BYTE +  NUM_FILENAME_BYTE
#define START_USERID_BYTE START_FILE_SIZE_BYTE + NUM_FILE_SIZE_BYTE
#define START_GROUPID_BYTE START_USERID_BYTE + NUM_USERID_BYTE
#define START_ACCESS_RIGHT_BYTE START_GROUPID_BYTE + NUM_GROUPID_BYTE
#define START_FIRST_TIMESTAMP_BYTE START_ACCESS_RIGHT_BYTE + NUM_ACCESS_RIGHT_BYTE
#define START_SECOND_TIMESTAMP_BYTE START_FIRST_TIMESTAMP_BYTE + NUM_TIMESTAMP_BYTE
#define START_THIRD_TIMESTAMP_BYTE START_SECOND_TIMESTAMP_BYTE + NUM_TIMESTAMP_BYTE
#define START_POINTER_BYTE START_THIRD_TIMESTAMP_BYTE + NUM_TIMESTAMP_BYTE


#endif /* myfs_structs_h */
