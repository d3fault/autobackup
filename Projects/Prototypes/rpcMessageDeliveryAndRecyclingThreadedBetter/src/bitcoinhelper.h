#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper(); //we choose not to have a parent so we don't ever become someone's child and follow them on moveToThread

signals:
    void initialized();
    void started();
    void stopped();
public slots:
    void init();
    void start();
    void stop();
};

#endif // BITCOINHELPER_H
