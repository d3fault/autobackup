#ifndef UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H
#define UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H

#include <QIODevice>

#include <memory>

#include <QCryptographicHash>
#include <QTextStream>

class QSaveFile;

class UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification : public QIODevice
{
    Q_OBJECT
public:
    explicit UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification(const QString &fileName, QObject *parent = nullptr);

    static const QByteArray ModificationGuardHeaderPrefix;
    QByteArray DummyCryptographicHashForSizingPurposes;

    virtual bool open(OpenMode weAlwaysOpenInWriteOnlyTruncateNoMatterWhat = (QIODevice::WriteOnly | QIODevice::Truncate)) override;
    bool commit();
private:
    virtual void close() override;
    virtual bool isSequential() const override;
    virtual bool seek(qint64 pos) override;

    virtual qint64 readData(char *data, qint64 maxSize) override;
    virtual qint64 writeData(const char *data, qint64 maxSize) override;

    qint64 myHeaderSize_InclNewline() const;
    bool myActualOpenForWriting();
    QByteArray getHasherResultEncoded();
    bool fileContentsMatchCryptographicHash(QTextStream &t, const QByteArray &cryptographicHexHashToCheckAgainst);

    void myE(QString msg);

    bool m_FirstCallToWriteData;
    QSaveFile *m_SaveFile;
    QCryptographicHash m_Hasher;
signals:
    void e(QString);
};

#endif // UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H
