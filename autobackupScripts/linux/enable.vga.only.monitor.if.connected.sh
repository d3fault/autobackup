#!/bin/bash
xrandrquery=`xrandr --query`;
vgaconnectedstring='VGA1 connected';
if [[ "$xrandrquery" == *"$vgaconnectedstring"* ]]; then
xrandr --output LVDS1 --off --output VGA1 --primary --auto;
pcmanfm --wallpaper-mode=stretch;
pcmanfm --wallpaper-mode=center;
fi
