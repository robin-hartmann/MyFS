#!/bin/bash
scriptDir=$(dirname "$0")

binDir="$scriptDir/../../cmake-build-debug/bin/"
mkfs="$binDir/mkfs.myfs"
mount="$binDir/mount.myfs"

helloWorld="$scriptDir/hello-world.txt"

printf "\n*** cleanup ***\n"
rm -rf tmp/
mkdir tmp/ tmp/mnt/

printf "\n*** mkfs ***\n"
$mkfs tmp/container $helloWorld

printf "\n*** mount ***\n"
$mount tmp/container tmp/log tmp/mnt/

printf "\n*** read pre-initialized file ***\n"
cat tmp/mnt/hello-world.txt

printf "\n*** create file manually ***\n"
printf Hello World! > tmp/mnt/hello-world.txt

printf "\n*** read manually created file ***\n"
cat tmp/mnt/hello-world.txt

printf "\n*** unmount ***\n"
sudo umount tmp/mnt/
