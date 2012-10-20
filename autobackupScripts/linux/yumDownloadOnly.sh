#!/bin/bash
epochTime=`date +%s`
newDir=/home/d3fault/updates/updateAt`echo $epochTime`/
mkdir `echo $newDir`
yum install `echo $*` -y --downloadonly --downloaddir=`echo $newDir`
cp /home/d3fault/updates/installAllRpmsInThisDir.sh "$newDir"

