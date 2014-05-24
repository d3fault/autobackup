#!/bin/bash
aptSources=/etc/apt/sources.list
upgradeScript=/root/aptUpgrade.sh
echo "deb http://http.debian.net/debian/ wheezy main" > $aptSources || exit 1
echo "deb http://http.debian.net/debian/ wheezy-updates main" >> $aptSources
echo "deb http://http.debian.net/debian/ wheezy-backports main" >> $aptSources
echo "deb http://security.debian.org/ wheezy/updates main" >> $aptSources
cp -a $aptSources $aptSources.wheezy
echo "#!/bin/bash" > $upgradeScript
echo "cp -a $aptSources.wheezy $aptSources" >> $upgradeScript
echo "apt-get update" >> $upgradeScript
echo "apt-get upgrade -y" >> $upgradeScript
chmod +x $upgradeScript
$upgradeScript
apt-get install -y build-essential cmake pkg-config checkinstall automake autoconf libtool git curl p7zip-full zip unzip libqt4-dev libssl-dev zlib1g-dev
apt-get install -y -t wheezy-backports tmux
echo "deb http://http.debian.net/debian/ sid main" > $aptSources.sid
echo "deb http://security.debian.org/ wheezy/updates main" >> $aptSources.sid
echo "cp -a $aptSources.sid $aptSources" >> $upgradeScript
echo "apt-get update" >> $upgradeScript
echo "apt-get install -y -t sid libboost-all-dev" >> $upgradeScript
echo "cp -a $aptSources.wheezy $aptSources" >> $upgradeScript
echo "apt-get update" >> $upgradeScript
$upgradeScript
exit 0
