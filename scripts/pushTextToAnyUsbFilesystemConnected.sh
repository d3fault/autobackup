#!/bin/bash
exit 1

#I think this would work, but idk kinda scared to use it as a script. I'm 99% sure git won't push if the arg isn't a repository (or the right repository (or the right repository at the right state (doesn't overwrite without --force))). the thing of note is the asterisk in the push command
cd ~/text

#git push --all /media/user/*/textBare && echo "done pushing to usb filesystems"
#^actually I think that's unsafe, a 3rd party thumb drive could provide untrusted code that could be somehow accidentally interpreted as bash code. I might be wrong, but I don't know that I'm not right (or something). Maybe globbing first and then using a for loop is best? it'd defintely give me better feedback "done pushing to /path/for/example/in/$i", but that's besides the point.