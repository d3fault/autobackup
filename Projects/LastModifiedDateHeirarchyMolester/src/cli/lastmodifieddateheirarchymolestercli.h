#ifndef LASTMODIFIEDDATEHEIRARCHYMOLESTERCLI_H
#define LASTMODIFIEDDATEHEIRARCHYMOLESTERCLI_H

#include <QObject>
#include <QCoreApplication>
#include <QTextStream>

#include "lastmodifieddateheirarchymolester.h"

class LastModifiedDateHeirarchyMolesterCli : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedDateHeirarchyMolesterCli(QObject *parent = 0);
private:
    LastModifiedDateHeirarchyMolester *m_Molester; //TODOreq really fucking important: heap/new alloc so we can delete it in a slot connected to "aboutToQuit". If it was just a regular member, a.exec() might not return and therefore "this" might not go out of scope (so Molester's deconstructor would never run! (*shakes fist at Windows*) -- glad I was smart enough to catch that error :-P... makes me think of all the noobs out there that probably didn't (and I'm even thinking about posting a PSA to the mailing list to be nice...)
    QTextStream m_StdInStream;
    QTextStream m_StdOutStream;

    void usage();
    void quit();
    static const QString m_CreationDateArgFlag;
    static const QString m_XmlFormatFromTreeCommandActuallyArgFlag;
private slots:
    void handleD(const QString &msg);
    void handleAboutToQuit();
public slots:
    void parseArgsAndThenMolestLastModifiedDateHeirarchy();
};

#endif // LASTMODIFIEDDATEHEIRARCHYMOLESTERCLI_H
