#!/bin/bash
exit 1

#########Example of sleepFor45minsThenEmailAdminThenPause.sh (skip this for general introduction to script)##

##Note: this example script requires these packages to be installed: apt-get install sendemail libio-socket-ssl-perl libnet-smtp-ssl-perl libnet-ssleay-perl
##^I'm actually not sure that ibnet-smtp-ssl-perl is required but w/e too lazy to confirm it isn't (and it makes sense that it would be required)

##!/bin/bash
#sleep 2700
#sendemail -f root-server-upgrader@example.com -t your-email-address@example.com -u "Server upgrade pending" -m "apt-get upgrade didn't finish in less than 45 minutes, so this most likely means that the upgrade process is waiting for a response to a question it asked you. ssh in as roo and tmux attach to be able to answer the question(s)" -s smtp.example.com:587 -o tls=yes -xu your-email-address@example.com -xp YourPasswordGoesHere
#pause
#

#########End Example of sleepFor45minsThenEmailAdminThenPause.sh


## General Introduction ##

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
    echo "Starting background process that sleeps for 45 minutes then sends you an email"
    if [[ -x "./sleepFor45minsThenEmailAdminThenPause.sh" ]]
    then
        ./sleepFor45minsThenEmailAdminThenPause.sh > /dev/null 2>&1 & # the "email admin" part will be server-specific. check the top of this file for an example script
    else
        echo "error, helper script not found or is not executable: ./sleepFor45minsThenEmailAdminThenPause.sh (you must create that script. check this script for an example of it). make sure you also run: chmod u+x sleepFor45minsThenEmailAdminThenPause.sh"
        exit 1
    fi

    pidoflastcommand="${!}" #store pid of process launched in background so we can kill it later before it sends that email out

    apt-get update
    apt-get upgrade -y

    #if there are testing/sid packages installed, to "upgrade" those you'd need to do an "apt-get install -t testing-or-sid package1 package2 etc" here

    echo "Killing  background process that sleeps for 45 minutes then sends you an email (so it won't send you that email)"
    kill "${pidoflastcommand}"
    echo "Sleeping for a day"
    sleep 86400 #24 hours
    echo "Waking up. Another day, another dollar."
done
