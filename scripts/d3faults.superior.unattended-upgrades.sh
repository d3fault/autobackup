#!/bin/bash
exit 1

#TODOreq: implement it
#For doing "unattended upgrades" on a remote server, the following strategy can be used (note: I am aware debian has an unattended-upgrades package, but I don't trust it -- wtf does it do when an upgrade asks me a question? how could that package possibly know my answer?)

#0) A tmux session is launched as root on the remote server
#1) A while true loop in bash, sleeping ~24 hours at the end of each iteration
#2) We launch a background process that performs the following:
#	2a) Sleep for ~45 minutes (or whatever is determined to be a good "max upgrade duration")
#	2b) Send an email to us saying that an upgrade is awaiting response to a question, at which point we can re-attach to the tmux session and answer the question
#	2c) pause (see the comment in (4) for an explaination of why)
#3) apt-get update && apt-get upgrade -y
#4) Kill the process launched in (2) so that the email in (2b) never gets sent (note: the process may have already exitted, so the script needs to make sure not to crash in that case -- if we store a PID, can't that PID be used by a different process at a later date? TO DOnereq wtfz -- one solution is to add a `pause` after (2b) so that the process DOESN'T exit and we know the PID won't be re-used by some arbitrary 3rd party process)

#alternatively, you can omit the "-y" flag from the apt-get upgrade in (3) so that you are asked to confirm EVERY upgrade, not just ones that ask questions... and you still won't be bothered when there's nothing to upgrade
