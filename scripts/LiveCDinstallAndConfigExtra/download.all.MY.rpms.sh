#!/bin/sh
# assumes rpmfusion repo and adobe repo already installed. you can remove adobe repo probably, unless you want flash updates. actually yea i'm gonna fuck that shit. wait no the install --downloadonly won't work without it in the repo list. fml
yum install yum-downloadonly
yum install p7zip flash-plugin mozilla-adblockplus mozilla-noscript alsa-plugins-pulseaudio nspluginwrapper keepassx ntp vagalume gnome-games mplayer firefox gstreamer-plugins-ugly gstreamer-plugins-bad -y -v --downloadonly --downloaddir=./
