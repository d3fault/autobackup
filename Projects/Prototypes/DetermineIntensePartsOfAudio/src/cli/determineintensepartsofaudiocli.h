#ifndef DETERMINEINTENSEPARTSOFAUDIOCLI_H
#define DETERMINEINTENSEPARTSOFAUDIOCLI_H

#include <QObject>

class DetermineIntensePartsOfAudioCli : public QObject
{
    Q_OBJECT
public:
    explicit DetermineIntensePartsOfAudioCli(QObject *parent = 0);
signals:
    void exitRequested(int exitCode);
private slots:
    void handleE(const QString &msg);
    void handleDeterminingIntensePartsOfAudioFinished(bool success, QList<quint64> msTimestampsOfIntensePartsOfAudio);
};

#endif // DETERMINEINTENSEPARTSOFAUDIOCLI_H
