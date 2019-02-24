#!/bin/bash
scriptDir=$(dirname "$0")

binDir="$scriptDir/../../cmake-build-debug/bin"
mkfs="$binDir/mkfs.myfs"
mount="$binDir/mount.myfs"

helloWorld="$scriptDir/hello-world.txt"

printf "\n*** cleanup ***\n"
rm -rf $scriptDir/tmp/
mkdir $scriptDir/tmp/ $scriptDir/tmp/mnt/

printf "\n*** mkfs ***\n"
$mkfs $scriptDir/tmp/container $helloWorld

printf "\n*** mount ***\n"
$mount $scriptDir/tmp/container $scriptDir/tmp/log $scriptDir/tmp/mnt/

printf "\n*** read pre-initialized file ***\n"
cat $scriptDir/tmp/mnt/hello-world.txt

printf "\n*** create file manually ***\n"
printf Hello World! > $scriptDir/tmp/mnt/hello-world-manual.txt

printf "\n*** read manually created file ***\n"
cat $scriptDir/tmp/mnt/hello-world-manual.txt

printf "\n*** unmount ***\n"
sudo umount $scriptDir/tmp/mnt/
