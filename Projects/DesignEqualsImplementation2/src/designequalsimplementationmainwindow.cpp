#include "designequalsimplementationmainwindow.h"

//TODOreq: a toolbar that changes based on which kind of project view tab is open. ALWAYS have the "move" (mouse icon) button on far left regardless of toolbar's other contents. in class diagram view, some other buttons are inheritence arrows. in use case view, some other buttons are "signal/slot connection activation" arrows. when in any "arrow" mode, pressing the mouse "move" thing is your way of cancelling out (like tons of apps do)
//TODOreq: zooming in and out should only shrink/enlarge the objects ("class" uml object, for example). arrow width (or really any object's width tbh) and font size should always stay the same (but text can re-wrap/whatever based on new object size)
DesignEqualsImplementationMainWindow::DesignEqualsImplementationMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
}
DesignEqualsImplementationMainWindow::~DesignEqualsImplementationMainWindow()
{

}
