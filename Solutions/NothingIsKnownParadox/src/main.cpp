//I wonder if that heisenburg theory of system observation is relevant. By analyzing ourselves (trying to determine whether or not a thing is known), we alter ourselves (we learn whether or not a thing is known)

//LoL'd as I just had this thought: what am I fumbling with here? Something... or nothing? (as in... is this lol project significant or is it just stupid fun nonsense?)

//I'm getting to lazy to continue working on it and I fucked up the design hardcore (or maybe I didn't, lolidkmybffjill) but meh whatever...

#include <QtCore/QCoreApplication>

#include "nothingisknownparadox.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    NothingIsKnownParadox n;
    Q_UNUSED(n)

    return a.exec();
}
