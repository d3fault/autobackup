#include "newtypeseen_createdesignequalsclassfromit_ornoteasdefinedelsewheretype_dialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox> //was planning on using a radio, but it's more work/clutter...
#include <QHBoxLayout>
#include <QPushButton>

#include "../../designequalsimplementationproject.h"

#define NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog_COLUMN_OF_THE_LABEL 0
#define NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog_COLUMN_OF_THE_CHECKBOX 1

NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(const QList<QString> listOfTypesToDecideOn, DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_DesignEqualsImplementationProject(designEqualsImplementationProject)
{
    //TODOoptional: scroll bars in case there are a fucking shit load of new types (afaik, c[++] doesn't set a limit on num params)

    //too long and unseen, maybe set minwidth for it? setWindowTitle(tr("For each of these never before seen types..."));
    setWindowTitle(tr("New types..."));

    QVBoxLayout *myLayout = new QVBoxLayout();

    m_NewTypesGridLayout = new QGridLayout();
    int currentRow = 0;
    Q_FOREACH(const QString &currentTypeToDecideOn, listOfTypesToDecideOn)
    {
        m_NewTypesGridLayout->addWidget(new QLabel(currentTypeToDecideOn/* + ":"*/), currentRow, 0);
        QCheckBox *internallyDesignedOrExternallyDefinedCheckbox = new QCheckBox(tr("Create Class"));
        internallyDesignedOrExternallyDefinedCheckbox->setToolTip(tr("If you un-check this, the new type will still be usable, but it will be noted as being 'defined elsewhere' (such as in a 3rd party library). You can even specify the necessary #includes that will be placed automatically whenever the externally defined type is referenced in the project from the blah blah blah dialog")); //TODOreq: make the blah blah blah dialog and explain properly how to get there
        internallyDesignedOrExternallyDefinedCheckbox->setChecked(true);
        m_NewTypesGridLayout->addWidget(internallyDesignedOrExternallyDefinedCheckbox, currentRow, NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog_COLUMN_OF_THE_CHECKBOX);
        ++currentRow;
    }

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButton->setDefault(true);


    okCancelRow->addWidget(cancelButton);
    okCancelRow->addWidget(okButton);

    myLayout->addLayout(m_NewTypesGridLayout);
    myLayout->addLayout(okCancelRow);

    setLayout(myLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this, SIGNAL(accepted()), this, SLOT(handleDialogAccepted()));
}
QList<QString> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::typesDecidedToBeDesignEqualsImplementationClass() const
{
    return buildTypesListDependingOnWhetherCreateClassIsChecked(true);
}
QList<QString> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::typesDecidedToBeDefinedElsewhere() const
{
    return buildTypesListDependingOnWhetherCreateClassIsChecked(false);
}
QList<Type*> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::newTypesSeen() const
{
    return m_NewTypesSeen;
}
QList<QString> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::buildTypesListDependingOnWhetherCreateClassIsChecked(bool checked) const
{
    QList<QString> ret;
    const int numTypesDecidingOn = m_NewTypesGridLayout->rowCount();
    for(int i = 0; i < numTypesDecidingOn; ++i)
    {
        if(static_cast<QCheckBox*>(m_NewTypesGridLayout->itemAtPosition(i, NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog_COLUMN_OF_THE_CHECKBOX)->widget())->isChecked() == checked)
        {
            ret.append(static_cast<QLabel*>(m_NewTypesGridLayout->itemAtPosition(i, NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog_COLUMN_OF_THE_LABEL)->widget())->text());
        }
    }
    return ret;
}
void NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::handleDialogAccepted()
{
    //in the project: for each new type, create classes OR note them as defined elsewhere

    m_NewTypesSeen.clear();
    Q_FOREACH(const QString &currentNewDesignEqualsClass, typesDecidedToBeDesignEqualsImplementationClass())
    {
        m_NewTypesSeen << m_DesignEqualsImplementationProject->createNewClass(currentNewDesignEqualsClass);
    }
    Q_FOREACH(const QString &currentNewDefinedElsewhereType, typesDecidedToBeDefinedElsewhere())
    {
        m_NewTypesSeen << m_DesignEqualsImplementationProject->noteDefinedElsewhereType(currentNewDefinedElsewhereType);
    }
}
