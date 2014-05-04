#include <QCoreApplication>

#include "ffmpegsegmentuploadercli.h"

//TODOoptional: on first connect, remove any partially written files. which leads me to TODOoptimization: hack the sftp source code to detect such cases and to then do an sha1sum of the partially written fail vs the same range in the file to be uploaded. if they match, just continue uploading (you were going to overwrite it anyways) from that left off point. hell it might already do that, but i don't know and i doubt it.
//^^i'm not going to do that optional remove because it's implied when i do the overwrite and just adds unneeded complexity (had:logics)

//TO DOneoptional: push sftp queue into it's own lib (and a cli would be easy (but why? sftp itself at that point lawl)). i'm considering re-using the code for "server<-->server" propagation (after initial upload that this very app takes care of), BUT i'm also researching rasterbar libtorrent. i'm quite certain libtorrent will do, but uncertain just how easy it will be to use. set_alert_dispatch looks promising (for getting when my shit is complete asynchronously)... but i'm concerned with the dht'ness of it all (i guess i could set up a tracker.. but central point of failure is central). there's also the consideration that my 3 minute segments are so fucking small that the benefits of torrents are probably irrelevant... at least until i increase the quality etc. torrent makes it scalable, which means it's mostly just an optimization and therefore optional
//TODOoptional: this involves more wrapping the ffmpeg call, which i'm considering doing, but after that is done i think each ffmpeg session should get it's own folder (perhaps based on start timestamp). otherwise, previous sessions will have their video files overwritten, which may be undesireable/accidental behavior
//^related, but barely: i could do a dir watcher (but 'know' the filename (since automating ffmpeg, is easy) of the segment file ahead of time) that transforms into a file watcher, so i don't have to do the "touch" stuff below. ffmpeg would make it then, and i think it would happen after the first segment finishes

//1) touch segments list file (or truncate to zero). i think it might need to be initially created by ffmpeg, i'm seeing weird inconsistencies with this
//2) launch this app (segments file must exist)
//3) launch ffmpeg

//TODOoptional: only if segment upload is successful, queue for delete maybe 24 hours later? these aren't the master copies, but maybe needed if a server crashes before propagation. deleting after 24 hours will keep the hdd from filling up, which would require manual intervention
//^^perhaps as a cli flag, omitted = no delete

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FfmpegSegmentUploaderCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
