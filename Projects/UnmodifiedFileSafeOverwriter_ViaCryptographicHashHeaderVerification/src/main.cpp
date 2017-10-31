#include <QCoreApplication>

#include <QDebug>
#include <QTextStream>
#include <QDateTime>

#include "unmodifiedfilesafeoverwriter_viacryptographichashheaderverification.h"
#include "debutoutput.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification f("/run/shm/test.txt");
    DebugOutput d;
    QObject::connect(&f, &UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification::e, &d, &DebugOutput::handleE);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug("failed to open file for overwriting");
        qDebug() << f.errorString();
        return 1;
    }

    {
        QTextStream t(&f);
        qsrand(QDateTime::currentMSecsSinceEpoch());
        t << "here is a randomly genererated number: " << qrand() << endl; //TODOreq: this app doesn't like trailing newlines, ie if there is a trailing newline in the target file then it thinks the file was modified, the hashing fails! I think it's because we're reading BY newline, reading it AS a delimeter, NOT including it in the m_Hasher.addData call! the only reason I thought this worked before was because my file (excluding the header) was only a single line! this shit probably doesn't work with multiple lines either and I'm pretty sure that's why
    }

    if(!f.commit())
    {
        qDebug() << "failed to commit file";
        return 1;
    }
    qDebug() << "success";
    return 0;
}
