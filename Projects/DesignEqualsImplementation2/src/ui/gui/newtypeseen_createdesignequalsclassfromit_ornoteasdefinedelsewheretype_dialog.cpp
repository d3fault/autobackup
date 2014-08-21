#include "newtypeseen_createdesignequalsclassfromit_ornoteasdefinedelsewheretype_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(const QList<QString> listOfTypesToDecideOn, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    //TODOoptional: scroll bars in case there are a fucking shit load of new types (afaik, c[++] doesn't set a limit on num params)

    setWindowTitle(tr("For each of these never before seen types..."));

    QVBoxLayout *myLayout = new QVBoxLayout();

    //TODOreq: pending 'type' refactor

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButton->setDefault(true);
    okCancelRow->addWidget(cancelButton);
    okCancelRow->addWidget(okButton);

    myLayout->addLayout(okCancelRow);

    setLayout(myLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}
QList<QString> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::typesDecidedToBeDesignEqualsImplementationClass() const
{
    return QList<QString>();
}
QList<QString> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::typesDecidedToBeDefinedElsewhere() const
{
    return QList<QString>();
}
