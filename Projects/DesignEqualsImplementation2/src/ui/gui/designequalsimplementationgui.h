#ifndef DESIGNEQUALSIMPLEMENTATIONGUI_H
#define DESIGNEQUALSIMPLEMENTATIONGUI_H

#include <QObject>
#include <QTextStream>

class DesignEqualsImplementationMainWindow;

class DesignEqualsImplementationGui : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationGui(QObject *parent = 0);
    ~DesignEqualsImplementationGui();
private:
    DesignEqualsImplementationMainWindow *m_Gui;
    QTextStream m_StdErr;
private slots:
    void handleDesignEqualsImplementationReadyForConnections(QObject *designEqualsImplementationAsQObject);
    void handleE(const QString &msg);
};

#endif // DESIGNEQUALSIMPLEMENTATIONGUI_H
