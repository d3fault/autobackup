#ifndef SYMBOLICLINKSDETECTORBUSINESS_H
#define SYMBOLICLINKSDETECTORBUSINESS_H

#include <QObject>
#include <QDir>

#include "symboliclinksdetector.h"

class SymbolicLinksDetectorBusiness : public QObject
{
    Q_OBJECT
public:
    explicit SymbolicLinksDetectorBusiness(QObject *parent = 0);
private:
    SymbolicLinksDetector m_SymbolicLinksDetector;
signals:
    void d(const QString &);
public slots:
    void detectSymbolicLinks(const QString &searchDirectoryString, bool quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE);
};

#endif // SYMBOLICLINKSDETECTORBUSINESS_H
