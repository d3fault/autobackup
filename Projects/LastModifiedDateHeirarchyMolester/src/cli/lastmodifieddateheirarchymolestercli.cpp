#include "lastmodifieddateheirarchymolestercli.h"

const QString LastModifiedDateHeirarchyMolesterCli::m_CreationDateArgFlag = "-c";

LastModifiedDateHeirarchyMolesterCli::LastModifiedDateHeirarchyMolesterCli(QObject *parent) :
    QObject(parent), m_Molester(0), m_StdInStream(stdin), m_StdOutStream(stdout)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    m_Molester = new LastModifiedDateHeirarchyMolester(this); //Since we aren't using a backend thread, parent'ing is a good habit (even though aboutToQuit will always get signaled before 'this' goes out of scope IN THIS APP)

    //Trying to determine if I need another class to handle the stdin/stdout shit or if I should just put it in 'this'. I think ultimately CLIs are way less likely to be re-used modularly than a graphical Widget, so I can just do stdin/out processing here and have frontend "own" the backend. That's what I _THINK_, but I admit it may be wrong :-S

    connect(m_Molester, SIGNAL(d(QString)), this, SLOT(handleD(QString)));

    //Yet another easy/prolly-doesn't-matter/SEMANTICS/organizational question: do I, in main.cpp, connect a signal from 'this' called "finished" to QCoreApplication::quit, or do I simply invokeMethod quit from this class when the time comes!?!? I know they effectively do the same thing, but I'm sort of a perfectionist when it comes to code...
}
void LastModifiedDateHeirarchyMolesterCli::usage()
{
    m_StdOutStream << "Usage: LastModifiedDateHeirarchyMolesterCli [OPTION] DIRECTORY EASYTREEFILE" << endl;
    m_StdOutStream << "Molest (touch all the children in) DIRECTORY using timestamps obtained from EASYTREEFILE." << endl << endl;
    m_StdOutStream << "\t" << m_CreationDateArgFlag << "\t\tEasy Tree File contains 'Created' DateTime (obsolete, keeping for compatibility)" << endl;
}
void LastModifiedDateHeirarchyMolesterCli::quit()
{
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}
void LastModifiedDateHeirarchyMolesterCli::handleD(const QString &msg)
{
    m_StdOutStream << msg << endl;
}
void LastModifiedDateHeirarchyMolesterCli::handleAboutToQuit()
{
    if(m_Molester)
    {
        delete m_Molester;
        m_Molester = 0;
    }
}
void LastModifiedDateHeirarchyMolesterCli::parseArgsAndThenMolestLastModifiedDateHeirarchy()
{
    QStringList args = QCoreApplication::instance()->arguments();
    if((args.size() < 3) || args.contains("--help", Qt::CaseInsensitive) || args.contains("/?", Qt::CaseSensitive))
    {
        usage();
        quit();
        return;
    }
    bool easyTreeFileHasCreationDateTime = false;
    if(args.contains(m_CreationDateArgFlag, Qt::CaseSensitive))
    {
        easyTreeFileHasCreationDateTime = true;
        args.removeOne(m_CreationDateArgFlag);
    }
    if(args.size() != 3)
    {
        usage();
        quit();
        return;
    }

    m_Molester->molestLastModifiedDateHeirarchy(args.at(1), args.at(2), easyTreeFileHasCreationDateTime);
    quit();
    //well that was fun, now I [hope/think ;-P] know Qt/CLI as much as I do Qt/GUI. Was easy (and yea, I haven't even run/tested this code xD)
}
