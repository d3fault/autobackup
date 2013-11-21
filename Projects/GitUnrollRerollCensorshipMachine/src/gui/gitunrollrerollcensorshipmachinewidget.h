#ifndef GITUNROLLREROLLCENSORSHIPMACHINEWIDGET_H
#define GITUNROLLREROLLCENSORSHIPMACHINEWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDir>
#include <QFileDialog>
//TODOreq: QCheckBox to toggle d3fault's specific hacks :-P

class GitUnrollRerollCensorshipMachineWidget : public QWidget
{
    Q_OBJECT
public:
    GitUnrollRerollCensorshipMachineWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;

    QLabel *m_FilePathToListOfFilepathsToCensorLabel;
    QLineEdit *m_FilePathToListOfFilepathsToCensorLineEdit;
    QPushButton *m_FilePathToListOfFilepathsToCensorBrowseButton;

    QLabel *m_AbsoluteSourceGitDirToCensorLabel;
    QLineEdit *m_AbsoluteSourceGitDirToCensorLineEdit;
    QPushButton *m_AbsoluteSourceGitDirToCensorBrowseButton;

    QLabel *m_AbsoluteDestinationGitDirCensoredLabel;
    QLineEdit *m_AbsoluteDestinationGitDirCensoredLineEdit;
    QPushButton *m_AbsoluteDestinationGitDirCensoredBrowseButton; //TODOreq: will be in "save new file" mode, even though it's a directory (because it needs to not exist, and you can't select a non-existing directory xD)

    QLabel *m_AbsoluteWorkingDirLabel;
    QLineEdit *m_AbsoluteWorkingDirLineEdit;
    QPushButton *m_AbsoluteWorkingDirBrowseButton;

    QPushButton *m_DoUnrollRerollButton;

    QPlainTextEdit *m_Debug;
signals:
    void unrollRerollGitRepoCensoringAtEachCommitRequested(QString filePathToListOfFilepathsToCensor, QString absoluteSourceGitDirToCensor, QString absoluteDestinationGitDirCensored, QString absoluteWorkingDir);
private slots:
    void handleFilePathToListOfFilepathsToCensorBrowseButtonClicked();
    void handleAbsoluteSourceGitDirToCensorBrowseButtonClicked();
    void handleAbsoluteDestinationGitDirCensoredBrowseButtonClicked();
    void handleAbsoluteWorkingDirBrowseButtonClicked();
    void handleDoUnrollRerollButtonClicked();
public slots:
    void handleD(const QString &msg);
};

#endif // GITUNROLLREROLLCENSORSHIPMACHINEWIDGET_H
