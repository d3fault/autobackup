#ifndef HOTTEECLI_H
#define HOTTEECLI_H

#include <QObject>
#include <QTextStream>

#include "objectonthreadhelper.h"
#include "../lib/hottee.h"

class HotteeCli : public QObject
{
    Q_OBJECT
public:
    explicit HotteeCli(QObject *parent = 0);
    void cliUserInterfaceLoop();
private:
    QTextStream m_StdIn;
    QTextStream m_StdOut;
    ObjectOnThreadHelper<Hottee> m_BusinessThread;

    void usage();
signals:
    void startHotteeingRequested(const QString &inputProcessAndArgs, const QString &destination1, const QString &destination2, const QString &outputProcessAndArgs);
    void queryChunkWriteOffsetAndStorageCapacityStuffRequested();
    void startWritingAtNextChunkStartRequested();
    void stopWritingAtEndOfThisChunkRequested();
    void quitAfterThisChunkFinishesRequested();
private slots:
    void handleD(const QString &msg);
    void handleHotteeReadyForConnections();
    void handleAboutToQuit();
};

#endif // HOTTEECLI_H
