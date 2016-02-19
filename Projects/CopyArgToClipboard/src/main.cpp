#include <QGuiApplication>
#include <QClipboard>
#include <QTextStream>

//I find myself using xfce4-terminal's auto-completion in order to type out long paths and filenames etc. Sometimes they are relative, sometimes they are absolute. I usually use the "file" command or "ls" command as a sort of noop just to let me use the auto-completion. My next action is usually to double click that path/filename and to copy it to my clipboard. Using a mouse on this 1fps monitor is a pain in the ass, and using it in general is a pain in the ass just because when I'm outside the "mousepad" is a small section of wood littered with cables around it (knock one, system goes poof), and when I'm in my tent mode then navigating my hands around inside my sleeping bag is also a pain. So I've been learning tons of keyboard commands and I'm actually kinda liking it, but this extremely simple app came to mind to give me one less place I have to use the mouse, and well yea. The most common place I use it is when providing a relative path to a .pro file's "include" command for .pri files
int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QStringList argz = a.arguments();
    QTextStream stdOut(stdout);
    if(argz.size() < 2)
    {
        stdOut << "Error: no argument provided" << endl;
        return 1;
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(argz.at(1));
    stdOut << "Argument copied" << endl;
    return 0;
}
