#!/bin/bash
TEH_DIR=/media/usb0/debFiles
if [ -d "$TEH_DIR" ]; then
	apt-get upgrade -y --print-uris | awk '{ if(length($1) > 7){ if(substr($1,1,8) == "'"'"'http://"){ if(substr($1,(length($1)-3),4) == "deb'"'"'"){ print(substr($1,2,(length($1)-2))); } } } }' > /media/usb0/debFiles/deb.files.upgrade.urls.txt
else
	echo "The directory: $TEH_DIR , does not exist"
fi
