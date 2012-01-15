#ifndef DECODER_H
#define DECODER_H

#include <QObject>

class Decoder : public QObject
{
    Q_OBJECT
public:
    explicit Decoder(QObject *parent = 0);

signals:

public slots:
    void handleNewInputData(QByteArray *newInputData);

};

#endif // DECODER_H
