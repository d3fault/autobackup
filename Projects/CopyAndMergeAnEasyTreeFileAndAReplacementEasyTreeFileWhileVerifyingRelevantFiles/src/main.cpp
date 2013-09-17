#include <QtGui/QApplication>

#include "copyandmergeaneasytreefileandareplacementeasytreefilewhileverifyingrelevantfilesgui.h"

//2 inputs: 1 regular easy tree file and 1 "use this as replacement" formatted
//performs copy of all entries in both files and checks hash as doing so for the 1 regular easy tree file (replacements get new hashes)
//1 output: 1 regular easy tree file, which has replacements now as regular entries

//I need to run an old/static/manual backup of everything current before running this app because it scares the crap out of me

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
