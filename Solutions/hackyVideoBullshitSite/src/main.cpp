#include <Wt/WServer>
#include <QCoreApplication>
#include "hackyvideobullshitsite.h"

//TODOreq: make sure that at app startup, video files in the "watched" folder that are already there are moved into position in the same way videos added (at runtime) are. This might already be implemented, idfk
//NOTE: this app will be deployed on multiple servers, each of which having a copy of "my brain" directory. the order in which filesystem entries are written must be synchronized between each deployment server, since i'm using qdiriterator as an optimization for not listing 1800 files in my minddump folder. OT: an easy way to accomplish this is to use qdiriterator itself to read what file to copy next xD, or to do them in alphabetical order or whatever. i can't use 'newest first' for such writing-order because i intend to be actively adding files, so it would become broken right then. 'oldest first' actually makes the most sense and is the only one that wouldn't get broken when a single file is inserted xD

//I'd imagine lots would want to use this + abc2 themselves (yw)... so here's a note to anyone genricizing (making not-'d3fault' specific): abstract my specific stuff into some kind of "profile" loaded at runtime or whatever, that way i can pull your changes back in and spread the software further around :-P (row row fight the powa)
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    //TODOoptional: could embed my folder setup shell scripts (well, they're too specific to my needs atm, but later if genericized) into the binary and call them here/now if first run is detected (whether QSettings exists and/or is populated). Basically the installer and resulting executable can be one and the same...

    HackyVideoBullshitSite hackyVideoBullshitSite(argc, argv);
    Q_UNUSED(hackyVideoBullshitSite)

    return app.exec();
}
