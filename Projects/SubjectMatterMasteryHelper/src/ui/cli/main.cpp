#include <QCoreApplication>

#include "subjectmattermasteryhelpercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SubjectMatterMasteryHelperCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
