#include "watchsigsfileandpostchangestousenetcli.h"

#include <QCoreApplication>

#include "watchsigsfileandpostchangestousenet.h"
#include "standardinputnotifier.h"

#define WatchSigsFileAndPostChangesToUsenetCli_SIGSFILE "--sigs-file"
#define WatchSigsFileAndPostChangesToUsenetCli_FILES_DIR "--files-dir"
#define WatchSigsFileAndPostChangesToUsenetCli_APP_DATA_DIR "--app-data-dir"
#define WatchSigsFileAndPostChangesToUsenetCli_USER_ARG "--authuser" //If I did modularilize this app and there was a PostFileToUsenetCli, then in there should live teh parsing of these args, and then I could call that parse function from here to save lines/etc. It should parse these args into an object/pod-struct -- makes me wonder if QCommandLineParser has any way to deal with different libs/clis like that, I suppose each one could be passed a pointer to the command line parser (and register callbacks? idfk haven't thought it through much)
#define WatchSigsFileAndPostChangesToUsenetCli_PASS_ARG "--authpass"
#define WatchSigsFileAndPostChangesToUsenetCli_PORT_ARG "--port"
#define WatchSigsFileAndPostChangesToUsenetCli_SERVER_ARG "--server"

#define EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli { cliUsage(); QMetaObject::invokeMethod(qApp, "exit", Qt::QueuedConnection, Q_ARG(int, 1)); return; }

#define MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(stringVariableName, cliArg) \
QString stringVariableName; \
int stringVariableName##Index = arguments.indexOf(cliArg); \
if(stringVariableName##Index > -1) \
{ \
    arguments.removeAt(stringVariableName##Index); \
    if(arguments.size() < (stringVariableName##Index+1)) \
        EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli \
    userArg = arguments.takeAt(stringVariableName##Index); \
} \
else \
    EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli

WatchSigsFileAndPostChangesToUsenetCli::WatchSigsFileAndPostChangesToUsenetCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    QStringList arguments = qApp->arguments();

    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(sigsFilePathToWatch, WatchSigsFileAndPostChangesToUsenetCli_SIGSFILE)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(dirCorrespondingToSigsFile, WatchSigsFileAndPostChangesToUsenetCli_FILES_DIR)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(dataDirForKeepingTrackOfAlreadyPostedFiles, WatchSigsFileAndPostChangesToUsenetCli_APP_DATA_DIR)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(authUser, WatchSigsFileAndPostChangesToUsenetCli_USER_ARG)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(authPass, WatchSigsFileAndPostChangesToUsenetCli_PASS_ARG)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(portString, WatchSigsFileAndPostChangesToUsenetCli_PORT_ARG)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(server, WatchSigsFileAndPostChangesToUsenetCli_SERVER_ARG)

    //TODOoptional: make sure port is a number (but we still want it as a string lol so fuck it)
    if(!arguments.empty())
    {
        handleE("unaccounted for arguments: " + arguments.join(", "));
        EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli
    }

    WatchSigsFileAndPostChangesToUsenet *watchSigsFileAndPostChangesToUsenet = new WatchSigsFileAndPostChangesToUsenet(this);
    connect(watchSigsFileAndPostChangesToUsenet, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(watchSigsFileAndPostChangesToUsenet, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(watchSigsFileAndPostChangesToUsenet, SIGNAL(doneWatchingSigsFileAndPostingChangesToUsenet(bool)), this, SLOT(handleDoneWatchingSigsFileAndPostingChangesToUsenet(bool)));
    connect(this, SIGNAL(quitCleanlyRequested()), watchSigsFileAndPostChangesToUsenet, SLOT(quitCleanly()));

    StandardInputNotifier *standardInputNotifier = new StandardInputNotifier(this);
    connect(standardInputNotifier, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceivedLine(QString)));

    connect(this, &WatchSigsFileAndPostChangesToUsenetCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);


    TODO LEFT OFF
    //QMetaObject::invokeMethod(watchSigsFileAndPostChangesToUsenet)
}
void WatchSigsFileAndPostChangesToUsenetCli::cliUsage()
{
    handleE("Usage: WatchSigsFileAndPostChangesToUsenetCli " WatchSigsFileAndPostChangesToUsenetCli_USER_ARG " username-goes-here " WatchSigsFileAndPostChangesToUsenetCli_PASS_ARG " password-goes-here " WatchSigsFileAndPostChangesToUsenetCli_PORT_ARG " port-goes-here "/*" yep-you-guessed-it "*/ WatchSigsFileAndPostChangesToUsenetCli_SERVER_ARG " usenet-server-goes-here");
}
void WatchSigsFileAndPostChangesToUsenetCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void WatchSigsFileAndPostChangesToUsenetCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void WatchSigsFileAndPostChangesToUsenetCli::handleStandardInputReceivedLine(const QString &userInputLine)
{
    if(userInputLine.toLower() == "q")
    {
        emit o("quitting once the file currently being posted finishes posting");
        emit quitCleanlyRequested(); //emit quitRequested(); //oshi-, this time the frontend is requesting a quit from the backend :-P. TODOreq: cleanly shutdown (finish posting current file, serialize posted files for next time). TODOoptional: qq (which should still serialize posted files, just cancel the current one is all (should qq return 1 or 0? i guess 0 but idk))
        disconnect(this, SIGNAL(quitRequested()));
        return;
    }
}
void WatchSigsFileAndPostChangesToUsenetCli::handleDoneWatchingSigsFileAndPostingChangesToUsenet(bool success)
{
    if(!success)
        handleE("failed at watching sigs file and posting changes to usenet :(");
    emit exitRequested(success ? 0 : 1);
}
