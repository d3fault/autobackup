#ifndef SFTPUPLOADANDRENAMEALLVIDEOSEGMENTSINFOLDERCLI_H
#define SFTPUPLOADANDRENAMEALLVIDEOSEGMENTSINFOLDERCLI_H

#include <QObject>

#include <QTextStream>

class SftpUploadAndRenameAllVideoSegmentsInFolderCli : public QObject
{
    Q_OBJECT
public:
    explicit SftpUploadAndRenameAllVideoSegmentsInFolderCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void cliUsage();
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
};

#endif // SFTPUPLOADANDRENAMEALLVIDEOSEGMENTSINFOLDERCLI_H
