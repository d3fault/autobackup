#ifndef GENERATEDDATABUFFER_H
#define GENERATEDDATABUFFER_H

#include <QObject>

class GeneratedDataBuffer : public QObject
{
    Q_OBJECT
public:
    explicit GeneratedDataBuffer(QObject *parent = 0);
    int getCapacity();
    int getSize();
private:
    int m_Capacity;
    int m_Size;
signals:

public slots:

};

#endif // GENERATEDDATABUFFER_H
