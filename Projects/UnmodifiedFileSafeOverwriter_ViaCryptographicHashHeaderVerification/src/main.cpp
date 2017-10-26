#include <QCoreApplication>

#include <QDebug>
#include <QTextStream>
#include <QDateTime>

#include "unmodifiedfilesafeoverwriter_viacryptographichashheaderverification.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification f("/run/shm/test.txt");
    if(!f.openFileForOverwritingAfterVerifyingItHasntBeenModified())
    {
        qDebug("failed to open file for overwriting");
        qDebug() << f.errorString();
        return 1;
    }

    {
        QTextStream t(&f);
        qsrand(QDateTime::currentMSecsSinceEpoch());
        t << "here is a randomly genererated number: " << qrand();
    }

    if(!f.commit())
    {
        qDebug() << "failed to commit file";
        return 1;
    }
    qDebug() << "success";
    return 0;
}
