#include <QApplication>

#include "designequalsimplementationgui.h"

//TODOreq: the deserializing + source generation should be pushed into a lib [and cli] for automation (*cough*build-chain*cough*), but core UML editting stuff doesn't need to (and probably couldn't (well... (fuck this "well")))
//LoL: if this design (first of all makes it far enough) ever gets too complicated or I realize I took some wrong directions after the fact, I can re-design it WITHIN ITSELF and then discard the old one :-P..... over and over infinitely...
//I keep fantacizing about this app ultimately generating __ANSI C__, Boost, or Qt... as a radio option at source generation time. ANSI C would be the coolest xD, and also (had:easily) the hardest.... BUT imfo every single C++ feature in existence can be coded in ANSI C. It's so stupid that the C++ committee keeps bumping versions and requirements and dependencies blah. Their definition of backwards compatibility is source compatibility, whereas I like to keep the runtime the same always and just keep making the precompiler fatter and fatter :-). C89/ANSI-C is pretty much the largest supported language/runtime in existence (and will probably continue to be for.. like.. ever). I'm not sure whether or not pthreads is ANSI-C though (GNU Pth is, but doesn't look like it supports win. Oh nvm anyways looks like Pth is a thread emulator only)
//^^At the very least, the "library" generation type mode (when no threads involved) could very easily be written in ANSI-C (but actually since I'm conceptuatlly using "slot invokes" instead of "method calls", thread safety is implied when designing so... bleh. This app is first and foremost written for Qt obviously)
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DesignEqualsImplementationGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
