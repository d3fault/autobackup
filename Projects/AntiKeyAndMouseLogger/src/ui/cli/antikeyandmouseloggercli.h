#ifndef ANTIKEYANDMOUSELOGGERCLI_H
#define ANTIKEYANDMOUSELOGGERCLI_H

#include <QObject>

#include <QTextStream>

#include "antikeyandmouselogger.h"

class AntiKeyAndMouseLoggerCli : public QObject
{
    Q_OBJECT
public:
    explicit AntiKeyAndMouseLoggerCli(QObject *parent = 0);
    ~AntiKeyAndMouseLoggerCli();
private:
    QString m_Password;
    QTextStream m_StdOut;

    void setStdInEchoEnabled(bool enabled);
signals:
    void translateShuffledKeymapEntryRequested(const QString &shuffledKeymapEntry);
private slots:
    void presentShuffledKeymapPage(const KeyMap &shuffledKeymapPage);
    void handleStandardInputReceivedLine(const QString &line);
    void handleShuffledKeymapEntryTranslated(const KeyMapEntry &translatedKeymapEntry);
};

#endif // ANTIKEYANDMOUSELOGGERCLI_H
