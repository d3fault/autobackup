#include <QApplication>

#include "designequalsimplementationgui.h"

//TODOreq: the deserializing + source generation should be pushed into a lib [and cli] for automation (*cough*build-chain*cough*), but core UML editting stuff doesn't need to (and probably couldn't (well... (fuck this "well")))
//LoL: if this design (first of all makes it far enough) ever gets too complicated or I realize I took some wrong directions after the fact, I can re-design it WITHIN ITSELF and then discard the old one :-P..... over and over infinitely...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DesignEqualsImplementationGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
