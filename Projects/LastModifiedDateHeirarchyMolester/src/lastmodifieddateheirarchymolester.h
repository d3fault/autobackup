#ifndef LASTMODIFIEDDATEHEIRARCHYMOLESTER_H
#define LASTMODIFIEDDATEHEIRARCHYMOLESTER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#include "easytreehashitem.h"
#include "filemodificationdatechanger.h"

class LastModifiedDateHeirarchyMolester : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedDateHeirarchyMolester(QObject *parent = 0);
private:
    FileModificationDateChanger m_FileModificationDateChanger;
signals:
    void d(const QString &);
public slots:
    void molestLastModifiedDateHeirarchy(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &easyTreeFilePath, bool easyTreeLinesHaveCreationDate);
};

#endif // LASTMODIFIEDDATEHEIRARCHYMOLESTER_H
