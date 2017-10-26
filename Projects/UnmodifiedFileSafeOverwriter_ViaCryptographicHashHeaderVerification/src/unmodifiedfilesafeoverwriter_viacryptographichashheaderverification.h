#ifndef UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H
#define UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H

#include <QBuffer>

#include <memory>

#include <QCryptographicHash>
#include <QTextStream>

class UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification : public QBuffer
{
    Q_OBJECT
public:
    explicit UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification(QObject *parent = 0)=delete;
    explicit UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification(const QString &fileName, QObject *parent = nullptr);
    explicit UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification(QByteArray *byteArray, QObject * parent = nullptr)=delete;

    static const QCryptographicHash::Algorithm CryptographicHashAlgorithm;
    static const QString ModificationGuardHeaderPrefix;

    bool openFileForOverwritingAfterVerifyingItHasntBeenModified();
    bool commit();
private:
    virtual bool open(OpenMode flags) override;
    virtual void close() override;

    static QByteArray quickHexHash(const QByteArray &input);
    static bool fileContentsMatchCryptographicHash(QTextStream &t, const QByteArray &cryptographicHexHashToCheckAgainst);

    void myE(QString msg);

    QString m_FileName;
signals:
    void e(QString);
};

#endif // UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H
