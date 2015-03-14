#ifndef MUSICFINGERSBACKENDTESTERGUI_H
#define MUSICFINGERSBACKENDTESTERGUI_H

#include <QObject>

class MusicFingersBackendTesterGui : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingersBackendTesterGui(QObject *parent = 0);
public slots:
    void handleMusicFingersReadyForConnections(QObject *musicFingersAsQObject);
private slots:
    void handleO(const QString &msg);
};

#endif // MUSICFINGERSBACKENDTESTERGUI_H
