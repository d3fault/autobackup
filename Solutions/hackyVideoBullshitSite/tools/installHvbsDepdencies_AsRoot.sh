#!/bin/bash
aptSources=/etc/apt/sources.list
echo "deb http://http.debian.net/debian/ wheezy main" > $aptSources || exit 1
echo "deb http://http.debian.net/debian/ wheezy-updates main" >> $aptSources
echo "deb http://http.debian.net/debian/ wheezy-backports main >> $aptSources
echo "deb http://security.debian.org/ wheezy/updates main" >> $aptSources
apt-get update
apt-get upgrade -y
apt-get install -y build-essential cmake pkg-config checkinstall automake autoconf libtool git curl p7zip-full zip unzip libqt4-dev libssl-dev zlib1g-dev
apt-get install -y -t wheezy-backports tmux
mv $aptSources $aptSources.bak
echo "deb http://http.debian.net/debian/ sid main" > $aptSources
echo "deb http://security.debian.org/ wheezy/updates main" >> $aptSources
apt-get update
apt-get -y -t sid libboost-all-dev
mv $aptSources.bak $aptSources
apt-get update
exit 0
