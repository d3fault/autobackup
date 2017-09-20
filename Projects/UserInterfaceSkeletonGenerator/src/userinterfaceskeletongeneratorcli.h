#ifndef USERINTERFACESKELETONGENERATORCLI_H
#define USERINTERFACESKELETONGENERATORCLI_H

#include <QObject>

class UserInterfaceSkeletonGeneratorCli : public QObject
{
    Q_OBJECT
public:
    explicit UserInterfaceSkeletonGeneratorCli(QObject *parent = 0);
};

#endif // USERINTERFACESKELETONGENERATORCLI_H
