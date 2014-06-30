#ifndef IOSIOSOFFICENOTEPADUI_H
#define IOSIOSOFFICENOTEPADUI_H

#include <Qt>

class IOsiosOfficeNotepadUi
{
    virtual ~IOsiosOfficeNotepadUi() { }
    //signals, working around qobject diamond inheritance
protected:
    virtual void myKeyPressedSignal(Qt::Key key)=0;
};

#endif // IOSIOSOFFICENOTEPADUI_H
