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
$mount $scriptDir/tmp/container $scriptDir/tmp/log.txt $scriptDir/tmp/mnt/ -s

printf "\n*** read block size ***\n"
stat -f %k $scriptDir/tmp/mnt/

printf "\n*** read pre-initialized file ***\n"
cat $scriptDir/tmp/mnt/hello-world.txt

printf "\n*** create file manually ***\n"
echo "Hello Manual World!" > $scriptDir/tmp/mnt/hello-manual-world.txt

printf "\n*** read manually created file ***\n"
cat $scriptDir/tmp/mnt/hello-manual-world.txt


printf "\n*** initial append ***\n"
echo "initial append" >> "$scriptDir/tmp/mnt/append.txt"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 2nd append ***\n"
echo "2nd append" >> "$scriptDir/tmp/mnt/append.txt"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 3rd append ***\n"
echo "3rd append" >> "$scriptDir/tmp/mnt/append.txt"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 4th append ***\n"
echo "4th append" >> "$scriptDir/tmp/mnt/append.txt"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** initial overwrite append: same length ***\n"
echo "same length...................................." > "$scriptDir/tmp/mnt/append.txt"
cat "$scriptDir/tmp/mnt/append.txt"

printf "\n*** 2nd overwrite append: longer length ***\n"
echo "longer length..................................." > "$scriptDir/tmp/mnt/append.txt"
cat "$scriptDir/tmp/mnt/append.txt"


printf "\n*** initial overwrite ***\n"
echo "initial overwrite" > "$scriptDir/tmp/mnt/overwrite.txt"
cat "$scriptDir/tmp/mnt/overwrite.txt"

printf "\n*** 2nd overwrite: same length ***\n"
echo "same length......" > "$scriptDir/tmp/mnt/overwrite.txt"
cat "$scriptDir/tmp/mnt/overwrite.txt"

printf "\n*** 3rd overwrite: longer length ***\n"
echo "longer length....." > "$scriptDir/tmp/mnt/overwrite.txt"
cat "$scriptDir/tmp/mnt/overwrite.txt"

printf "\n*** 4th overwrite: shorter length ***\n"
echo "shorter length" > "$scriptDir/tmp/mnt/overwrite.txt"
cat "$scriptDir/tmp/mnt/overwrite.txt"


printf "\n*** unmount ***\n"
sudo umount $scriptDir/tmp/mnt/
