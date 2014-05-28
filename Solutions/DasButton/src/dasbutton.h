#ifndef DASBUTTON_H
#define DASBUTTON_H

#include <QObject>
#include <QFile>
#include <QStringList>

class DasButton : public QObject
{
    Q_OBJECT
public:
    explicit DasButton(QObject *parent = 0);

private:
    QString m_PrefixAkaHomeDir;

    QString m_FilepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater;
    QString m_CopyrightHeaderUnmodified;
    QString m_CopyrightHeader_WithCstyleComments;
    QString m_CopyrightHeader_WithHashStyleComments;
    QString m_CopyrightHeader_WithPhpStyleComments;
    QString m_CopyrightHeader_WithNoComments;

    QString m_CopyrightHeader_WithXmlStyleCommentsErrorStyle;
    QString m_CopyrightHeader_WithXmlStyleComments;

    QStringList m_FileExtensionsWithCstyleComments;
    QStringList m_FileExtensionsWithSimpleHashStyleComments;
    QStringList m_FileExtensionsWithPhpStyleComments;
    QStringList m_FileExtensionsWithNoComments;
    QStringList m_FileExtensionsWithXmlStyleComments;
    //QStringList m_FileExtensionsWithHashStyleComments_AL <-- if just doing str replacing, regular hash style comments will do these

    QStringList m_HeaderExtensions;
    QStringList m_HeaderAllRightsReservedVariants;

    QStringList m_VideoExtensions;
    QStringList m_SpecialCaseRawAudioExtensions;
    QStringList m_AudioExtensions;
    QStringList m_ImageExtensions;

    bool pressPrivate(const QString &prefixAkaHomeDir, const QString &filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);

    bool readInCopyrightHeader();
    bool extractArchive();
    bool moveOldAndSemiOldArchivesFromTempExtractArea();
    bool unPrependCopyrightHeader();
    bool recursivelyUnprependCopyrightHeaderForDir(const QString &dir);
    bool molestUsingEasyTreeFilesWhileAccountingForCompressedFilenameChanges();
    bool myHackyMolestUsingEasyTreeFile(const QString &dirCorrespondingToEasyTreeFile, const QString &easyTreeFile);
    bool makeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives();
    bool myHackyMakeSimplifiedLastModifiedTimestampsFileForOldAndSemiOldArchives(const QString &absoluteFilePathToTree);

    bool fileExtensionIsInListOfExtensions(const QString &extension, const QStringList &extensionsList);
    bool replaceInFile(const QString &absoluteFilePath, const QString &stringToReplace, const QString &stringToReplaceWith);
    bool replaceInIOdevice(QIODevice *ioDeviceToReplaceStringIn, const QString &stringToReplace, const QString &stringToReplaceWith);
    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }
    inline bool myRename(const QString &renameSrc, const QString renameDest)
    {
        if(!QFile::rename(renameSrc, renameDest))
        {
            emit e("failed to rename: '" + renameSrc + "' to '" + renameDest + "'");
            return false;
        }
        return true;
    }
signals:
    void pressFinished(bool pressWasSuccessful);
    void o(const QString &);
    void e(const QString &);
public slots:
    void press(const QString &prefixAkaHomeDir, const QString &filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater);
};

#endif // DASBUTTON_H
