#include "minddumperfirstlaunchdirselector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QLabel>
#include <QStandardPaths>
#include <QPushButton>

#include "labellineeditbrowsebutton.h"

MindDumperFirstLaunchDirSelector::MindDumperFirstLaunchDirSelector(const QString &optionalSeededDirectoryProbablyPassedInAsAppArg, QWidget *parent)
    : QDialog(parent)
{
    bool prepopulateDirWithGuessGoodStartingPoint = optionalSeededDirectoryProbablyPassedInAsAppArg.isEmpty();
    QString mindDumpDirectoryPrepopulatedGuess;
    if(prepopulateDirWithGuessGoodStartingPoint)
    {
        //guesses:"documents" (if writable) -> desktop (dgaf if writable, but must exist) -> "/"
        QString guess = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); //TODOreq: guess. something like ~/ or /var/documents or fuck if I know. let Qt guess for you :). maybe the "desktop" is a good guess (most people don't know how to get to their documents folder (except using a shortcut, so actually yea maybe Documents is better guess dir. hell "Documents" dir could be used as minddump dir for all I care, since I generate filenames so safely))?
        if(guess.isEmpty())
        {
            QStringList desktopLocationsMaybe = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
            if(!desktopLocationsMaybe.isEmpty())
            {
                guess = desktopLocationsMaybe.first();
            }
            else
            {
                guess = "/";
            }
        }
        mindDumpDirectoryPrepopulatedGuess = guess;
    }
    else
    {
        mindDumpDirectoryPrepopulatedGuess = optionalSeededDirectoryProbablyPassedInAsAppArg;
    }

    removeTrailingSlashIfPresentButNotIfItsTheOnlyChar(&mindDumpDirectoryPrepopulatedGuess);

    QVBoxLayout *myLayout = new QVBoxLayout();
    QDateTime currentdateTime = QDateTime::currentDateTime();
    QString currentYearNumberAsString = QString::number(currentdateTime.date().year());
    QString currentMonthNumberAsString = QString::number(currentdateTime.date().month());
    QLabel *welcomeText = new QLabel("Welcome. Please choose a MindDumper directory to save your files in. In that directory, your files will be stored in a folder named after the year, and then a subfolder named after the month.\n\nSo for example if you choose: " + mindDumpDirectoryPrepopulatedGuess + "\nThen we will save files like this: " + appendSlashIfNeeded(mindDumpDirectoryPrepopulatedGuess) + currentYearNumberAsString + "/" + currentMonthNumberAsString + "/example.txt\n\n");
    welcomeText->setWordWrap(true);
    myLayout->addWidget(welcomeText);
    m_MindDumpDirectoryRow = new LabelLineEditBrowseButton("Save files in folder:", LabelLineEditBrowseButton::OpenDirectoryMode);
    myLayout->addLayout(m_MindDumpDirectoryRow);
    QHBoxLayout *quitAndOkButtonsRow = new QHBoxLayout();
    QPushButton *quitButton = new QPushButton("&Quit");
    connect(quitButton, &QPushButton::clicked, this, &MindDumperFirstLaunchDirSelector::reject);
    quitAndOkButtonsRow->addWidget(quitButton);
    QPushButton *okButton = new QPushButton("&Ok");
    connect(okButton, &QPushButton::clicked, this, &MindDumperFirstLaunchDirSelector::accept);
    quitAndOkButtonsRow->addWidget(okButton);
    myLayout->addLayout(quitAndOkButtonsRow);
    setLayout(myLayout);

    m_MindDumpDirectoryRow->lineEdit()->setText(mindDumpDirectoryPrepopulatedGuess);
}
QString MindDumperFirstLaunchDirSelector::selectedDir() const
{
    return m_MindDumpDirectoryRow->lineEdit()->text();
}
