#include "newtypeseen_createdesignequalsclassfromit_ornoteasdefinedelsewheretype_dialog.h"

#include <QVBoxLayout>
#include <QSignalMapper>
#include <QGridLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QPushButton>

#include "../../designequalsimplementationproject.h"

int NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::QObjectDerivedRadioButtonId = 0;
int NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::ImplicitlySharedRadioButtonId = 1;
int NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::DefinedElsewhereRadioButtonId = 2;

//TO DOnereq: convert from a single checkbox into 3x radio buttons (Class, Implicitly Shared Data Type, Defined Elsewhere). Default to Class. There needs to be a (bool/enum) constructor arg that makes Class column gray'd out or missing (in which case we default to Data), because Data can't have Class children. There needs to be an "all to this" button for each column
NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(const QList<QString> listOfTypesToDecideOn, DesignEqualsImplementationProject *designEqualsImplementationProject, TypesCanBeQObjectDerivedEnum typesCanBeQObjectDerived, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_DesignEqualsImplementationProject(designEqualsImplementationProject)
    , m_TypesCanBeQObjectDerived(typesCanBeQObjectDerived)
{
    //TODOoptional: scroll bars in case there are a fucking shit load of new types (afaik, c[++] doesn't set a limit on num params)

    //too long and unseen, maybe set minwidth for it? setWindowTitle(tr("For each of these never before seen types..."));
    setWindowTitle(tr("New types..."));

    QVBoxLayout *myLayout = new QVBoxLayout();

    QGridLayout *gridLayout = new QGridLayout();
    int row = 0;

    //headers + "select all" buttons
    int col = 0;
    if(m_TypesCanBeQObjectDerived == TypesCanBeQObjectDerived)
        gridLayout->addWidget(new QLabel(tr("QObject Derived Class")), row, ++col);
    gridLayout->addWidget(new QLabel(tr("Implicitly Shared Data Type")), row, ++col);
    gridLayout->addWidget(new QLabel(tr("Defined Elsewhere")), row, ++col);
    col = 0; //\r
    ++row; //\n

    QSignalMapper *selectAllSignalMapper = new QSignalMapper(this);

    if(m_TypesCanBeQObjectDerived == TypesCanBeQObjectDerived)
    {
        QPushButton *allQObjectDerivedPushButton = new QPushButton(tr("Select All"));
        gridLayout->addWidget(allQObjectDerivedPushButton, row, ++col);
        selectAllSignalMapper->setMapping(allQObjectDerivedPushButton, QObjectDerivedRadioButtonId);
        connect(allQObjectDerivedPushButton, SIGNAL(clicked()), selectAllSignalMapper, SLOT(map()));
    }

    QPushButton *allImplicitlySharedPushButton = new QPushButton(tr("Select All"));
    gridLayout->addWidget(allImplicitlySharedPushButton, row, ++col);
    selectAllSignalMapper->setMapping(allImplicitlySharedPushButton, ImplicitlySharedRadioButtonId);
    connect(allImplicitlySharedPushButton, SIGNAL(clicked()), selectAllSignalMapper, SLOT(map()));

    QPushButton *allDefinedElsewherePushButton = new QPushButton(tr("Select All"));
    gridLayout->addWidget(allDefinedElsewherePushButton, row, ++col);
    selectAllSignalMapper->setMapping(allDefinedElsewherePushButton, DefinedElsewhereRadioButtonId);
    connect(allDefinedElsewherePushButton, SIGNAL(clicked()), selectAllSignalMapper, SLOT(map()));

    connect(selectAllSignalMapper, SIGNAL(mapped(int)), this, SLOT(selectAllByButtonId(int)));


    Q_FOREACH(const QString &currentTypeToDecideOn, listOfTypesToDecideOn)
    {
        col = -1;
        gridLayout->addWidget(new QLabel(currentTypeToDecideOn), row, ++col);
        QButtonGroup *radioButtonGroup = new QButtonGroup(this);
        QRadioButton *implicitlySharedDataTypeRadioButton = new QRadioButton();
        if(typesCanBeQObjectDerived == TypesCanBeQObjectDerived)
        {
            QRadioButton *qobjectAkaClassRadioButton = new QRadioButton();
            radioButtonGroup->addButton(qobjectAkaClassRadioButton, QObjectDerivedRadioButtonId);
            gridLayout->addWidget(qobjectAkaClassRadioButton, row, ++col);
            qobjectAkaClassRadioButton->setChecked(true);
        }
        else
            implicitlySharedDataTypeRadioButton->setChecked(true);

        radioButtonGroup->addButton(implicitlySharedDataTypeRadioButton, ImplicitlySharedRadioButtonId);
        gridLayout->addWidget(implicitlySharedDataTypeRadioButton, row, ++col);

        QRadioButton *definedElsewhereRadioButton = new QRadioButton();
        radioButtonGroup->addButton(definedElsewhereRadioButton, DefinedElsewhereRadioButtonId);
        gridLayout->addWidget(definedElsewhereRadioButton, row, ++col);

        m_TypesAndTheirDecisionRadioGroups << radioButtonGroup;
        ++row;
    }
    //internallyDesignedOrExternallyDefinedCheckbox->setToolTip(tr("If you un-check this, the new type will still be usable, but it will be noted as being 'defined elsewhere' (such as in a 3rd party library). You can even specify the necessary #includes that will be placed automatically whenever the externally defined type is referenced in the project from the blah blah blah dialog")); //TODOreq: make the blah blah blah dialog and explain properly how to get there

    QHBoxLayout *okCancelRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButton->setDefault(true);

    okCancelRow->addWidget(cancelButton);
    okCancelRow->addWidget(okButton);

    myLayout->addLayout(gridLayout);
    myLayout->addLayout(okCancelRow);

    setLayout(myLayout);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this, SIGNAL(accepted()), this, SLOT(handleDialogAccepted()));
}
QList<Type*> NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::newTypesSeen() const
{
    return m_NewTypesSeen;
}
void NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::selectAllByButtonId(int buttonId)
{
    Q_FOREACH(TypesAndTheirDecisionRadioGroups currentTypeAndItsDecisionRadioGroup, m_TypesAndTheirDecisionRadioGroups)
        currentTypeAndItsDecisionRadioGroup.second->button(buttonId)->setChecked(true);
}
void NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::handleDialogAccepted()
{
    //in the project: for each new type, create qobject derived class OR implicitly shared data type OR note them as defined elsewhere
    m_NewTypesSeen.clear();
    Q_FOREACH(TypesAndTheirDecisionRadioGroups currentTypeAndItsDecisionRadioGroup, m_TypesAndTheirDecisionRadioGroups)
    {
        if(m_TypesCanBeQObjectDerived == TypesCanBeQObjectDerived && currentTypeAndItsDecisionRadioGroup.second->button(QObjectDerivedRadioButtonId)->isChecked())
        {
            m_NewTypesSeen << m_DesignEqualsImplementationProject->createNewClass(currentTypeAndItsDecisionRadioGroup.first);
            continue;
        }
        if(currentTypeAndItsDecisionRadioGroup.second->button(ImplicitlySharedRadioButtonId)->isChecked())
        {
            m_NewTypesSeen << m_DesignEqualsImplementationProject->createNewImplicitlySharedDataType(currentTypeAndItsDecisionRadioGroup.first);
            continue;
        }
        if(currentTypeAndItsDecisionRadioGroup.second->button(DefinedElsewhereRadioButtonId)->isChecked())
        {
            m_NewTypesSeen << m_DesignEqualsImplementationProject->noteDefinedElsewhereType(currentTypeAndItsDecisionRadioGroup.first);
            continue;
        }
        qFatal("somehow, no radio button was checked. bug");
    }
}
