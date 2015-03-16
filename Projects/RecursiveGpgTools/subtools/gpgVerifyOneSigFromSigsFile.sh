#!/bin/bash
if [ "$#" -lt 3 ]; then
	echo "Usage: $0 relativeFilePathToVerify sigsFile [heightOfOneGpgSigInSigsFile=11]"
	echo "Ex: $0 path/to/file.txt ~/path/to/sigfile.txt"
	echo ""
	echo "Note: relativeFilePathToVerify must be provided both in the format used in the sigs file (no leading ./), and it must also exist at that path relative to your working directory. So you should cd into the root of the directory you recursively sign/verify and then run this command from there"
	exit 1
fi

heightOfOneGpgSigInSigsFile=11
if [ "$#" -ge 4 ]; then
	heightOfOneGpgSigInSigsFile="$3"
fi
grep "$1" -A "$heightOfOneGpgSigInSigsFile" "$sigsFile" | tail -n "$heightOfOneGpgSigInSigsFile" | gpg --verify - "$1" #TODOoptional: determine where the pgp sig ends dynamically without using the height like this
