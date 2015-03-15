#ifndef RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H
#define RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H

#include <QObject>

#include <QTextStream>

#define GPG_DEFAULT_PATH "/usr/bin/gpg" //TODOoptional: custom path as arg

class RecursiveCustomDetachedSignatures : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveCustomDetachedSignatures(QObject *parent = 0);
    bool readPathAndSignature(QTextStream &customDetachedGpgSignaturesTextStream, QString *out_FilePathToVerify, QString *out_CurrentFileSignature);
signals:
    void e(const QString &msg);
};

#endif // RECURSIVECUSTOMDETACHEDGPGSIGNATURES_H
