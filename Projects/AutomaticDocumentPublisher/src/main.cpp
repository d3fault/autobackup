#include <QCoreApplication>

//TODOreq: atm I'm assuming that the picture/etc post-commit githook already asked the publisher 'service' to publish the pic, so we can assume we are the only instance of ourselves running. I need to actually code the 'service' QLocalServer shiz and the post-commit QLocalSocket shiz but that's just busy work
//TODOoptimization: the root index.html file could use a nagle's optimization to decrease [re-]writes.... ALTHOUGH then again the linux fs buffer probably does a good enough job of this (only writes to memory at first)... so maybe don't?
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
