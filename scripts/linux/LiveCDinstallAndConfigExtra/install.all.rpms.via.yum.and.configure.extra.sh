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
echo "getting the date"
ntpdate 0.fedora.pool.ntp.org
echo "got the date"

# USER MODE SETTINGS. PERFORM ALL ROOT OPERATIONS BEFORE THIS LINE nvm doesn't work
# su liveuser

# desktop, a preinstalled image specific to fedora 17 lxde
echo "changing background"
su liveuser --command="pcmanfm --set-wallpaper=/usr/share/backgrounds/images/ladybugs.jpg"
echo "background changed"

# firefox, create new profile, append custom config -- assumes noscript + adblock installed
echo "Creating and customizing a Firefox profile"
su liveuser --command="/usr/bin/firefox -CreateProfile default"
su liveuser --command="cat ./firefox.prefs.additions.txt >> /home/liveuser/.mozilla/firefox/*.default/prefs.js"
echo "Done Creating and customizing a Firefox profile"

# keepassx
echo "Configuring keepassx"
su liveuser --command="mkdir -p /home/liveuser/.config/keepassx/"
su liveuser --command="cp ./keepassx.config.ini /home/liveuser/.config/keepassx/config.ini"
echo "Done configuring keepassx"
echo "-"
echo "install/configure script done"
