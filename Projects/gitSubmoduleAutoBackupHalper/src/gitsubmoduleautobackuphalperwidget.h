#ifndef GITSUBMODULEAUTOBACKUPHALPERWIDGET_H
#define GITSUBMODULEAUTOBACKUPHALPERWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QLabel>
#include <QLineEdit>

class GitSubmoduleAutoBackupHalperWidget : public QWidget
{
    Q_OBJECT
public:
    GitSubmoduleAutoBackupHalperWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;
    QPushButton *m_SaveProfileButton;

    QLabel *m_RootDirToMonitorLabel;
    QLineEdit *m_RootDirToMonitorLineEdit;
    QPushButton *m_RootDirToMonitorBrowseButton;

    //TODO LEFT OFF not going to do this right now because I determined it is a premature optimization and I don't have the time for it right now

    QPlainTextEdit *m_Debug;
public slots:
    void handleD(const QString &msg);
private slots:
    void handleSaveProfileButtonClicked();
};

#endif // GITSUBMODULEAUTOBACKUPHALPERWIDGET_H
