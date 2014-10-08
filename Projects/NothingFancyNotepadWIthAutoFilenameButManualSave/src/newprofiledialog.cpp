#include "newprofiledialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>
#include <QCheckBox>

#include "labellineeditbrowsebutton.h"

NewProfileDialog::NewProfileDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setWindowTitle("New Profile");

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *profileNameRow = new QHBoxLayout();
    QLabel *profileNameLabel = new QLabel(tr("Profile Name:"));
    m_ProfileNameLineEdit = new QLineEdit();
    profileNameRow->addWidget(profileNameLabel);
    profileNameRow->addWidget(m_ProfileNameLineEdit);

    m_AutoSaveBaseDirRow = new LabelLineEditBrowseButton(tr("Base Dir for Auto-Saving"), LabelLineEditBrowseButton::OpenDirectoryMode);

    m_FolderizeCheckbox = new QCheckBox(tr("Folderize within Base Dir. Year folder, month folder, day folder"));
    m_FolderizeCheckbox->setChecked(true);

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButton->setDefault(true);
    okCancelRow->addWidget(cancelButton);
    okCancelRow->addWidget(okButton);

    myLayout->addLayout(profileNameRow);
    myLayout->addLayout(m_AutoSaveBaseDirRow);
    myLayout->addWidget(m_FolderizeCheckbox);
    myLayout->addLayout(okCancelRow);

    setLayout(myLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(sanitizeAndMaybeAcceptUserInput()));
    connect(m_ProfileNameLineEdit, SIGNAL(returnPressed()), this, SLOT(sanitizeAndMaybeAcceptUserInput()));

    m_ProfileNameLineEdit->setFocus();
}
QString NewProfileDialog::newProfileName()
{
    return m_ProfileNameLineEdit->text();
}
QString NewProfileDialog::newProfileBaseDir()
{
    return appendSlashIfNeeded(m_AutoSaveBaseDirRow->lineEdit()->text());
}
bool NewProfileDialog::folderizeBaseDir()
{
    return m_FolderizeCheckbox->isChecked();
}
void NewProfileDialog::sanitizeAndMaybeAcceptUserInput()
{
    if(m_ProfileNameLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Profile name cannot be blank"));
        return;
    }
    QFileInfo profileBaseDirFileInfo(m_AutoSaveBaseDirRow->lineEdit()->text());
    if(!profileBaseDirFileInfo.exists())
    {
        QMessageBox::critical(this, tr("Error, not exist"), tr("Your base dir does not exist"));
        return;
    }
    if(!profileBaseDirFileInfo.isDir())
    {
        QMessageBox::critical(this, tr("Error, not dir"), tr("Your base dir is not a directory"));
        return;
    }
    accept();
}
