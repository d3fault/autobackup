#ifndef LOCALFILE_H
#define LOCALFILE_H

#include <QObject>

class LocalFile : public QObject
{
    Q_OBJECT
public:
    explicit LocalFile(QObject *parent = 0);

signals:

public slots:
    void handleNewData(QByteArray* newData);
};

#endif // LOCALFILE_H
