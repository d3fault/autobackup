#include "gitsubmoduleautobackuphalperwidget.h"

GitSubmoduleAutoBackupHalperWidget::GitSubmoduleAutoBackupHalperWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();

    m_SaveProfileButton = new QPushButton(tr("Save Profile"));

    m_RootDirToMonitorLabel = new QLabel(tr("Root Dir To Monitor:"));
    m_RootDirToMonitorLineEdit = new QLineEdit();
    m_RootDirToMonitorBrowseButton = new QPushButton("Browse");
    QHBoxLayout *rootDirToMonitorHrow = new QHBoxLayout();
    rootDirToMonitorHrow->addWidget(m_RootDirToMonitorLabel);
    rootDirToMonitorHrow->addWidget(m_RootDirToMonitorLineEdit);
    rootDirToMonitorHrow->addWidget(m_RootDirToMonitorBrowseButton);

    m_Layout->addWidget(m_SaveProfileButton);
    m_Layout->addLayout(rootDirToMonitorHrow);
    m_Layout->addWidget(m_Debug);
    setLayout(m_Layout);

    connect(m_SaveProfileButton, SIGNAL(clicked()), this, SLOT(handleSaveProfileButtonClicked()));
}
void GitSubmoduleAutoBackupHalperWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void GitSubmoduleAutoBackupHalperWidget::handleSaveProfileButtonClicked()
{
    //TODOreq: Saves "profile name" (ideally typed in profile selection drop-down), "root dir to monitor (live non-bare dir)", "> 0 list of storage directories (bare repos (ex: 750x1/2))" to QSettings
}
