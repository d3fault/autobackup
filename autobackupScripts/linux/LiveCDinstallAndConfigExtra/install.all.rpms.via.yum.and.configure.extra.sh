#!/bin/sh
yum localinstall `ls *.rpm` -yv --disablerepo=*
mkdir /home/liveuser/.vagalume/
cp ./vagalume-config.xml /home/liveuser/.vagalume/config.xml
chmod 777 /home/liveuser/.vagalume/config.xml
