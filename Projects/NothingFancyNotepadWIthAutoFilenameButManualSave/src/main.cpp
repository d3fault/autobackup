#include <QApplication>

#include "nothingfancynotepadwithautofilenamebutmanualsavemainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("NothingFancyNotepadWIthAutoFilenameButManualSaveOrganizationName");
    QCoreApplication::setOrganizationDomain("NothingFancyNotepadWIthAutoFilenameButManualSaveOrganizationDomain");
    QCoreApplication::setApplicationName("NothingFancyNotepadWIthAutoFilenameButManualSave");

    NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow w;
    w.show();

    return a.exec();
}
