#include <QCoreApplication>

#include "qthttpsclienttest.h"

//My last attempt to get this https server working. This app just mimics WHAT I THINK IS an https client: ssl socket + http client (derp "GET /path"). It is to confirm that the ssl server is at least functioning correctly, which, if it works, "proves" to me that there's MORE to an [ACTUAL] https server/client than this attempt
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtHttpsClientTest c;
    Q_UNUSED(c)

    return a.exec();
}
