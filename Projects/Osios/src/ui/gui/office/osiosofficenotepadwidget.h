#ifndef OSIOSOFFICENOTEPADWIDGET_H
#define OSIOSOFFICENOTEPADWIDGET_H

#include <QWidget>
#include "../../../office/iosiosofficenotepadui.h"

class OsiosOfficeNotepadWidget : public QWidget, public IOsiosOfficeNotepadUi
{
    Q_OBJECT
public:
    explicit OsiosOfficeNotepadWidget(QWidget *parent = 0);
signals:
    void myKeyPressedSignal(Qt::Key key);
};

#endif // OSIOSOFFICENOTEPADWIDGET_H
