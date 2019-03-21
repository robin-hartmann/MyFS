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
sudo $mount $scriptDir/tmp/container $scriptDir/tmp/log.txt $scriptDir/tmp/mnt/ -s -o allow_other
# alternatively: run without -o allow_other:
# sudo $mount $scriptDir/tmp/container $scriptDir/tmp/log.txt $scriptDir/tmp/mnt/ -s
# but then all read access to files needs to be done with sudo, as well

printf "\n*** read pre-initialized file ***\n"
cat $scriptDir/tmp/mnt/hello-world.txt

printf "\n*** create file manually ***\n"
sudo bash -c "echo 'Hello Manual World!' > $scriptDir/tmp/mnt/hello-manual-world.txt"

printf "\n*** read manually created file ***\n"
cat $scriptDir/tmp/mnt/hello-manual-world.txt

printf "\n*** unmount ***\n"
sudo umount $scriptDir/tmp/mnt/
