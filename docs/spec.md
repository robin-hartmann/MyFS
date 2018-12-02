# Specification

## Part 1: Read-Only File System

### build `mkfs.myfs`

Creates and initializes the block device.

- is called like this:<br>
  `mkfs.myfs <container-file> [file1, file2, ...]`
    - selected files are added to the root directory no matter from which directory they originate
        - amount of files is limited to `NUM_DIR_ENTRIES`
        - adding a file that already exists causes an error
            - `EEXIST`
    - most attributes (name, size, `atime`, `mtime`, `ctime`) are copied from the original files
        - length of name is limited to `NAME_LENGTH`
    - user and group id are set depending on the current user
    - access rights (mode) of files are set to `S_IFREG | 0444`

### build `mount.myfs`

Mounts the block device.

- is called like this:<br>
  `mount.myfs <container-file> <log-file> <mount-dir>`
- the root directory returns the following values for...
    - ...its own access rights (mode): `S_IFDIR | 0555`
    - ...its own nlink: `2`
    - ...a file's nlink: `1`
- amount of opened files is limited to `NUM_OPEN_FILES`
- trying to add, edit or delete files or adding a directory causes an error
    - `EROFS`
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

## Part 2: Read-Write File System

### extend `mkfs.myfs`

- creates an empty device of fixed size (at least 30 MB of space)
    - optionally, selected files can still be copied

### extend `mount.myfs`

- files on the block device can be read, edited, deleted and new files can be added
- adding a directory causes an error
    - `ENOSYS`
