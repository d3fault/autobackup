#include <Wt/WApplication>
using namespace Wt;

#include <QtCore/QCoreApplication>
#include <QThread>

#include "AppDbHelper.h"
#include "wtusersession.h"

WApplication *createApplication(const WEnvironment&)
{
    return new WtUserSession(env, server);
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread broadcastServerThread;
    AppDbHelper *AppDbHelper = AppDbHelper::Instance();
    AppDbHelper->moveToThread(&broadcastServerThread);
    AppDbHelper.start();

    //...todo "init" appDbHelper and WRun
}

