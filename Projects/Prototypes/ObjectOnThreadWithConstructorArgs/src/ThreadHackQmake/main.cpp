#include <QtCore/QCoreApplication>
#include <QStringList>

#include "threadhackqmake.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList argList = a.arguments();
    if(argList.size() != 3)
    {
        return -1;
    }
    QString command = argList.at(0);
    int ret = 0;
    if(command == QString("-pre"))
    {
        ret = ThreadHackQMake::runPreQmakeHack(argList.at(1), argList.at(2));
    }
    else if(command == QString("-post"))
    {
        ret = ThreadHackQMake::runPostQmakeHack(argList.at(1), argList.at(2));
    }
    else
    {
        ret = -2;
    }

    if(ret == 0)
    {
        return a.exec();
    }
    return ret;
}
