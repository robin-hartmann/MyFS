#!/bin/bash
scriptDir=$(dirname "$0")

binDirRel="$scriptDir/../../cmake-build-debug/bin"
binDirAbs=$(cd "$binDirRel"; pwd)

mkfs="$binDirAbs/mkfs.myfs"
mount="$binDirAbs/mount.myfs"

pushd "$scriptDir/../.." > /dev/null
MYFS_BIN_MKFS="$mkfs" MYFS_BIN_MOUNT="$mount" npx myfs-test
popd > /dev/null
