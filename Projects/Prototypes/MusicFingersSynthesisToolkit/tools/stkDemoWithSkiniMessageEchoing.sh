#!/bin/bash
#I did this on my online box while fucking around with stk to see if it met my needs, but eh I guess it's worth saving. It requires the debian package 'stk'. It's really just a minor modification of the STKDemo script
cd /usr/share/stk/rawwaves/butt || exit 1 #must be mkdir'd. the stk demo uses hardcoded paths of ../../rawwaves
echo "ctrl+c ends"
wish < /usr/share/stk/tcl/Demo.tcl | tee >(stk-demo Clarinet -or -ip)
cd -
