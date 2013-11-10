#ifndef FILEMODIFICATIONDATECHANGER_H
#define FILEMODIFICATIONDATECHANGER_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QProcess>

class FileModificationDateChanger : public QObject
{
    Q_OBJECT
public:
    explicit FileModificationDateChanger(QObject *parent = 0);
    bool changeModificationDate(const QString &absolutePathOfFileToTouch, QDateTime newDateTime);
private:
    QProcess m_TouchProcess;
    static const QString m_TouchProcessPath;
signals:
    void d(const QString &);
};

#endif // FILEMODIFICATIONDATECHANGER_H
