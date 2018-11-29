# Tasks

### Part 1: Read-Only File System
- block device can be mounted and is automatically initialized by calling `mkfs.myfs <container-file> [file1, file2, ...]`
    - selected files are copied onto the block device
- files on the block device can be read, but not edited or deleted

#### Part 1a: definition of the file system's structure and of the software architecture

#### Part 1b: creation and initialization of the block device using `mkfs.myfs <container-file> [file1, file2, ...]`
- selected files are added to the root directory no matter from which directory they originate
    - multiple files with the same name should cause an error
    - most attributes (name, size, `atime`, `mtime`, `ctime`) can be copied from the original files
        - to read the attributes of the original files use `stat()`
    - user and group id should be set depending on the current user
    - access rights (mode) of files should be set to `S_IFREG | 0444`
- fuse operations are not needed necessarily
    - `BlockDevice` can be used directly
    - but operations could be implemented and re-used by the next step

#### Part 1c: mouting the block device using `mount.myfs <container-file> <log-file> <mount-dir>`
- can be dismounted using `fusermount --unmount`
- the root directory should return the following values for...
    - ...its own access rights (mode): `S_IFDIR | 0555`
    - ...its own nlink: `2`
    - ...a file's nlink: `1`
- the following operations need to be implemented (minimum):
    - container
        - MyFS::fuseInit()
        - MyFS::fuseRelease()
    - directory
        - MyFS::fuseOpendir()
            - can be opened together with the container (fuseInit())
        - MyFS::fuseReaddir()
        - MyFS::fuseReleasedir()
            - can be released with the container (fuseRelease())
        - MyFS::fuseGettatr()
    - file
        - MyFS::fuseOpen()
        - MyFS::fuseRead()
        - MyFS::fuseRelease()
- for each opened file the last read block should be buffered

#### Part 1d: extensive testing
- calling `mount.myfs -s` causes fuse to run in single mode for easier debugging

### Part 2: Read-Write File System
- block device can be mounted and is automatically initialized by calling `mkfs.myfs <container-file> [file1, file2, ...]`
    - creates an empty devices of fixed size
    - optionally, selected files can still be copied
- files on the block device can be read, edited, deleted and new files can be added

### Part 3: Documentation

### Deadline for Submission: 30.01.19

### Additional Information
- use libfuse
    - [documentation](http://libfuse.github.io/doxygen/)
- only one directory
- the file system should have at least 30 MB of space

#### Structure of MyFS container files (minimum)
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

#### Operations
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

#### Proper Error Codes
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
