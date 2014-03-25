#ifndef HOTTEECLI_H
#define HOTTEECLI_H

#include <QObject>
#include <QSocketNotifier>
#include <QTextStream>

#include "../lib/hottee.h"

class HotteeCli : public QObject
{
    Q_OBJECT
public:
    explicit HotteeCli(QObject *parent = 0);
private:
    QSocketNotifier m_StdInSocketNotifier; //fucking finally
    QTextStream m_StdIn;
    QTextStream m_StdOut;
    Hottee m_Hottee;

    void cliUsage();
    void cliUserInterfaceMenu();
signals:
    void startHotteeingRequested(const QString &inputProcessAndArgs, const QString &destination1, const QString &destination2, const QString &outputProcessAndArgs);
    void queryChunkWriteOffsetAndStorageCapacityStuffRequested();
    void startWritingAtNextChunkStartRequested();
    void stopWritingAtEndOfThisChunkRequested();
    void quitAfterThisChunkFinishesRequested();
private slots:
    void handleD(const QString &msg);
    void stdInHasLineOfInput();
};

#endif // HOTTEECLI_H
