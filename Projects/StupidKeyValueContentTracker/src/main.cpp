#include <QCoreApplication>

//modelled LOOSELY around git, but key/value store instead of fs. fs is so 1990s ;oP
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
