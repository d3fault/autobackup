#ifndef SIMPLIFIEDHEIRARCHYMOLESTER_H
#define SIMPLIFIEDHEIRARCHYMOLESTER_H

#include <QObject>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QScopedPointer>

#include "simplifiedlastmodifiedtimestamp.h"
#include "filemodificationdatechanger.h"

class SimplifiedHeirarchyMolester : public QObject
{
    Q_OBJECT
public:
    explicit SimplifiedHeirarchyMolester(QObject *parent = 0);

signals:
    void d(const QString &);
public slots:
    void molestHeirarchy(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath);
};

#endif // SIMPLIFIEDHEIRARCHYMOLESTER_H
