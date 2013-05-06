#ifndef SYMBOLICLINKSDETECTOR_H
#define SYMBOLICLINKSDETECTOR_H

#include <QObject>
#include <QDir>
#include <QListIterator>

class SymbolicLinksDetector : public QObject
{
    Q_OBJECT
public:
    explicit SymbolicLinksDetector(QObject *parent = 0);
    void detectSymbolicLinks(QDir &searchDirectory, bool quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE);
private:
    QDir::Filters m_FiltersForDetectingSymbolicLinks; //TO DOnereq (lol nvm QFileInfo::isSymLink() does detect shortcuts on Windows after all!): .lnk files in windows are under System and could also be detected if we wanted... but I am a lot less worried about those getting "left out" than symbolic links... because symbolic links might actually have interesting content on the other end... whereas links are usually just bullshit shortcuts to pre-installed applications (USUALLY... of course... in reality, I see them as essentially the same fucking thing (it's a shame Qt doesn't also))
    bool m_QuitOnFirstDetected;
    bool m_QuitFlag;
    QFileInfo m_CurrentFileInfo;

    void detectSymbolicLinksRecursingIntoDirs(const QFileInfoList &currentDirectoryTree);
signals:
    void d(const QString &);
};

#endif // SYMBOLICLINKSDETECTOR_H
