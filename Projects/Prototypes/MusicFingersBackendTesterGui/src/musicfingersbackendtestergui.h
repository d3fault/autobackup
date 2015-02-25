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
};

#endif // MUSICFINGERSBACKENDTESTERGUI_H
