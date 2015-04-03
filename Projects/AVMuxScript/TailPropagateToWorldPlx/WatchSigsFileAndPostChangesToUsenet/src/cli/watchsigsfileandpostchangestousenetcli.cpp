#include "watchsigsfileandpostchangestousenetcli.h"

#include <QCoreApplication>

#include "watchsigsfileandpostchangestousenet.h"
#include "standardinputnotifier.h"

#define WatchSigsFileAndPostChangesToUsenetCli_SIGSFILE "--sigs-file"
#define WatchSigsFileAndPostChangesToUsenetCli_FILES_DIR "--files-dir"
//#define WatchSigsFileAndPostChangesToUsenetCli_APP_DATA_DIR "--app-data-dir"
#define WatchSigsFileAndPostChangesToUsenetCli_USER_ARG "--authuser" //If I did modularilize this app and there was a PostFileToUsenetCli, then in there should live teh parsing of these args, and then I could call that parse function from here to save lines/etc. It should parse these args into an object/pod-struct -- makes me wonder if QCommandLineParser has any way to deal with different libs/clis like that, I suppose each one could be passed a pointer to the command line parser (and register callbacks? idfk haven't thought it through much)
#define WatchSigsFileAndPostChangesToUsenetCli_PASS_ARG "--authpass"
#define WatchSigsFileAndPostChangesToUsenetCli_PORT_ARG "--port"
#define WatchSigsFileAndPostChangesToUsenetCli_SERVER_ARG "--server"

#define EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli { cliUsage(); emit exitRequested(1); return; }

#define MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(stringVariableName, cliArg) \
QString stringVariableName; \
int stringVariableName##Index = arguments.indexOf(cliArg); \
if(stringVariableName##Index > -1) \
{ \
    arguments.removeAt(stringVariableName##Index); \
    if(arguments.size() < (stringVariableName##Index+1)) \
        EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli \
    stringVariableName = arguments.takeAt(stringVariableName##Index); \
} \
else \
    EEEEEEEEEEE_WatchSigsFileAndPostChangesToUsenetCli

WatchSigsFileAndPostChangesToUsenetCli::WatchSigsFileAndPostChangesToUsenetCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    connect(this, &WatchSigsFileAndPostChangesToUsenetCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    QStringList arguments = qApp->arguments();
    arguments.takeFirst(); //app filename

    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(sigsFilePathToWatch, WatchSigsFileAndPostChangesToUsenetCli_SIGSFILE)
    MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(dirCorrespondingToSigsFile, WatchSigsFileAndPostChangesToUsenetCli_FILES_DIR)
    //MANDATORY_ARG_WatchSigsFileAndPostChangesToUsenetCli(dataDirForKeepingTrackOfAlreadyPostedFiles, WatchSigsFileAndPostChangesToUsenetCli_APP_DATA_DIR)
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
    connect(this, SIGNAL(printMessageIDsForRelativeFilePathRequested(QString)), watchSigsFileAndPostChangesToUsenet, SLOT(printMessageIDsForRelativeFilePath(QString)));
    connect(this, SIGNAL(printMessageIdCurrentlyBeingPostedRequested()), watchSigsFileAndPostChangesToUsenet, SLOT(printMessageIdCurrentlyBeingPosted()));
    connect(this, SIGNAL(quitCleanlyRequested()), watchSigsFileAndPostChangesToUsenet, SLOT(quitCleanly()));

    StandardInputNotifier *standardInputNotifier = new StandardInputNotifier(this);
    connect(standardInputNotifier, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceivedLine(QString)));


    QMetaObject::invokeMethod(watchSigsFileAndPostChangesToUsenet, "startWatchingSigsFileAndPostChangesToUsenet", Q_ARG(QString, sigsFilePathToWatch), Q_ARG(QString, dirCorrespondingToSigsFile)/*, Q_ARG(QString, dataDirForKeepingTrackOfAlreadyPostedFiles)*/, Q_ARG(QString, authUser), Q_ARG(QString, authPass), Q_ARG(QString, portString), Q_ARG(QString, server));
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
    const QString &userInputLineLowered = userInputLine.toLower();
    if(userInputLineLowered == "q")
    {
        handleO("quitting once the file currently being posted finishes posting");
        emit quitCleanlyRequested(); //emit quitRequested(); //oshi-, this time the frontend is requesting a quit from the backend :-P. TODOoptional: qq (which should still serialize posted files, just cancel the current one is all (should qq return 1 or 0? i guess 0 but idk))
        disconnect(this, SIGNAL(quitCleanlyRequested()));
        return;
    }
    if(userInputLineLowered.startsWith("m"))
    {
        if(userInputLineLowered.size() > 2 && userInputLineLowered.at(1) == QStringLiteral(" "))
        {
            emit printMessageIDsForRelativeFilePathRequested(userInputLineLowered.mid(2));
        }
        else
        {
            emit printMessageIdCurrentlyBeingPostedRequested(); //I was gonna do "previous" message ID (to get one that's already been posted), but it's much simpler [on my backend] to get the current one. With part sizes of 350kb, it won't take long for this 'current' message ID to become the 'previous' (and why I want it: so I can verify manually that this app is still working [and that my shit isn't getting SILENTLY filtered guh]))
        }
        return;
    }
    if(userInputLineLowered == "h")
    {
        handleO(QStringLiteral("Selections:\n\tM - Get Message-ID currently being posted\n\tM [relative file path] - Get all known MessageIDs for the specifies file path\n\tH - This Help\n\tQ - Quit Cleanly\n"));
    }
    //TODOmb: query the data file's LOCATION?
}
void WatchSigsFileAndPostChangesToUsenetCli::handleDoneWatchingSigsFileAndPostingChangesToUsenet(bool success)
{
    if(!success)
        handleE("failed at watching sigs file and posting changes to usenet :(");
    emit exitRequested(success ? 0 : 1); //i call this the "real world to posix" translation
}
