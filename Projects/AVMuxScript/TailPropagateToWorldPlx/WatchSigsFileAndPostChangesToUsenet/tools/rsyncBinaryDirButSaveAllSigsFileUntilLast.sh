#!/bin/bash
if [ ! -f ./allSigs.txt ]; then
	echo "no allSigs.txt in cwd"
	exit 1
fi
epochTime=`date +%s`
rsync -avhh --progress --protect-args --exclude '/allSigs.txt' --backup --suffix=~accidentallyOverwrittenDuringSyncAt-$epochTime ./ user@d3fault.net:/home/user/binary/
rsync -avhh --progress --protect-args ./allSigs.txt user@d3fault.net:/home/user/binary/
echo "done"
