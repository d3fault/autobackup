#!/bin/bash

#generation of test/dummy data for life shaper to practice on -- mimics current state of files
#TODOreq: tree stat generation and parsing/utilzing

#regular files fs -- see union heirchy notes for explaination
mkdir regularFiles
cd regularFiles

#regular archive is pretty easy
mkdir folder1
cd folder1
echo "file 1 content" > file1
cd ..
mkdir folder2
cd folder2
mkdir folder2a
cd folder2a
echo "file 2a content" > file2a
echo "file 2aa content" > file2aa
cd ..
cd ..

#regular git dir
mkdir folder3
cd folder3
echo "file 3 content first created" > file3
git init
git add .
git commit -m "folder 3 repo initial commit"
echo "file 3 content second revision" > file3
git add .
git commit -m "folder 3 first revision"
echo "file 3 content append woot" >> file3
git add .
git commit -m "folder 3 second revision"
cd ..

#censor of file revision, conflict at new file revision etc(?)
mkdir folder4
cd folder4
echo "my secret passcodes to censor: LOLOLOLOLOLOLOLOL" > file4
git init
git add .
git commit -m "initial commit of a file which was not censored when first committed"
echo "my secret passcodes to censor: ORLYORLYORLYORLYORLYORLYORLY" > file4
echo "some more data to take into account worst case scenarios or something" >> file4
git add .
git commit -m "first revision where something, still to be censored, was revised -- and other ok stuff is added too"
echo "here is just yet more data so we can practice handling the 'was censored and almost conflicted but did not' scenario" >> file4
git add .
git commit -m "second revision where data was appended and there is a conflict that should be able to be auto corrected"
cd ..

#folder exist, commit, folder delete, commit [or multiple], folder re-create --- DO NOT OVERWRITE OLD??, commit
mkdir folder5
cd folder5
echo "some bullshit file that isn't relevant but needs to still exist later heh" > file5
git init
git add .
git commit -m "initial commit where folder does not exist yet"
echo "some modification to that bullshit file because who knows what kind of stuff is also going on" >> file5
mkdir folder5a
cd folder5a
echo "the guts of file5a that will potentially be overwritten if i am not careful" > file5a
cd ..
git add .
git commit -m "commit where the folder that we need to specially account for is first created"
rm -rfv ./folder5a/
echo "yet even more bullshit blah blah blah" >> file5
git add .
git commit -m "commit where the folder is deleted"
echo "another file is created out of irrelevance" > file55
echo "another append for good measure" >> file5
git add .
git commit -m "a commit that has nothing to do with the target folder"
mkdir folder5a
cd folder5a
echo "this is the data that will overwrite the old data if i am not careful. we don't even necessarily need to have matching filenames for this overwrite to happen" > file5b
cd ..
git add .
git commit -m "target folder re-created and file without matching filename created within"
cd ..

#file exist, commit, move to subfolder (old or new -- and even accounting for re-create as above)!, commit
#new
mkdir folder6
cd folder6
echo "the contents of the file that will be moved to the subfolder later" > file6
echo "an irrelevant file that won't move but will change" > file66
git init
git add .
git commit -m "initial commit of file no folder"
mkdir folder6a
mv file6 folder6a
echo "an append to that other file"
cd folder6a
echo "hell this file will probably also change as/when it is being moved who knows" >>file6
cd ..
git add .
git commit -m "file has been both appended to and moved to a NEW subfolder, which i think is going to be submodulized"
#old
echo "the contents of a file that will be moved to an already existing subfolder later" > file666
mkdir folder6b
cd folder6b
echo "stuff in the folder justifying it's existence and that will irrelevantly change also" > file6b
cd ..
git add .
git commit -m "commit where the subfolder that we will move a file into is created -- so is the file but that is irrelevant"
mv file666 folder6b
cd folder6b
echo "appendz galore wewt" >> file6b
cd ..
git add .
git commit -m "commit where a file is moved to an already existing folder that is already or something submodulized"
cd folder6b
echo "a full modification to the file that was moved" > file666
echo "lol more appendz galore pewp" >> file6b
cd ..
git add .
git commit -m "commit with a modification to the file that was moved to the old folder"
cd ..

#union heirchy -- regular files on 1tb drive are one, files stored via backup scripts on 750x are another (both have certain folders that are git repos)
cd ..
mkdir oneOfMy750xDrives
cd oneOfMy750xDrives

#regular files within
mkdir folderA
cd folderA
echo "file a contents" > filea
echo "file aa contents" > fileaa
mkdir folderAA
cd folderAA
echo "file something in folderAA" > fileSomething
cd ..
cd ..

#a git repo within -- still needs all above considerations.. but we don't need to regen the test material so long as they are definitely used
mkdir folderB
cd folderB
echo "file b contents" > fileb
git init
git add .
git commit -m "initial commit of a git repo on 750x drive"
echo "file b revision 1" > fileb
git add .
git commit -m "first revision of git repo on 750x drive"
echo "append to file b" >> fileb
echo "new file irrelevant" > filebb
git add .
git commit -m "second revision of git repo on 750x drive with new file too"
cd ..

#union heichy -- go back to start
cd ..