#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    explicit Synchronizer(QObject *parent = 0);

signals:
    void d(const QString &);
    //TODO: void showFrame(QVideoFrame);
public slots:
    void audioTimeUpdated(quint64 microsecondsElapsed);
};

#endif // SYNCHRONIZER_H
