#include <QtCore/QCoreApplication>

#include <Wt/WApplication>
using namespace Wt;

WApplication *createApplication(const WEnvironment& env)
{
  return new HelloApplication(env);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    return WRun(argc, argv, &createApplication);
}
