#ifndef LIFESHAPERSTATICFILES_H
#define LIFESHAPERSTATICFILES_H

#include <QObject>
#include <QIODevice>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include "easytreeparser.h"
#include "easytreehashitem.h"

class LifeShaperStaticFiles : public QObject
{
    Q_OBJECT
public:
    explicit LifeShaperStaticFiles(QObject *parent = 0);
    ~LifeShaperStaticFiles();
    void startIteratingEasyTreeHashFile(QIODevice *easyTreeHashFile, QString outputFilesFolder, QString lineLeftOffFrom_OR_emptyString);
    void stopIteratingEasyTreeHashFile();
    void setRecursivelyApplyFromHereUntilParent(bool recursiveEnabled);
    void leaveBehind();
    void iffyCopyright();
    void deferDecision();
    void bringForward();
    void useTHISasReplacement(QString replacementFilePath);
private:
    bool m_RecursiveUntilParent; //TODOreq: this is dependent on the ordering of the EasyTreeHash file. Basically the root needs to be listed first and the deepest need to be listed later. A folders entries should all come in a burst, so to speak. This is how my file is already formatted but it might not always be the case in the future(???)
    QTextStream *m_TextStreamPointerForRecursionHacksLoL;
    bool m_FirstTimeInProcessNextLineForRecursion;
    QString m_RecursionStartsWithFilePathToDetectWhenParentComes;
    bool m_SpecialCaseForUseTHIStextStreamAndOnlyForDirs;

    QString m_LeaveBehindFilename;
    QString m_IffyCopyrightFilename;
    QString m_DeferDecisionFilename;
    QString m_BringForwardFilename;
    QString m_UseTHISasReplacementFilename;

    QFile m_LeaveBehindFile;
    QFile m_IffyCopyrightFile;
    QFile m_DeferDecisionFile;
    QFile m_BringForwardFile;
    QFile m_UseTHISasReplacementFile;

    QTextStream m_LeaveBehindTextStream;
    QTextStream m_IffyCopyrightTextStream;
    QTextStream m_DeferDecisionTextStream;
    QTextStream m_BringForwardTextStream;
    QTextStream m_UseTHISasReplacementTextStream;

    QTextStream m_EasyTreeHashTextStream;
    QString m_CurrentLine;
    EasyTreeHashItem *m_CurrentEasyTreeHashItem;
    void processCurrentEasyTreeHashLineRegularly();
    void processNextLineOrNotifyOfCompletion();
    bool outputFilesExistInFolder(QString basePath);
    void cleanUp();
signals:
    void nowProcessingEasyTreeHashItem(QString /*line*/, QString /*relativeFilePath*/, bool /*isDirectory*/, qint64 /*fileSize_OR_zeroIfDir*/, QString /*creationDateTime*/, QString /*lastModifiedDateTime*/);
    void stoppedRecursingDatOperationBecauseParentDirEntered();
    void finishedProcessingEasyTreeHashFile();
    void d(const QString &);
};

#endif // LIFESHAPERSTATICFILES_H
