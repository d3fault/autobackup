#ifndef DRUMMACHINE_H
#define DRUMMACHINE_H

#include <QObject>
#include <QHash>

class QSoundEffect;

class DrumMachine : public QObject
{
    Q_OBJECT
public:
    explicit DrumMachine(QObject *parent = 0);
private:
    QHash<Qt::Key, QSoundEffect*> m_KeysToSoundEffectsHash;
public slots:
    void drum(Qt::Key key);
};

#endif // DRUMMACHINE_H
