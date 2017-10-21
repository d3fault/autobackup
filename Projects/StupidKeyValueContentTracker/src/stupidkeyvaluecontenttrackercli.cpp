#include "stupidkeyvaluecontenttrackercli.h"

#include <QCoreApplication>

//TODOreq: non-interactive 'add' does nothing, because the staged mutations are not persisted when app exits
//^I'm thinking of maybe doing just add and bulkAdd, dropping the whole "commit" concept. but idk yet tbh it's hard to think about "users of" this lib, which may be written years from now, and whether or not they'll NEED add/commit capabilities
//^^if I keep 'commit', then it makes sense to have a 'status' command to show the staged mutations
//TODOreq: when I read in 2 TimeAndData_Timeline entries with the same datetime (but different data) from QSettings and stuffing them into a MultiMap, am I guaranteed to read them in using the same order? are they sorted by key _AND_ value, or I think yea just key is the only sort, so this needs addressing. it needs to be defined
StupidKeyValueContentTrackerCli::StupidKeyValueContentTrackerCli(QObject *parent)
    : QObject(parent)
    , m_StandardInputNotifier(nullptr)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
    , m_AppArgs(qApp->arguments())
    , m_Interactive(false)
{
    m_AppArgs.removeFirst(); //app name
    connect(this, &StupidKeyValueContentTrackerCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);
}
void StupidKeyValueContentTrackerCli::main()
{
    emit initializeRequested();
}
void StupidKeyValueContentTrackerCli::showUsage()
{
    handleE("usage:");
    handleE("ex0: ./thisApp add key data");
    handleE("ex1: ./thisApp get key");
    handleE("ex3: ./thisApp commit commitMessage");
    handleE("ex2: ./thisApp --interactive");
}
void StupidKeyValueContentTrackerCli::myE(const QString &msg)
{
    handleE(msg);
    showUsage();
    emit exitRequested(1);
}
void StupidKeyValueContentTrackerCli::processArgs()
{
     //TODOreq:
    // ex0: ./thisApp add key data
    // ex1: ./thisApp get key
    // ex3: ./thisApp commit commitMessage
    // ex2: ./thisApp --interactive

    //TODOreq: should a non-interactive "add" command _default_ to committing implicitly? ex: ./app add --no-commit key data; ./app add --no-commit key2 data2; ./app commit boobs
    //OR, should it behave like git and not commit after each add? I could ofc allow many adds in a single line, hell even many commands xD. KISS for now, but the first question of the comment 1 line above is still relevant

    if(m_AppArgs.isEmpty())
    {

        myE(""); //usage() implied
        return;
    }

    if(m_AppArgs.contains("--interactive"))
    {
        if(m_AppArgs.size() > 1)
        {
            myE("error: --interactive must be the only arg (for now)"); //TODOreq
            return;
        }
        handleO("type \"quit\" to exit");
        m_Interactive = true;
        m_StandardInputNotifier = new StandardInputNotifier(this);
        connect(m_StandardInputNotifier, &StandardInputNotifier::standardInputReceivedLine, this, &StupidKeyValueContentTrackerCli::handleStandardInputReceivedLine);
        return; //do nothing until the user types something and presses enter
    }

    processCommandAndCommandArgs(m_AppArgs);
}
void StupidKeyValueContentTrackerCli::processCommandAndCommandArgs(QStringList commandAndCommandArgs)
{
    //list.first is command (or we error out), and following are it's args

    QString command_orEmptyIfNoneSpecified = "";
    QStringList commandArgs;
    while(!commandAndCommandArgs.isEmpty())
    {
        QString nextItem = commandAndCommandArgs.takeFirst();
        if(command_orEmptyIfNoneSpecified.isEmpty())
        {
            command_orEmptyIfNoneSpecified = nextItem;
            continue;
        }
        else
        {
            commandArgs << nextItem;
            continue;
        }
    }
    if(command_orEmptyIfNoneSpecified.isEmpty())
    {
        myE("error: no command specified and --interactive flag not given");
        return;
    }
    command_orEmptyIfNoneSpecified = command_orEmptyIfNoneSpecified.toLower();
    if(command_orEmptyIfNoneSpecified == "add")
    {
        if(commandArgs.size() != 2)
        {
            myE("error: add command takes exactly 2 arguments");
            return;
        }
        emit addRequested(commandArgs.at(0), commandArgs.at(1));
        return;
    }
    else if(command_orEmptyIfNoneSpecified == "get")
    {
        if(commandArgs.size() != 1)
        {
            myE("error: get command takes exactly 1 argument");
            return;
        }
        QString revision; //TODOreq:
        emit readKeyRequested(commandArgs.at(0), revision);
        return;
    }
    else if(command_orEmptyIfNoneSpecified == "commit")
    {
        if(commandArgs.size() != 1)
        {
            myE("error: commit command takes exactly 1 argument");
            return;
        }
        emit commitRequested(commandArgs.at(0));
        return;
    }
    else
    {
        myE("error: unknown command: " + command_orEmptyIfNoneSpecified);
        return;
    }
}
void StupidKeyValueContentTrackerCli::quitIfNotInteractive(bool success)
{
    if(!m_Interactive)
    {
        emit exitRequested(success ? 0 : 1);
    }
}
void StupidKeyValueContentTrackerCli::handleE(QString msg)
{
    m_StdErr << msg << endl;
}
void StupidKeyValueContentTrackerCli::handleO(QString msg)
{
    m_StdOut << msg << endl;
}
void StupidKeyValueContentTrackerCli::handleInitializationFinished(bool success)
{
    if(success)
    {
        handleO("initializated successfully");
    }
    else
    {
        handleE("initialization failed");
    }
    processArgs();
}
void StupidKeyValueContentTrackerCli::handleAddFinished(bool success)
{
    if(success)
    {
        handleO("add finished successfully");
        //this line was here for testing: emit commitRequested("testCommitMessage0");
    }
    else
    {
        handleE("add failed");
    }
    quitIfNotInteractive(success);
}
void StupidKeyValueContentTrackerCli::handleCommitFinished(bool success)
{
    if(success)
    {
        handleO("commit finished successfully");
    }
    else
    {
        handleE("commit failed");
    }
    quitIfNotInteractive(success);
}
void StupidKeyValueContentTrackerCli::handleReadKeyFinished(bool success, QString key, QString revision, QString data)
{
    if(success)
    {
        handleO("readKeyFinished finished successfully");
        handleO(key + " : ");
        handleO(data);
    }
    else
    {
        handleE("readKeyFinished failed");
    }
    quitIfNotInteractive(success);
}

void StupidKeyValueContentTrackerCli::handleStandardInputReceivedLine(const QString &line)
{
    QString interactiveCommandMb = line.trimmed().toLower();
    if(interactiveCommandMb == "quit" || interactiveCommandMb == "exit") //TODOmb: "help"
    {
        emit exitRequested(0);
        return;
    }
    QStringList lineSplitAtSpaces = line.split(" ", QString::SkipEmptyParts);
    processCommandAndCommandArgs(lineSplitAtSpaces);
}
