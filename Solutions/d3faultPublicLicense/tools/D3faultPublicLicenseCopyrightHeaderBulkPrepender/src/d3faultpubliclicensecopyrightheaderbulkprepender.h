#ifndef D3FAULTPUBLICLICENSECOPYRIGHTHEADERBULKPREPENDER_H
#define D3FAULTPUBLICLICENSECOPYRIGHTHEADERBULKPREPENDER_H

#include <QObject>
#include <QStringList>
#include <QHash>

class QIODevice;

class D3faultPublicLicenseCopyrightHeaderBulkPrepender : public QObject
{
    Q_OBJECT
public:
    explicit D3faultPublicLicenseCopyrightHeaderBulkPrepender(QObject *parent = 0);
private:
    enum CopyrightHeaderFileStyleEnum { NoCommentsPlaintextFileStyle, CFileStyle, PhpFileStyle, SimpleHashFileStyle, ShellScriptHashFileStyle, XmlFileStyle, CStyleHeaderProprietaryAllRightsReservedForNow };

    QHash<CopyrightHeaderFileStyleEnum, QStringList> m_FileStyleExtensionsHash;
    QHash<CopyrightHeaderFileStyleEnum, QString> m_LazyFormattedCopyrightHeaderHash;

    QString m_CopyrightHeaderUnmodified;

    bool bulkPrependD3faultPublicLicenseCopyrightHeaderPrivate(const QString &dirToPrependTo, const QString &optionalAlternateCopyrightHeaderFilePath);
    uint whereToInsertCopyrightHeadersForFileStyle(CopyrightHeaderFileStyleEnum copyrightHeaderFileStyle);
    QString lazyFormatCopyrightHeaderForFileStyle(CopyrightHeaderFileStyleEnum copyrightHeaderFileStyle);
    QString formatCopyrightHeaderForFileStyle(CopyrightHeaderFileStyleEnum copyrightHeaderFileStyle);
    bool insertFormattedCopyrightHeaderIntoFileAtLine(uint lineToInsertAt, const QString &formattedCopyrightHeaderText, const QString &absoluteFilePathOfFileToInsertCopyrightHeaderInto);
    bool insertFormattedCopyrightHeaderIntoIoDeviceAtLine(uint lineToInsertAt, const QString &formattedCopyrightHeaderText, QIODevice *ioDeviceToInsertCopyrightHeaderInto);
signals:
    void bulkPrependD3faultPublicLicenseCopyrightHeaderFinished(bool success);
    void o(const QString &);
    void e(const QString &);
public slots:
    void bulkPrependD3faultPublicLicenseCopyrightHeader(const QString &dirToPrependTo, const QString &optionalAlternateCopyrightHeaderFilePath);
};

#endif // D3FAULTPUBLICLICENSECOPYRIGHTHEADERBULKPREPENDER_H
