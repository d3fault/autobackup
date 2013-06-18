#ifndef LIFESHAPERSTATICFILESBUSINESS_H
#define LIFESHAPERSTATICFILESBUSINESS_H

#include <QObject>
#include <QFile>

#include "lifeshaperstaticfiles.h"

class LifeShaperStaticFilesBusiness : public QObject
{
    Q_OBJECT
public:
    explicit LifeShaperStaticFilesBusiness(QObject *parent = 0);
    ~LifeShaperStaticFilesBusiness();
private:
    LifeShaperStaticFiles m_LifeShaperStaticFiles;
    QFile m_EasyTreeHashFile;
    void cleanUp();
signals:
    void nowProcessingEasyTreeHashItem(QString /*line*/, QString /*relativeFilePath*/, bool /*isDirectory*/, qint64 /*fileSize_OR_zeroIfDir*/, QString /*creationDateTime*/, QString /*lastModifiedDateTime*/);
    void stoppedRecursingDatOperationBecauseParentDirEntered();
    void finishedProcessingEasyTreeHashFile();
    void d(const QString &);
public slots:
    void startIteratingEasyTreeHashFile(QString easyTreeHashFile, QString outputFilesFolder, QString lineLeftOffFrom_OR_emptyString);
    void stopIteratingEasyTreeHashFile();
    void setRecursivelyApplyFromHereUntilParent(bool recursiveEnabled);
    void leaveBehind();
    void iffyCopyright();
    void deferDecision();
    void bringForward();
    void useTHISasReplacement(QString replacementFilePath); //TODOreq: needs to be absolute path on fs when passed in, but use the old relative filename/path once slipped into place -- this is a detail for the app that will do the actual copying and reconstructing... this app just makes notes pretty much
};

#endif // LIFESHAPERSTATICFILESBUSINESS_H
