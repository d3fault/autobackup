#ifndef HOTTEECLI_H
#define HOTTEECLI_H

#include <QObject>
#include <QSocketNotifier>
#include <QTextStream>

class Hottee;

class HotteeCli : public QObject
{
    Q_OBJECT
public:
    explicit HotteeCli(QObject *parent = 0);
private:
    QSocketNotifier m_StdInSocketNotifier; //fucking finally. damnit nvm, apparently doesn't work on windows (fuck you bill gates)
    QTextStream m_StdIn;
    QTextStream m_StdOut;
    Hottee *m_Hottee;

    void cliUsage();
    void cliUserInterfaceMenu();
signals:
    void startHotteeingRequested(const QString &inputProcessAndArgs, const QString &destination1, const QString &destination2, const QString &outputProcessAndArgs, qint64 outputProcessFilenameOffsetJoinPoint);
    void queryChunkWriteOffsetAndStorageCapacityStuffRequested();
    void startWritingAtNextChunkStartRequested();
    void restartOutputProcessOnNextChunkStartRequested();
    void stopWritingAtEndOfThisChunkRequested();
    void quitAfterThisChunkFinishesRequested();
private slots:
    void handleD(const QString &msg);
    void stdInHasLineOfInput();
    void handleQuitRequested();
};

#endif // HOTTEECLI_H
