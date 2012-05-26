#!/bin/sh
yum localinstall `ls *.rpm` -yv --disablerepo=*

# mount 5gb ram as temp filesystem
# mkdir because it's a read-only live usb drive so it does not persist reboots
mkdir /ram
mount -t tmpfs -o size=5g tmpfs /ram

# vagalume last.fm username/password
mkdir /home/liveuser/.vagalume/
cp ./vagalume-config.xml /home/liveuser/.vagalume/config.xml
chmod 777 /home/liveuser/.vagalume/config.xml

# date, time, and timezone
cp /usr/share/zoneinfo/America/Phoenix /etc/localtime
ntpdate 0.fedora.pool.ntp.org

# USER MODE SETTINGS. PERFORM ALL ROOT OPERATIONS BEFORE THIS LINE nvm doesn't work
# su liveuser

# desktop, a preinstalled image specific to fedora 17 lxde
su liveuser --command="pcmanfm --set-wallpaper=/usr/share/backgrounds/images/ladybugs.jpg"

# firefox, create new profile, append custom config -- assumes noscript + adblock installed
su liveuser --command="/usr/bin/firefox -CreateProfile default"
su liveuser --command="cat ./firefox.prefs.additions.txt >> /home/liveuser/.mozilla/firefox/*.default/prefs.js"


