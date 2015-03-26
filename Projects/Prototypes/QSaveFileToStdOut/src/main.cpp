#include <QCoreApplication>

#include <QSaveFile>
#include <QTextStream>
#include <QFile>

#include "savefileorstdout.h"

//Conclusion: either QSaveFile needs an "autoCommitOnDestruct" bool property, or a "SaveFileOrStdOut" IODevice inheriter (that wraps the below functionality) needs to be created. That SaveFileOrStdOut constructor should use stdout if an empty string is passed as the filename (or perhaps additionally a "-" (but eh maybe they want the "-" file!)). I think the SaveFileOrStdOut object is the better solution, because it hides ALL of the below logic, not just the "if(isSaveFile) commit()" parts of it. Certain types of functionality (namely QSaveFile::cancelWriting) can't be brought to that class, since stdout has no way of canceling.
//This brings about another question: where should I put small helper objects such as the one described? I guess GloballySharedClasses but idfk
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList arguments = a.arguments();
    arguments.removeFirst();

    QString fileArg;
    if(!arguments.isEmpty())
        fileArg = arguments.takeFirst();
    SaveFileOrStdOut output(fileArg);
    if(!output.open((QIODevice::WriteOnly /*QIODevice::ReadWrite bleh prolly can't open stdout in read mode, but maybe more clever hacks can be used to make it proxy two iodevices so that stdin can be read too!!! Most apps don't use a single file for both input and output, but recursive gpg signer is an example of one that does*/ | QIODevice::Text)))
    {
        qDebug("failed to open output device");
        return 1;
    }
    QTextStream outputStream(&output);
    outputStream << "yolo" << endl;

#if 0
    bool isSaveFile = false;
    QIODevice *ioDevice;
    if(arguments.isEmpty())
    {
        QFile *stdOut = new QFile(&a);
        if(!stdOut->open(stdout, QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug("failded sdfksdf");
            return 1;
        }
        ioDevice = stdOut;
    }
    else
    {
        isSaveFile = true;
        QSaveFile *blah = new QSaveFile(arguments.takeFirst(), &a);
        if(!blah->open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug("lsdkjflksdjfsdkljfsdlk");
            return 1;
        }
        ioDevice = blah;
    }

    QTextStream dsfkj(ioDevice);
    dsfkj << "yolo" << endl;

    if(isSaveFile)
    {
        if(!static_cast<QSaveFile*>(ioDevice)->commit())
        {
            qDebug("failed to commit");
            return 1;
        }
    }
#endif
    return 0;
}
