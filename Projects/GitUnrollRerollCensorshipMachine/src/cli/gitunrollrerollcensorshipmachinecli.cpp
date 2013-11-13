#include "gitunrollrerollcensorshipmachinecli.h"

GitUnrollRerollCensorshipMachineCli::GitUnrollRerollCensorshipMachineCli(QObject *parent) :
    QObject(parent), m_Business(0), m_StdOutStream(stdout)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    m_Business = new GitUnrollRerollCensorshipMachine(this);
    connect(m_Business, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
}
void GitUnrollRerollCensorshipMachineCli::usage()
{
    m_StdOutStream << "Usage: GitUnrollRerollCensorshipMachineCli FILE_CONTAINING_RELATIVE_FILEPATHS_TO_CENSOR SOURCE_DIR_TO_BE_CENSORED DESTINATION_DIR_FOR_CENSORED_GIT_REPO_RESULT [WORKING_DIR]" << endl;
    m_StdOutStream << "Unroll SOURCE_DIR_TO_BE_CENSORED to the first commit, ensure all filepaths in FILE_CONTAINING_RELATIVE_FILEPATHS_TO_CENSOR are censored, then add that 'censored checkout' to git repo in DESTINATION_DIR_FOR_CENSORED_GIT_REPO_RESULT. Checkout the next revision, censor, repeat... for entire history. The git commit date in the censored repo will be the same as the one in the pre-censored one. This application does not 'follow'/censor renamed files (shit out of luck)" << endl << endl << "Options:" << endl;
}
void GitUnrollRerollCensorshipMachineCli::quit()
{
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}
void GitUnrollRerollCensorshipMachineCli::parseArgsAndThenDoGitUnrollRerollCensoring()
{
    QStringList args = QCoreApplication::instance()->arguments();
    if((args.size() < 4) || args.contains("--help", Qt::CaseInsensitive) || args.contains("/?", Qt::CaseSensitive))
    {
        usage();
        quit();
        return;
    }

    //process options flags here

    QString workingDir = QDir::tempPath(); //we make a temp sub-dir (safely) and then remove it at the end -- or something TODOreq
    if(args.size() == 5) //options flags can go anywhere, but after they are all processed, "optional working dir" must be the very last (4th (0-index lewl)) argument
    {
        workingDir = args.at(4);
        args.removeAt(4);
    }
    if(args.size() != 4)
    {
        usage();
        quit();
        return;
    }

    m_Business->unrollRerollGitRepoCensoringAtEachCommit(args.at(1), args.at(2), args.at(3), workingDir);
    quit();
}
void GitUnrollRerollCensorshipMachineCli::handleD(const QString &msg)
{
    m_StdOutStream << msg << endl;
}
void GitUnrollRerollCensorshipMachineCli::handleAboutToQuit()
{
    if(m_Business)
    {
        delete m_Business;
        m_Business = 0;
    }
}
