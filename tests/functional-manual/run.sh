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


printf "\n*** initial append ***\n"
sudo bash -c "echo 'initial append' >> '$scriptDir/tmp/mnt/append.txt'"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 2nd append ***\n"
sudo bash -c "echo '2nd append' >> '$scriptDir/tmp/mnt/append.txt'"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 3rd append ***\n"
sudo bash -c "echo '3rd append' >> '$scriptDir/tmp/mnt/append.txt'"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 4th append ***\n"
sudo bash -c "echo '4th append' >> '$scriptDir/tmp/mnt/append.txt'"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** initial overwrite append: same length ***\n"
sudo bash -c "echo 'same length....................................' > '$scriptDir/tmp/mnt/append.txt'"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 2nd overwrite append: longer length ***\n"
sudo bash -c "echo 'longer length...................................' > '$scriptDir/tmp/mnt/append.txt'"
cat "$scriptDir/tmp/mnt/append.txt"


printf "\n*** initial overwrite ***\n"
sudo bash -c "echo 'initial overwrite' > '$scriptDir/tmp/mnt/overwrite.txt'"
cat "$scriptDir/tmp/mnt/overwrite.txt"

printf "\n*** 2nd overwrite: same length ***\n"
sudo bash -c "echo 'same length......' > '$scriptDir/tmp/mnt/overwrite.txt'"
cat "$scriptDir/tmp/mnt/overwrite.txt"

printf "\n*** 3rd overwrite: longer length ***\n"
sudo bash -c "echo 'longer length.....' > '$scriptDir/tmp/mnt/overwrite.txt'"
cat "$scriptDir/tmp/mnt/overwrite.txt"

printf "\n*** 4th overwrite: shorter length ***\n"
sudo bash -c "echo 'shorter length' > '$scriptDir/tmp/mnt/overwrite.txt'"
cat "$scriptDir/tmp/mnt/overwrite.txt"


printf "\n*** unmount ***\n"
sudo umount $scriptDir/tmp/mnt/
