#include "hackyvideobullshitsite.h"

//NOTE: this app will be deployed on multiple servers, each of which having a copy of "my brain" directory. the order in which filesystem entries are written must be synchronized between each deployment server, since i'm using qdiriterator as an optimization for not listing 1800 files in my minddump folder. OT: an easy way to accomplish this is to use qdiriterator itself to read what file to copy next xD, or to do them in alphabetical order or whatever. i can't use 'newest first' for such writing-order because i intend to be actively adding files, so it would become broken right then. 'oldest first' actually makes the most sense and is the only one that wouldn't get broken when a single file is inserted xD
int main(int argc, char* argv[])
{
    HackyVideoBullshitSite hackyVideoBullshitSite;
    return hackyVideoBullshitSite.startHackyVideoBullshitSiteAndWaitForFinished(argc, argv);
}
