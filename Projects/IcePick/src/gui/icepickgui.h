#ifndef ICEPICKGUI_H
#define ICEPICKGUI_H

#include <QObject>

class IcePickWidget;

class IcePickGui : public QObject
{
    Q_OBJECT
public:
    explicit IcePickGui(QObject *parent = 0);
    virtual ~IcePickGui();
private:
    IcePickWidget *m_Gui;
private slots:
    void handleIcePickReadyForConnections(QObject *icePickAsQObject);
};

#endif // ICEPICKGUI_H
