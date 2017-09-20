#ifndef GUI_H
#define GUI_H

#include <QObject>

class Gui : public QObject
{
    Q_OBJECT
public:
    explicit Gui(QObject *parent = 0);
private:
    int x = 69;
signals:
    void doFuckRequested();
};

#endif // GUI_H
