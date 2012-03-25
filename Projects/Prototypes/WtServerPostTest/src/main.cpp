#include <QtCore/QCoreApplication>
#include <QDebug>

#include "QtAwareWtApp.h"
#include <Wt/WServer>
using namespace Wt;

WApplication *createApplication(const WEnvironment& env, WServer &server)
{
    return new QtAwareWtApp(env, server);
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WServer server(argv[0]);
    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    server.addEntryPoint(Wt::Application, boost::bind(createApplication, _1, boost::ref(server)));

    if(server.start())
    {
      int sig = Wt::WServer::waitForShutdown();
      qDebug() << "Shutting down: (signal = " << sig << ")";
      server.stop();
      return sig;
    }
    return 1;
}
