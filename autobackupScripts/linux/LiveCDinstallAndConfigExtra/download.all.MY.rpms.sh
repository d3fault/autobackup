#!/bin/sh
# assumes rpmfusion already installed
yum install yum-downloadonly
yum install keepassx ntp vagalume gnome-games mplayer firefox gstreamer-plugins-ugly gstreamer-plugins-bad -yv --downloadonly --downloaddir=./
