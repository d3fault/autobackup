#ifndef EASYTREEGUIBUSINESS_H
#define EASYTREEGUIBUSINESS_H

#include <QObject>
#include <QFile>

#include "easytree.h"

class EasyTreeGuiBusiness : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeGuiBusiness(QObject *parent = 0);
private:
    EasyTree m_EasyTree;
signals:
    void d(const QString &);
    void doneGeneratingEasyTreeFile();
public slots:
    void generateEasyTreeFile(const QString &dirToTree, const QString &treeOutputFilePath);
};

#endif // EASYTREEGUIBUSINESS_H
