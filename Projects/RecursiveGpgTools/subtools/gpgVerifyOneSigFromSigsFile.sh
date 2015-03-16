#!/bin/bash
if [ "$#" -lt 2 ]; then
	echo "Usage: $0 relativeFilePathToVerify sigsFile"
	echo "Ex: $0 path/to/file.txt ~/path/to/sigfile.txt"
	echo ""
	echo "Note: relativeFilePathToVerify must be provided both in the format used in the sigs file (no leading ./), and it must also exist at that path relative to your working directory. So you should cd into the root of the directory you recursively sign/verify and then run this command from there"
	exit 1
fi

desiredGpgSig0=`grep "$1" -A 2048 "$2"` #2048 _lines_ of sig data max xD
numLinesUntilEndOfSig=`echo "$desiredGpgSig0" | grep -nm 1 "\-\-\-\-\-END PGP SIGNATURE\-\-\-\-\-" | cut -f1 -d:`
numLinesOfSig=$((numLinesUntilEndOfSig-1)) # minus one to account for file path line
echo "$desiredGpgSig0" | head -n "$numLinesUntilEndOfSig" | tail -n "$numLinesOfSig" | gpg --verify - "$1"
