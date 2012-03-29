#include <Wt/WServer> //i think i just need to include this before any Qt headers so i don't need to use CONFIG += no_keywords in the .pro file. am unsure
using namespace Wt;

#include <QtCore/QCoreApplication>
#include <QDebug>

#include "wtusersession.h" //this has to be included both after WServer and QCoreApplication, as it depends on both

WApplication *createApplication(const WEnvironment& env, WServer &server)
{
    return new WtUserSession(env, server);
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
      return sig; //TODO: 0 instead?
    }
    return 1;
}

