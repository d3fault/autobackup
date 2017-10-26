#ifndef UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H
#define UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H

#include <QSaveFile>

#include <memory>

#include <QCryptographicHash>
#include <QTextStream>

class UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification : public QSaveFile
{
    Q_OBJECT
public:
    explicit UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification(const QString &name, QObject *parent = nullptr);

    static const QCryptographicHash::Algorithm CryptographicHashAlgorithm;
    static const QString ModificationGuardHeaderPrefix;

    bool openFileForOverwritingAfterVerifyingItHasntBeenModified();
    bool confirmAndDoOverwrite();
protected:
    qint64 writeData(const char *data, qint64 len) override;
private:
    explicit UnmodifiedFileSafeOverwriter_ViaCryptographicHashHeaderVerification(QObject *parent = nullptr);

    static QByteArray quickHexHash(const QByteArray &input);
    static bool fileContentsMatchCryptographicHash(QTextStream &t, const QByteArray &cryptographicHexHashToCheckAgainst);

    bool commit();
    void myE(QString msg);
    bool openFileForOverwriting();

    //bool m_FirstCallToWriteData;
signals:
    void e(QString);
};

#endif // UNMODIFIEDFILESAFEOVERWRITER_VIACRYPTOGRAPHICHASHHEADERVERIFICATION_H
