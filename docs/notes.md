# Notes

## Part 1: Read-Only File System

### [Specification](spec.md#Part-1:-Read-Only-File-System)

### Part 1a

Definition of the file system's structure and of the software architecture.

### Part 1b

Creation and initialization of the block device using `mkfs.myfs <container-file> [file1, file2, ...]`.

- to read the attributes of the original files use `stat()`
- fuse operations are not needed necessarily
    - `BlockDevice` can be used directly
    - but operations could be implemented and re-used by the next step

### Part 1c

Mouting the block device using `mount.myfs <container-file> <log-file> <mount-dir>`

- can be dismounted using `fusermount --unmount`

### Part 1d

Extensive testing

- calling `mount.myfs -s` causes fuse to run in single mode for easier debugging

## Part 2: Read-Write File System

### [Specification](spec.md#Part-2:-Read-Write-File-System)

## Part 3: Documentation

## Additional Information

### Deadline for Submission: 30.01.19

### [libfuse documentation](http://libfuse.github.io/doxygen/)

### [libfuse examples](https://github.com/libfuse/libfuse/tree/master/example)

### Structure of MyFS container files (minimum)
- Super Block
    - information about the file system (size, position of entries, ...)
- DMAP
    - location of empty data blocks
- File Allocation Table (FAT)
- Root Directory of file in the file system
    - file name
    - file size
    - user and group id
    - access rights (mode)
    - timestamps
        - last access (`atime`)
        - last change (`mtime`)
        - last status change (`ctime`)
    - pointer to first data block
- Data Blocks

### Operations
- directory
    - readdir(path)
        - get content
    - mkdir(path, mode)
        - create
    - rmdir(path)
        - delete
- file
    - mknod(path, mode, dev)
        - create
    - unlink(path)
        - delete
    - rename(old, new)
        - rename or move
    - open(path, flags)
        - open
    - read(path, buf, length, offset, fh)
        - read
    - write(path, buf, size, offset, fh)
        - write
    - truncate(path, len, fh)
        - cut to length
    - flush(path, fh)
        - write to disk
    - release(path, fh)
        - close finally
- metadata
    - getattr(path)
        - read metadata
    - chmod(path, mode)
        - change access rights
    - chown(path, uid, gid)
        - change owner
    - fsinit(self)
        - initialize

### Proper Error Codes
- functions called by fuse return on...
    - ...success: `value` >= 0
    - ...error: `-errorCode`
- codes defined in `errno.h`
    - ENOSYS
        - function not implemented
    - EROFS
        - read-only file system
    - EPERM
        - operation not allowed
    - EACCES
        - access denied
    - ENOENT
        - file or directory does not exist
    - EIO
        - I/O exception
    - EEXIST
        - file exists
    - ENOTDIR
        - file is not a directory
    - EISDIR
        - file is a directory
    - ENOTEMPTY
        - directory is not empty
