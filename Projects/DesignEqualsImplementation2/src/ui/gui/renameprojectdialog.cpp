#include "renameprojectdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "../../designequalsimplementationproject.h"

RenameProjectDialog::RenameProjectDialog(DesignEqualsImplementationProject *project, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_Project(project)
{
    setWindowTitle(tr("Rename Project"));

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *projectNameRow = new QHBoxLayout();
    QLabel *projectNameLabel = new QLabel(tr("Project &Name:"));
    m_ProjectNameLineEdit = new QLineEdit(project->Name);
    projectNameLabel->setBuddy(m_ProjectNameLineEdit);
    projectNameRow->addWidget(projectNameLabel);
    projectNameRow->addWidget(m_ProjectNameLineEdit, 1);

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButton->setDefault(true);
    okCancelRow->addWidget(cancelButton);
    okCancelRow->addWidget(okButton);

    myLayout->addLayout(projectNameRow);
    myLayout->addLayout(okCancelRow);
    setLayout(myLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));

    m_ProjectNameLineEdit->setFocus();
    m_ProjectNameLineEdit->selectAll(); //implies set focus?
}
void RenameProjectDialog::handleAccepted()
{
    const QString &newProjectName = m_ProjectNameLineEdit->text();
    if(newProjectName.trimmed().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Your project name was either empty or pure whitespace, so it is not being renamed"));
        return;
    }
    m_Project->Name = newProjectName; //TODOreq: use reactor pattern, setName/nameChanged/etc instead, and update gui
}
