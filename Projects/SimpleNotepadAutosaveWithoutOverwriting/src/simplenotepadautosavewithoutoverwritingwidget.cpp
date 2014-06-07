#include "simplenotepadautosavewithoutoverwritingwidget.h"

#include <QDate>
#include <QDir>
#include <QFile>
#include <QDateTime>
//#include <QPlainTextEdit>
#include <QCryptographicHash>
#include <QDataStream>
#include <QTextStream>
#include <QVBoxLayout>
#include <QTimer>

#include "signalemittingonkeyeventsplaintextedit.h"

SimpleNotepadAutosaveWithoutOverwritingWidget::SimpleNotepadAutosaveWithoutOverwritingWidget(const QString &autoSaveTemplate = QString(), QWidget *parent)
    : QWidget(parent)
    , m_AutoSaveTemplate(autoSaveTemplate)
    , m_HaveStoppedTypingForThreeSecondsTimer(new QTimer(this))
{
    if(autoSaveTemplate.isEmpty())
    {
        m_AutoSaveTemplate = "autosave-%timestamp%-%contenthash%.txt"; //TODOreq: OT: i should folderize on the week. has nothing to do with when i save (had:saving)
    }

    m_PlainTextEdit = new SignalEmittingOnKeyEventsPlainTextEdit();
    connect(m_PlainTextEdit, SIGNAL(keyPressed(qint64,int)), this, SLOT()
    QVBoxLayout *theLayout = new QVBoxLayout(this);
    theLayout->addWidget(m_PlainTextEdit);

    m_HaveStoppedTypingForThreeSecondsTimer->setInterval(3000); //TODOoptional: specifiable. also worth nothing that if there's a visual cue for 3 secs, it might make make change the way i type to account for that (less disk writes = increasing disk life length)
    m_HaveStoppedTypingForThreeSecondsTimer->setSingleShot(true);
    connect(m_HaveStoppedTypingForThreeSecondsTimer, SIGNAL(timeout()), this, SLOT(haveStoppedTypingForThreeSecondsTimerTimedOut())); //seriously there are so many aspects to such a "simple" app :-P

    this->setLayout(theLayout);
}
bool SimpleNotepadAutosaveWithoutOverwritingWidget::save()
{
    QDate nowDate = QDate::currentDate();
    int weekIntoYear = nowDate.weekNumber();
    QString theDirTODOreq_WithSlashAppended = "/run/shm/" + weekIntoYear; //TODOreq: allow theDir to be specified
    QDir theDir(theDirTODOreq_WithSlashAppended);
    if(!theDir.mkpath(theDirTODOreq_WithSlashAppended))
    {
        emit e("failed to make week dir: " + theDirTODOreq_WithSlashAppended);
        return false;
    }
    //TODOreq: maybe: "dupe overwrite-or-whatever timestamp" recordings. when dupe detected, only use "dupe overwrite-or-whatever timestamp" when they are _ALSO_ a character for character (byte for byte) match. waiting for that dupe overwrite-or-whatever [with a different timestamp] present a great event to react to for when to lazily and rarely ever do byte for byte match (although if the system is abused they can be used differently (or perhaps the system must change somehow but i am unsure how as of this writing (perhaps if i stated (stared*))someNcloseParenthesis
    //TODOreq: auto parenthesis inserting like qt creator. my code does it but my comments don't :(. i like it in normal writings (one might even argue that i love them)
    //the dupe recordings can go in a file in the dir simply called "dupe.recordings.txt" with one entry per line (filename and timestamp entry (the last modified date is technically the first timestamp entry, so the entry (and hell even the file why not) is lazy created. filename contains content hash implicitly, and yea it MIGHT be a collission so it may be wise to check for byte for byte matches here (mabe once file is of certain size, we don't do byte for byte searched? or do i have that backwards actually for a different reason: big files , err the concept of infinity, make showing up on a shorter segment of bytes, more probable. correlation between size (inserted:of container) of file and

    //and what god damnit.
    //correlation between size and improbability
    //inverse correlation between size and probability
    //wowowowow
    //i've "proven using limits" that we exist
    //perhaps in a different way than i feel i already have before
    //or maybe i just worded it better

    //maybe: filenametimstapentryinquestion.txt.dupe.list.txt which stands for dupes of that filename which is a content hash because infinite file sysezesz mean
    //sizes, this is
    //that is
    //not systems
    //ok anyways: infinite file syzes means that collsiisions will always happen

    //it is also an extra maybe-read rofl, which means if we combine it as it is stored/read on with thefile itself, we will at most waste extra space if the number to indicate the extra payload makes our size of number require more bits but then that itself has a bit size fuckthing and there could theoretically be infinite but my brain hurts

    //where was i?
    //the chances of a collision given that i am already including timestamps in the filename (ROFL FORGOT!!!) is rare-ER, but i think that same err loop thing i barely remember also pplies
    //damn back to code, keep tangent'ing hardcore but it's kinda fun...
    //i should send the 3 copies of the keystrokes across the network, since i'm not sure checking a file's existence vs creating that file [and making sure it didn't already exist! failing if it did] is possible. but  i can ensure object state on a per thread (and therefore per network) basis. TODOreq: test the failing of each of those 3

    //blah simple save for now, ignoring chance of collission too
    QString filenameString = m_AutoSaveTemplate;
    filenameString.replace("%timestamp%", QString::number(QDateTime::currentMSecsSinceEpoch()));
    const QString &contentString = m_PlainTextEdit.toPlainText();
    const QByteArray &contentsUtf8ByteAray = contentString.toUtf8();
    QByteArray contentByteArray;

    {
        QDataStream properSerliaizingDataStream(&contentByteArray, QIODevice::WriteOnly);
        properSerliaizingDataStream << contentsUtf8ByteAray; //UTF-8 QString (had 16 here but changed my mind before compiling/etc), milliseconds in timestamp, and probably sha512 once i research it some

        //decided later: qstring being utf-16 is dumb when files are going utf-8. i'm now changing my serializing method to convert to utf-8 first, err no, the hashing method. yea fuck it actually ima leave the aww shit it will lead to inconsistoncies though and a weird silly dumb exploit
        //

        //yea fuck it just going to force utf-8 since even these run out always once we start encountering alien races (just a matter of time before another one grows behind us, which is guaranteed given that we grew)
    }
    const QByteArray &contentHashByteArray = QCryptographicHash::hash(contentByteArray, QCryptographicHash::Sha1/*sha1 is 4.8 compat but they all eventually become obsolete*/);

    QString base64contentHash;

    {
        QTextStream contentHashTextStream(&base64contentHash, QIODevice::WriteOnly);
        contentHashTextStream << contentHashByteArray.toBase64();
    }
    filenameString.replace("%contenthash%", base64contentHash);
    const QString &theFilename = theDirTODOreq_WithSlashAppended + filenameString;
    QFile theFile(theFilename);
    if(!theFile.open(QIODevice::WriteOnly, QIODevice::Text))
    {
        QTextStream theFileTextStream(&theFile);
        theFileTextStream << contentString; //converts to utf-8 (again but fuck it)
    }
    if(!theFile.flush())
    {
        emit e("failed to flush: " + theFilename);
        return false;
    }
    return true;
}
void SimpleNotepadAutosaveWithoutOverwritingWidget::handlePlainTextEditKeyPressed(qint64 timestamp /*these high precision timestamps are used for calculations, but they are averaged out with the "sentence" before serialized. otherwise they would take up shit tons of data*/, int key)
{
    m_HaveStoppedTypingForThreeSecondsTimer->start(); // [re-]start
    //TODOoptional: use timestamp and key in some way (perhaps entry point to saving backspaces, which will come later) now this should be TODOreq

    //TODOreq: oh right i want to save on punctation as well as 3 second pauses
}
void SimpleNotepadAutosaveWithoutOverwritingWidget::haveStoppedTypingForThreeSecondsTimerTimedOut()
{
    save(); //TODOoptional: use bool in some way, but idk how [EXACTLY (yet?)] tbh :(

    //TODOreq: this is, could be, AND MOST LIKELY IS, an increment/OVERWRITE to this file... meaning we lost the "opened" timestamp. shit exactly idfk how simple i want this FIRST version to be but i do know of awesome features i want in this app ultimately when i run into them...

    //levelDB is nice for high workloads... and as long as you have 3 copies on the network already. so AGAIN, how "simple" to make this first one?

    //I SAY: KISS MAKE IT FUNCTIONAL EVEN IF GREATLY LACKING BUT SOMETHING TO USE TO KEEP YOU INTERESTED _AND_ TESTING, THEN HACK IN ONE FEATURE AT A TIME
}

//To "save" by just replicating to neighbors is a good way to save without making permanent, because it only ever touches ram that way. hdd and sdd/flash are permanent
