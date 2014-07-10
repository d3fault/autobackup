#ifndef STEVENHEREANDNOWGUI_H
#define STEVENHEREANDNOWGUI_H

#include <QObject>
#include <QScopedPointer>

class StevenHereAndNowWidget;

class StevenHereAndNowGui : public QObject
{
    Q_OBJECT
public:
    explicit StevenHereAndNowGui(QObject *parent = 0);
private slots:
    void handleStevenHereAndNowReadyForConnections(QObject *stevenHereAndNowAsQObject);
private:
    QScopedPointer<StevenHereAndNowWidget> m_GuiPtr;
};

#endif // STEVENHEREANDNOWGUI_H
