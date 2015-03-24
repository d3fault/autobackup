#!/bin/sh
freeBytesBefore=`df | mawk '{ if($1=="/dev/sda1") print($4)  }'`
echo "Auto-Removing...";
apt-get autoremove -y;
echo "Done Auto-Removing";
echo "Purging...";
apt-get purge;
echo "Done Purging";
echo "Auto-Cleaning...";
apt-get autoclean;
echo "Done Auto-Cleaning";
echo "Cleaning All";
apt-get clean all;
echo "Done Cleaning All";
echo "\n";
freeBytesAfter=`df | mawk '{ if($1=="/dev/sda1") print($4)  }'`
mbFreed=`echo "($freeBytesAfter-$freeBytesBefore)/1024" | bc`
echo "Freed $mbFreed MB from clean";
