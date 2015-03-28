#include <QApplication>

#include "nothingfancynotepadwithautofilenamebutmanualsavemainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("NothingFancyNotepadWithAutoFilenameButManualSaveOrganizationName");
    QCoreApplication::setOrganizationDomain("NothingFancyNotepadWithAutoFilenameButManualSaveOrganizationDomain");
    QCoreApplication::setApplicationName("NothingFancyNotepadWithAutoFilenameButManualSave");

    NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow w;
    w.show();

    return a.exec();
}
