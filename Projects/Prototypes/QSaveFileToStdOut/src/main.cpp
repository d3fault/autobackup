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

#if 1 //16 lines -- honestly though 'proxy io device' is pretty ugly solution so the ONE EXTRA LINE (*gasp*) might be worth it... but also lol I can probably condense a lot of these lines/parts xD
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
    outputStream.flush();
    if(!output.commitIfSaveFile())
    {
        qDebug("failed to commit save file");
        return 1;
    }
#endif

#if 0 //17 lines
    QIODevice *ioDevice;
    if(arguments.isEmpty())
        ioDevice = new QFile(&a);
    else
        ioDevice = new QSaveFile(arguments.takeFirst(), &a);
    if(!ioDevice->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open iodevice");
        return 1;
    }
    QTextStream dsfkj(ioDevice);
    dsfkj << "yolo" << endl;
    dsfkj.flush();
    if(qobject_cast<QSaveFile*>(ioDevice) && (!static_cast<QSaveFile*>(ioDevice)->commit()))
    {
        qDebug("failed to commit");
        return 1;
    }
#endif

#if 0 //21 lines
    bool isSaveFile = false;
    QIODevice *ioDevice;
    if(arguments.isEmpty())
        ioDevice = new QFile(&a);
    else
    {
        isSaveFile = true;
        ioDevice = new QSaveFile(arguments.takeFirst(), &a);
    }
    if(!ioDevice->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open iodevice");
        return 1;
    }
    QTextStream dsfkj(ioDevice);
    dsfkj << "yolo" << endl;
    dsfkj.flush();
    if(isSaveFile && (!static_cast<QSaveFile*>(ioDevice)->commit()))
    {
        qDebug("failed to commit");
        return 1;
    }
#endif
    return 0;
}
