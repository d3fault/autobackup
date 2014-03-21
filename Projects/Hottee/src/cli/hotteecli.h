#ifndef HOTTEECLI_H
#define HOTTEECLI_H

#include <QObject>
#include <QTextStream>

class Hottee;

class HotteeCli : public QObject
{
    Q_OBJECT
public:
    explicit HotteeCli(QObject *parent = 0);
    void startHotteeAndWaitUntilDone();
private:
    void usage();
    QTextStream m_StdOut;
    Hottee *m_Hottee;
private slots:
    void handleD(const QString &msg);
    void handleChangeAlternateSoon(const QString &destinationAboutToChangeTo);
    void handleDestinationToggled(const QString &destinationChangedFrom);
    void handleAboutToQuit();
};

#endif // HOTTEECLI_H
