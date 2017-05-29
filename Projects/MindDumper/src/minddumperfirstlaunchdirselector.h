#ifndef MINDDUMPERFIRSTLAUNCHDIRSELECTOR_H
#define MINDDUMPERFIRSTLAUNCHDIRSELECTOR_H

#include <QDialog>

class LabelLineEditBrowseButton;

class MindDumperFirstLaunchDirSelector : public QDialog
{
    Q_OBJECT
public:
    explicit MindDumperFirstLaunchDirSelector(const QString &optionalSeededDirectoryProbablyPassedInAsAppArg = QString(), QWidget *parent = 0);
    QString selectedDir() const;
private:
    static inline void removeTrailingSlashIfPresentButNotIfItsTheOnlyChar(QString *inputString) { int inputStringLength = inputString->length(); if(inputString->endsWith("/") || inputStringLength < 2) return; *inputString = inputString->left(inputStringLength-1); }
    static inline QString appendSlashIfNeeded(const QString &inputString) { if(inputString.endsWith("/")) return inputString; return inputString + "/"; }

    LabelLineEditBrowseButton *m_MindDumpDirectoryRow;
};

#endif // MINDDUMPERFIRSTLAUNCHDIRSELECTOR_H
