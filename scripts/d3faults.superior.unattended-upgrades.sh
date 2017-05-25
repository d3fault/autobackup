#!/bin/bash
exit 1

#For doing "unattended upgrades" on a remote server, the following strategy is used (note: I am aware debian has an unattended-upgrades package, but I don't trust it -- wtf does it do when an upgrade asks me a question? how could that package possibly know my answer?)

#0) A tmux session is launched as root on the remote server
#1) A while true loop in bash, sleeping ~24 hours at the end of each iteration
#2) We launch a background process that performs the following:
#	2a) Sleep for ~45 minutes (or whatever is determined to be a good "max upgrade duration")
#	2b) Send an email to us saying that an upgrade is awaiting response to a question, at which point we can re-attach to the tmux session and answer the question
#	2c) pause (see the comment in (4) for an explaination of why)
#3) apt-get update && apt-get upgrade -y
#4) Kill the process launched in (2) so that the email in (2b) never gets sent (note: the process may have already exitted, so the script needs to make sure not to crash in that case -- if we store a PID, can't that PID be used by a different process at a later date? TO DOnereq wtfz -- one solution is to add a `pause` after (2b) so that the process DOESN'T exit and we know the PID won't be re-used by some arbitrary 3rd party process)

#alternatively, you can omit the "-y" flag from the apt-get upgrade in (3) so that you are asked to confirm EVERY upgrade, not just ones that ask questions... and you still won't be bothered when there's nothing to upgrade

#TODOmb: instead of running once every 24 hours, run on an event: whenever a DSA is posted. all I'd have to do is subscribe to the mailing list :). I'm not sure if backports has it's own DSA mailing list, it might (and don't backports get upgraded differently anyways (by specifying install again? I forget)) -- doing this would [probably] be less bandwidth wasteful (might be more wasteful xD) and would [definitely] pull in security patches MUCH faster
#TODOoptimization: optimization of above comment^ ... I could parse the relevant packages from the DSA and then determine (systematically) if it even affects me (or my dependencies). I'm not sure exactly how to do that (apt and/or dpkg magic), but I'm 99% sure it's possible. This would cause me to only ever use bandwidth when I need it (aside from polling an email server a bunch LoLoL (or being an email server (fuck that..... but maybe)))

while :
do
    #TODOreq: get pid of process launched in background. for now fuck it, killall works
    ./sleepFor45minsThenEmailAdminThenPause.sh > /dev/null 2>&1 & # no point in putting the contents of that simple script here, since the "email admin" part will be server-specific (and I also don't know a/the command to do it yet ("sendemail" sounds promising xD)). but the first line will be "sleep 2700" (45 mins)

    apt-get update
    apt-get upgrade -y

    #if there are testing/sid packages installed, to "upgrade" those you'd need to do an "apt-get install -t testing-or-sid package1 package2 etc" here

    killall sleepFor45minsThenEmailAdminThenPause.sh
    echo "Sleeping for a day"
    sleep 86400 #24 hours
done
