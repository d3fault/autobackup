#include "easytreeguibusiness.h"

EasyTreeGuiBusiness::EasyTreeGuiBusiness(QObject *parent) :
    QObject(parent)
{ }
void EasyTreeGuiBusiness::generateEasyTreeFile(const QString &dirToTree, const QString &treeOutputFilePath)
{
    QFile easyTreeOutputFile(treeOutputFilePath);
    if(!easyTreeOutputFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit d("failed to open easy tree output file for writing");
        return;
    }

    QList<QString> *dirNamesToIgnore = new QList<QString>();
    QList<QString> *fileNamesToIgnore = new QList<QString>();
    QList<QString> *fileNamesEndWithIgnoreList = new QList<QString>();
    QList<QString> *dirNamesEndsWithIgnoreList = new QList<QString>();

    //TODOoptional: make the GUI/etc utilize the file/folder full/endsWith filters (seen below as empty QStringLists)
    m_EasyTree.generateTreeText(dirToTree, &easyTreeOutputFile, dirNamesToIgnore, fileNamesToIgnore, fileNamesEndWithIgnoreList, dirNamesEndsWithIgnoreList);

    delete dirNamesToIgnore;
    delete fileNamesToIgnore;
    delete fileNamesEndWithIgnoreList;
    delete dirNamesEndsWithIgnoreList;

    easyTreeOutputFile.flush();
    easyTreeOutputFile.close();

    emit d("successfully wrote easy tree output file");

    emit doneGeneratingEasyTreeFile();
}
