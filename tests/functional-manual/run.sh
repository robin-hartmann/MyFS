#!/bin/bash
scriptDir=$(dirname "$0")

binDir="$scriptDir/../../cmake-build-debug/bin/"
mkfs="$binDir/mkfs.myfs"
mount="$binDir/mount.myfs"

helloWorld="$scriptDir/hello-world.txt"

echo -e "\n*** cleanup ***\n"
rm -rf tmp/
mkdir tmp/ tmp/mnt/

echo -e "\n*** mkfs ***\n"
$mkfs tmp/container $helloWorld

echo -e "\n*** mount ***\n"
$mount tmp/container tmp/log tmp/mnt/

echo -e "\n*** read pre-initialized file ***\n"
cat tmp/mnt/hello-world.txt

echo -e "\n*** create file manually ***\n"
echo Hello World! > tmp/mnt/hello-world.txt

echo -e "\n*** read manually created file ***\n"
cat tmp/mnt/hello-world.txt

echo -e "\n*** unmount ***\n"
sudo umount tmp/mnt/
