##asumes qt/git/7za/tmux/boost-sid/wt-compiled already installed on remote server##

#TODOreq:
#-video import folder structures
#-no.ad.placeholder.jpg and my.sexy.face.logo.jpg


#this->launch(); //'this' == steven/d3fault, not "DasButton" app (TODOreq: hvbs needs to already be running rofl)
#emit o("my decryption key is: blah blah blah, blah blah blah"); //phones too (mb a /b/ post, mb a freenet post, mb a facebook post, mb vent session with some friends), don't trust 3 minute delay!
#plug in thumb drive with and cp -avi the autobackupBare from it (doesn't matter if encrypted... (WOOT))
scp -r /home/d3faultOnline/autobackupBare/* user@d3fault.net:/home/user/autobackupBare/; #simple git-push from then on. could have ssh'd in and done a git clone, but then i'd have to type my pw for d3faultOnlineBox into that server (git clone needs to be able to specify an ssh dest url imo)
#ssh user@d3fault.net, then (TODOoptional: all this in tmux in case dc):
mkdir -p /home/user/hvbsWebSrc/autobackupTemp/
mkdir -p /home/user/hvbsWebSrc/oldUnversionedArchivePerma/
mkdir -p /home/user/hvbsWebSrc/semiOldSemiUnversionedPerma/
mkdir -p /home/user/hvbsWebSrc/tempEmptyPlaceholderDuringDasButtonExecution/
echo "this folder will contain a lot more files in just a few minutes, please check back later" > /home/user/hvbsWebSrc/tempEmptyPlaceholderDuringDasButtonExecution/readme.txt
echo readme.txt:`date +%s` > /home/user/hvbsWebSrc/tempEmptyPlaceholderDuringDasButtonExecution/.lastModifiedTimestamps
ln -s /home/user/hvbsWebSrc/tempEmptyPlaceholderDuringDasButtonExecution /home/user/hvbsWeb/oldUnversionedArchive
ln -s /home/user/hvbsWebSrc/tempEmptyPlaceholderDuringDasButtonExecution /home/user/hvbsWeb/semiOldSemiUnversionedArchive
git clone file:///home/user/autobackupBare /home/user/autobackupRun
cd "/home/user/autobackupRun/Solutions/hackyVideoBullshitSite/tools/AtomicallyUpdateSymlinkToLatestGitCommit/src/cli/"
qmake
make
./AtomicallyUpdateSymlinkToLatestGitCommitCli "file:///home/user/autobackupBare" "/home/user/hvbsWeb/autobackupLatest" "/home/user/hvbsWebSrc/autobackupTemp/"
#TODOreq: text DOWNLOAD dir (copyright prepended)
#TODOreq: configure hvbs before launch
cd "/home/user/autobackupRun/Solutions/hackyVideoBullshitSite/src/"
qmake
make
ln -s /usr/local/share/Wt/resources/ resources
tmux -d -s hvbs './HackyVideoBullshitSite --docroot ".;/resources" --http-address 0.0.0.0 --http-port 7777'
cd "/home/user/autobackupRun/Solutions/DasButton/src/"
qmake
make
./DasButton
