#include <QCoreApplication>

#include "sftpuploadandrenameallvideosegmentsinfoldercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SftpUploadAndRenameAllVideoSegmentsInFolderCli cli; //there's a world of GPL'd software (i think sftp is BSD but that's besides the point) just a QProcess call away. qprocess invokin vs lib linkin, you lose your types and compile time checks/safety... but that's about it
    Q_UNUSED(cli)

    return a.exec();
}
