# Specification

## Part 1: Read-Only File System

### build `mkfs.myfs`

Creates and initializes the block device.

- is called like this:<br>
  `mkfs.myfs <container-file> [file1, file2, ...]`
    - selected files are added to the root directory no matter from which directory they originate
        - amount of files is limited to `NUM_DIR_ENTRIES`
        - throws `EEXIST` when adding a file that already exists
    - most attributes (name, size, `atime`, `mtime`, `ctime`) are copied from the original files
        - length of name is limited to `NAME_LENGTH`
    - user and group id are set depending on the current user
    - access rights (mode) of files are set to `S_IFREG | 0644`

### build `mount.myfs`

Mounts the block device.

- is called like this:<br>
  `mount.myfs <container-file> <log-file> <mount-dir>`
- the root directory returns the following values for...
    - ...its own access rights (mode): `S_IFDIR | 0755`
    - ...its own nlink: `2`
    - ...a file's nlink: `1`
- amount of opened files is limited to `NUM_OPEN_FILES`
- throws `EACCES` when trying to add, edit or delete files or adding a directory
- the following operations need to be implemented (minimum):
    - container
        - MyFS::fuseInit()
        - MyFS::fuseRelease()
    - directory
        - MyFS::fuseOpendir()
            - can be opened together with the container (fuseInit())
        - MyFS::fuseReaddir()
            - throws `ENOTDIR` when called for a directory other than root
        - MyFS::fuseReleasedir()
            - can be released with the container (fuseRelease())
        - MyFS::fuseGettatr()
            - throws `ENOENT` when called for a an entry that doesn't exist
    - file
        - MyFS::fuseOpen()
        - MyFS::fuseRead()
            - needs to be buffered
        - MyFS::fuseRelease()
        - relevant errors for files
            - throws `ENFILE` when too many files are open
            - throws `ENOENT` when a file wasn't found
            - throws `EBADF` when `fileInfo->fh` doesn't point to an open file
            - throws `ENXIO` when trying to access an index outside the file's boundaries
- for each opened file the last read block should be buffered

## Part 2: Read-Write File System

### extend `mkfs.myfs`

- creates an empty device of fixed size (at least 30 MB of space)
    - optionally, selected files can still be copied

### extend `mount.myfs`

- files on the block device can be read, edited (overwritten, appended), deleted and new files can be added
- throws `EACCES` when trying to add a directory
- the following operations need to be implemented additionally (minimum):
    - create new file
        - MyFS::fuseMknod()
            - is only called if MyFS::fuseGetattr() has returned `ENOENT` before
            - throws `EEXIST` if file already exists
            - throws `ENOSPC` if there is not enough space
        - MyFS::fuseCreate()
            - only if needed
    - (over)write file
        - MyFS::write()
            - the file has to be opened beforehand, so `fileInfo` should be used
            - needs to be buffered
            - throws `EBADF` when `fileInfo->fh` doesn't point to an open file
            - throws `ENOSPC` if there is not enough space
    - delete file
        - MyFS::unlink()
            - throws `ENOENT` if file doesn't exist
