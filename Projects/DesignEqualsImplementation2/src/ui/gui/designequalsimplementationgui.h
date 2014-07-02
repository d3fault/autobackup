#ifndef DESIGNEQUALSIMPLEMENTATIONGUI_H
#define DESIGNEQUALSIMPLEMENTATIONGUI_H

#include <QObject>

class DesignEqualsImplementationMainWindow;

class DesignEqualsImplementationGui : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationGui(QObject *parent = 0);
    ~DesignEqualsImplementationGui();
private:
    DesignEqualsImplementationMainWindow *m_Gui;
private slots:
    void handleDesignEqualsImplementationReadyForConnections(QObject *designEqualsImplementationAsQObject);
};

#endif // DESIGNEQUALSIMPLEMENTATIONGUI_H
