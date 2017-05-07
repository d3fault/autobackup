#include "automaticpicturepublisherwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AutomaticPicturePublisherGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
