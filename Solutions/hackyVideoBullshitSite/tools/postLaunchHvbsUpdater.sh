#!/bin/bash
exit 1

#this->launch(); //'this' == steven/d3fault, not "DasButton" app
#emit o("my decryption key is: blah blah blah, blah blah blah"); //phones too (mb a /b/ post, mb a freenet post, mb a facefuck post, mb vent session with some friends), don't trust 3 minute delay!
#plug in thumb drive with and cp -avi the autobackupBare from it (doesn't matter if encrypted... (WOOT))
#scp -r /home/d3faultOnline/autobackupBare/* user@d3fault.net:/home/user/autobackupBare/; #simple git-push from then on. could have ssh'd in and done a git clone, but then i'd have to type my pw for d3faultOnline into that server (git clone needs to be able to specify an ssh dest url imo)
#ssh user@d3fault.net, then tmux, then:

userHome=/home/user
autobackupBareUrl=file://$userHome/autobackupBare
autobackupRun=$userHome/autobackupRun

#copy pasted variables from preLaunch script
hvbs=$userHome/hvbs
hvbsWebSrc=$hvbs/webSrc
oldUnversionedArchiveWebSrcTemp=$hvbsWebSrc/oldUnversionedArchiveTemp
semiOldSemiUnversionedWebSrcTemp=$hvbsWebSrc/semiOldSemiUnversionedArchiveTemp
autobackupWebSrcTemp=$hvbsWebSrc/autobackupTemp

repoAtDir=$autobackupWebSrcTemp/repoAt`date +%s`
mkdir -p $repoAtDir/view
git archive --remote=$autobackupBareUrl master | tar -x -m -C $repoAtDir/view
cp -a $repoAtDir/view $repoAtDir/download

git clone $autobackupBareUrl $autobackupRun
d3faultPublicLicenseCopyrightHeaderBulkPrependerSrc=$autobackupRun/Solutions/d3faultPublicLicense/tools/D3faultPublicLicenseCopyrightHeaderBulkPrepender/src
cd "$d3faultPublicLicenseCopyrightHeaderBulkPrependerSrc"
qmake
make
copyrightHeaderToPrepend=$autobackupRun/Solutions/d3faultPublicLicense/header.prepend.dpl.v3.d3fault.launch.distribution.txt
./D3faultPublicLicenseCopyrightHeaderBulkPrependerCli $repoAtDir/download "$copyrightHeaderToPrepend"

#TODOreq: autobackup's downloads definitely needs "all rights reserved" prepended (perhaps to "view" as well)

#symlink swap the shits (why the fuck did i code AtomicThingo, guh bash is so much faster even though I hate it's softness/syntax/etc)
cd $autobackupWebSrcTemp
mkdir -p $autobackupWebSrcTemp/symlinkCreation
ln -s $repoAtDir symlinkCreation/autobackupCurrentSymlink
mv symlinkCreation/autobackupCurrentSymlink ./
cd -

#delete the old .lastModifiedTimestampsFile so hvbs sees the update
rm $autobackupWebSrcTemp/empty0/view/.lastModifiedTimestamps



#DasButton extracts old/semi-old and molests and makes new timestamp file
cd $autobackupRun/Solutions/DasButton/src
qmake
make
./DasButton




#the next two cp and mv src paths (old and semi-old) are hardcoded in DasButton
mkdir -p $oldUnversionedArchiveWebSrcTemp/perm
cp -a $hvbsWebSrc/oldUnversionedArchivePerm $oldUnversionedArchiveWebSrcTemp/perm/download
mv $hvbsWebSrc/oldUnversionedArchivePerm $oldUnversionedArchiveWebSrcTemp/perm/view

cd "$d3faultPublicLicenseCopyrightHeaderBulkPrependerSrc"
./D3faultPublicLicenseCopyrightHeaderBulkPrependerCli $oldUnversionedArchiveWebSrcTemp/perm/download "$copyrightHeaderToPrepend"

cd $oldUnversionedArchiveWebSrcTemp
mkdir -p $oldUnversionedArchiveWebSrcTemp/symlinkCreation
ln -s $oldUnversionedArchiveWebSrcTemp/perm symlinkCreation/oldUnversionedArchiveCurrentSymlink
mv symlinkCreation/oldUnversionedArchiveCurrentSymlink ./
cd -

rm $oldUnversionedArchiveWebSrcTemp/empty0/view/.lastModifiedTimestamps




mkdir -p $semiOldSemiUnversionedWebSrcTemp/perm
cp -a $hvbsWebSrc/semiOldSemiUnversionedArchivePerm $semiOldSemiUnversionedWebSrcTemp/perm/download
mv $hvbsWebSrc/semiOldSemiUnversionedArchivePerm $semiOldSemiUnversionedWebSrcTemp/perm/view

cd "$d3faultPublicLicenseCopyrightHeaderBulkPrependerSrc"
./D3faultPublicLicenseCopyrightHeaderBulkPrependerCli $semiOldSemiUnversionedWebSrcTemp/perm/download "$copyrightHeaderToPrepend"

cd $semiOldSemiUnversionedWebSrcTemp
mkdir -p $semiOldSemiUnversionedWebSrcTemp/symlinkCreation
ln -s $semiOldSemiUnversionedWebSrcTemp/perm symlinkCreation/semiOldSemiUnversionedArchiveCurrentSymlink
mv symlinkCreation/semiOldSemiUnversionedArchiveCurrentSymlink ./
cd -

rm $semiOldSemiUnversionedWebSrcTemp/empty0/view/.lastModifiedTimestamps



echo "post launch updater complete, check above for errors"
exit 0
