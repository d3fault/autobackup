#include "classeditordialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>


#include "methodsingleargumentwidget.h"
#include "../../designequalsimplementationclass.h"

//modeless yet still cancelable would be best, but for now i'll settle for modal and cancelable. actually fuck that shit, the editor is going to modify the backend object directly for now (fuck the police)
//TODOoptional: "types" can be either internal/designed types (sup) or "Qt/C++ built-ins (or basically ANY type, but similarly simply referred to by string). If you choose a "string" type but later convert it to an internal type, the app could ask you if you want to scan for other uses of that string type to convert them to the internal types as well, saving time/effort/etcS
ClassEditorDialog::ClassEditorDialog(DesignEqualsImplementationClass *classToEdit, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ClassBeingEditted(classToEdit)
{
    setWindowTitle(tr("Class Editor"));

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *classNameRow = new QHBoxLayout();
    QLabel *classNameLabel = new QLabel(tr("Class &Name:"));
    QLineEdit *m_ClassNameLineEdit = new QLineEdit(classToEdit->ClassName);
    classNameLabel->setBuddy(m_ClassNameLineEdit);
    classNameRow->addWidget(classNameLabel);
    classNameRow->addWidget(m_ClassNameLineEdit);

    QHBoxLayout *validStateNameRow = new QHBoxLayout();
    QLabel *validStateNameLabel = new QLabel(tr("Optional state name: ")); //TODOreq: use for RAII constructor, a better name for the "initialized" signal, and the async initialize slot itself (RAII constructor calls said slot directly)
    QString validStateTooltip("Choose an arbitrary name for when this class/object is valid (open, ready, etc). \"is\" (ex: isReady) will be prepended to it:");
    validStateNameLabel->setToolTip(validStateTooltip);
    QLineEdit *m_ValidStateNameLineEdit = new QLineEdit();
    m_ValidStateNameLineEdit->setToolTip(validStateTooltip);
    validStateNameLabel->setBuddy(m_ValidStateNameLineEdit);
    validStateNameRow->addWidget(validStateNameLabel);
    validStateNameRow->addWidget(m_ValidStateNameLineEdit);

    //Quick add
    QGroupBox *quickMemberAddGroupBox = new QGroupBox(tr("&Quick Add"));
    QHBoxLayout *quickMemberAddRow = new QHBoxLayout();
    QLineEdit *m_QuickMemberAddLineEdit = new QLineEdit();
    m_QuickMemberAddLineEdit->setPlaceholderText(tr("New member signature..."));
    QPushButton *quickAddNewPropertyButton = new QPushButton(tr("Property"));
    QPushButton *quickAddNewSignalButton = new QPushButton(tr("Signal"));
    QPushButton *quickAddNewSlotButton = new QPushButton(tr("Slot"));
    quickMemberAddRow->addWidget(m_QuickMemberAddLineEdit);
    quickMemberAddRow->addWidget(quickAddNewPropertyButton);
    quickMemberAddRow->addWidget(quickAddNewSignalButton);
    quickMemberAddRow->addWidget(quickAddNewSlotButton);
    quickMemberAddGroupBox->setLayout(quickMemberAddRow);

    //Add property
    QGroupBox *addPropertyGroupBox = new QGroupBox(tr("New Property"));
    QHBoxLayout *addPropertyRow = new QHBoxLayout();
    m_AddPropertyTypeLineEdit = new QLineEdit();
    m_AddPropertyTypeLineEdit->setPlaceholderText(tr("Property Type"));
    m_AddPropertyNameLineEdit = new QLineEdit();
    m_AddPropertyNameLineEdit->setPlaceholderText(tr("Property Name"));
    m_AddPropertyReadOnlyCheckbox = new QCheckBox(tr("Read-Only"));
    m_AddPropertyNotifiesOnChangeCheckbox = new QCheckBox(tr("Notifies On Change")); //TODOreq: not sure notifies on change is applicable when read-only, so perhaps uncheck + gray it out when read only. However I am not sure about this and it makes sense that a read-only property can be changed internally by the class and still have a changed notification. I think said notification would need to be generated by Qt in that case, seeing as there is no "setBlah" method to emit the signal otherwise...
    m_AddPropertyNotifiesOnChangeCheckbox->setChecked(true);
    QPushButton *addPropertyButton = new QPushButton(tr("Add Property")); //existing classes (TODOoptional: QCombobox listing them), C++ built-in types, etc
    addPropertyButton->setToolTip(tr("Add Property of Existing Type to Class"));
    QPushButton *addPropertyAndCreateClassButton = new QPushButton(tr("Create Type + Add Property")); //TODOreq: "On the fly" class creation, JIT, whatever. Ideally we'd be able to detect whether or not the class needs to be created and to do it automagically (somewhere checking against a list of qt types, c++ types, and designed types (ideally ideally, any type within "scope" would also be checked, but that's fucking hard as fuck))
    addPropertyAndCreateClassButton->setToolTip(tr("Create Property Type + Add Property to Class"));
    addPropertyRow->addWidget(m_AddPropertyTypeLineEdit);
    addPropertyRow->addWidget(m_AddPropertyNameLineEdit);
    addPropertyRow->addWidget(m_AddPropertyReadOnlyCheckbox);
    addPropertyRow->addWidget(m_AddPropertyNotifiesOnChangeCheckbox);
    addPropertyRow->addWidget(addPropertyButton);
    addPropertyRow->addWidget(addPropertyAndCreateClassButton); //TODOreq
    addPropertyGroupBox->setLayout(addPropertyRow);

    //Add slot -- TODOoptional: perhaps make this it's own [inline/embedded-here] widget so that I can re-use it in the signal/slot message editor [for slot creation on the fly]
    //***TODOreq***: 'tab' when in the arg name field adds a new arg and switches focus to the next arg type field (tab in type field simply moves to name field). Enter in arg name field adds the slot to the class (clears the form). Enter in slot name field also adds the slot to the class (such as when there are no args). Both of these abilities are explained in tooltips for "Add Arg" and "Add Slot" buttons respectively. Additionally, empty arg type/name fields (accidental tab press) are safely ignored (but if only one or the other is empty, we give the user an error). If the argument is not the very last argument, then pressing tab in arg name field does not add a new argument, but just functions normally and changes focus to the next arg type field
    //TODOreq: perhaps when add slot is pressed, the types are checked against internal/designed types and associated with them (otherwise, just string type). However this fucking fails when considering all the various ways to refer to a type in C++ (ptr, ref, const, etc)
    QGroupBox *addSlotGroupBox = new QGroupBox(tr("New Slot"));
    QHBoxLayout *addSlotRow = new QHBoxLayout();
    m_AddSlotNameLineEdit = new QLineEdit();
    m_AddSlotNameLineEdit->setPlaceholderText(tr("Slot Name"));
    m_AddSlotNameLineEdit->setToolTip(tr("Slot Name"));
    //QGroupBox *addSlotArgsGroupBox = new QGroupBox(tr("Arguments"));
    m_AddSlotArgumentsVLayout = new QVBoxLayout();
    addArgToSlotBeingCreated();
    QPushButton *addSlotAddArgButton = new QPushButton(tr("+"));
    QPushButton *addSlotButton = new QPushButton(tr("Add Slot")); //TODOreq: arg types might need to be created on the fly, but might be Qt/C++ builtins instead. Each arg needs to be handled differently
    addSlotRow->addWidget(m_AddSlotNameLineEdit);

    //addSlotArgsGroupBox->setLayout(m_AddSlotArgumentsVLayout);
    addSlotRow->addLayout(m_AddSlotArgumentsVLayout);
    addSlotRow->addWidget(addSlotAddArgButton);
    addSlotRow->addWidget(addSlotButton);
    addSlotGroupBox->setLayout(addSlotRow);

    //Done button
    QPushButton *doneButton = new QPushButton(tr("Done"));

    myLayout->addLayout(classNameRow);
    myLayout->addLayout(validStateNameRow);
    myLayout->addWidget(quickMemberAddGroupBox);
    myLayout->addWidget(addPropertyGroupBox);
    myLayout->addWidget(addSlotGroupBox);
    myLayout->addWidget(doneButton);

    setLayout(myLayout);

    connect(quickAddNewPropertyButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewPropertyButtonClicked()));
    connect(quickAddNewSignalButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewSignalButtonClicked()));
    connect(quickAddNewSlotButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewSlotButtonClicked()));

    //Property
    connect(addPropertyButton, SIGNAL(clicked()), this, SLOT(handleAddPropertyButtonClicked()));

    //Slot
    connect(m_AddSlotNameLineEdit, SIGNAL(returnPressed()), this, SLOT(handleAddSlotButtonClicked()));
    connect(addSlotAddArgButton, SIGNAL(clicked()), this, SLOT(handleAddSlotAddArgButtonClicked()));
    connect(addSlotButton, SIGNAL(clicked()), this, SLOT(handleAddSlotButtonClicked()));

    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept())); //or reject or done or close, no matter. might make this modeless. TODOreq: process any fields that may have been typed into. Like basically "done" could be a shortcut for "add slot" (+ done) or "add property" (+ done) etc. HOWEVER this may not be desireable so maybe we should prompt the user. Additionally, which of the unfinished edits to we process? All of them? Perhaps just a warning with cancel/ok whenever un-committed edits are detected? Perhaps even a checkbox for to do the speedy "add + done" thing to save clicks, opt in by default but saving whether or not it's checked

    //reactor pattern, gui responding to our own edits <3
    connect(classToEdit, SIGNAL(propertyAdded(DesignEqualsImplementationClassProperty*)), this, SLOT(handlePropertyAdded(DesignEqualsImplementationClassProperty*)));
    connect(classToEdit, SIGNAL(slotAdded(DesignEqualsImplementationClassSlot*)), this, SLOT(handleSlotAdded(DesignEqualsImplementationClassSlot*)));
}
void ClassEditorDialog::addArgToSlotBeingCreated()
{
    MethodSingleArgumentWidget *newArg = new MethodSingleArgumentWidget();

    m_AddSlotArgumentsVLayout->addWidget(newArg);

    connect(newArg, SIGNAL(commitMethodequested()), this, SLOT(handleAddSlotButtonClicked()));
    connect(newArg, SIGNAL(tabPressedInNameLineEdit()), this, SLOT(handleTabPressedInArgNameLineEdit_SoMakeNewArgIfThatWasTheLastOne_OtherwiseManuallyChangeFocusSinceTheTabWasEaten()));
    connect(newArg, SIGNAL(moveArgumentUpRequested(MethodSingleArgumentWidget*)), this, SLOT(handleMoveArgumentUpRequested(MethodSingleArgumentWidget*)));
    connect(newArg, SIGNAL(moveArgumentDownRequested(MethodSingleArgumentWidget*)), this, SLOT(handleMoveArgumentDownRequested(MethodSingleArgumentWidget*)));
    connect(newArg, SIGNAL(deleteArgumentRequested(MethodSingleArgumentWidget*)), this, SLOT(handleDeleteArgumentRequested(MethodSingleArgumentWidget*)));
    newArg->focusOnArgType();
}
void ClassEditorDialog::addAllArgsInLayoutToMethod(IDesignEqualsImplementationMethod *methodToAddArgumentsTo, QLayout *layoutContainingOneArgPerChild)
{
    int argCount = layoutContainingOneArgPerChild->count();
    for(int i = 0; i < argCount; ++i)
    {
        MethodSingleArgumentWidget *currentArgumentWidget = static_cast<MethodSingleArgumentWidget*>(layoutContainingOneArgPerChild->itemAt(i)->widget());
        const QString &argumentType = currentArgumentWidget->argumentType();
        const QString &argumentName = currentArgumentWidget->argumentName();
        if(argumentType.isEmpty() || argumentName.isEmpty())
            continue; //return? would there be more if an empty row is encountered? guess it depends on user. continue is best i guess
        methodToAddArgumentsTo->createNewArgument(argumentType, argumentName);
    }
}
bool ClassEditorDialog::addPropertyFieldsAreSane()
{
    if(m_AddPropertyTypeLineEdit->text().trimmed().isEmpty())
    {
        emit e(tr("Property Type must not be empty"));
        return false;
    }
    if(m_AddPropertyNameLineEdit->text().trimmed().isEmpty())
    {
        emit e(tr("Property Name must not be empty"));
        return false;
    }

    //TODOreq: real sanitization, such as invalid characters etc. however the compiler does this already so i mean maybe i should just not give a shit <3

    return true;
}
bool ClassEditorDialog::addSlotFieldsAreSane()
{
    if(m_AddSlotNameLineEdit->text().trimmed().isEmpty())
    {
        emit e(tr("Slot Name must not be empty"));
        return false;
    }

    return true; //TODOreq: more (the args, for example), better (invalid characters, see addPropertyFieldsAreSane)
}
void ClassEditorDialog::handleQuickAddNewPropertyButtonClicked()
{

}
void ClassEditorDialog::handleQuickAddNewSignalButtonClicked()
{

}
void ClassEditorDialog::handleQuickAddNewSlotButtonClicked()
{
    //TODOoptional: parse the method definition. i looked into using libclang, but was unable to get any of the examples working :-/. i could write a primitive basic parser, but also feel it might not be worth the effort since i'm definitely reinventing a wheel (and i know it would be limited in use and break easy)
}
void ClassEditorDialog::handleAddPropertyButtonClicked()
{
    if(addPropertyFieldsAreSane())
    {
        //signal, slot invoke, or direct method call? blah, semantecs at this point...
        m_ClassBeingEditted->createNewProperty(m_AddPropertyTypeLineEdit->text().trimmed(), m_AddPropertyNameLineEdit->text().trimmed(), m_AddPropertyReadOnlyCheckbox->isChecked(), m_AddPropertyNotifiesOnChangeCheckbox->isChecked());
        m_AddPropertyTypeLineEdit->clear();
        m_AddPropertyNameLineEdit->clear();
    }
}
void ClassEditorDialog::handleAddSlotAddArgButtonClicked()
{
    addArgToSlotBeingCreated();
}
void ClassEditorDialog::handleTabPressedInArgNameLineEdit_SoMakeNewArgIfThatWasTheLastOne_OtherwiseManuallyChangeFocusSinceTheTabWasEaten() //intuitive naming ;-)
{
    addArgToSlotBeingCreated(); //TODOreq only if last!
}
void ClassEditorDialog::handleMoveArgumentUpRequested(MethodSingleArgumentWidget *argumentRequestingToBeMovedUp)
{
    //TODOreq
}
void ClassEditorDialog::handleMoveArgumentDownRequested(MethodSingleArgumentWidget *argumentRequestingToBeMovedDown)
{
    //TODOreq
}
void ClassEditorDialog::handleDeleteArgumentRequested(MethodSingleArgumentWidget *argumentRequestingToBeDeleted)
{
    //TODOreq
}
void ClassEditorDialog::handleAddSlotButtonClicked()
{
    if(addSlotFieldsAreSane())
    {
        DesignEqualsImplementationClassSlot *newSlot = m_ClassBeingEditted->createwNewSlot(m_AddSlotNameLineEdit->text().trimmed()); //TODOoptional: reactor pattern fail? the args haven't been added, but the slot will already be emitted as added. Perhaps createNewSlot just needs an overload accepting a list of args? For now, fuck it.. just some flickering n shit ;-P (actually the args don't show up in graphics scene at all (but they do show up in source gen ofc), but flickering would still happen if i solved it halfway). Perhaps I should just new it myself and add it to class later after all args have been added
        addAllArgsInLayoutToMethod(newSlot, m_AddSlotArgumentsVLayout);

        m_AddSlotNameLineEdit->clear();
        //TODOreq: reset the args layout
    }
}
void ClassEditorDialog::handlePropertyAdded(DesignEqualsImplementationClassProperty *propertyAdded)
{
    //TODOreq: visualize added property in dialog
}
void ClassEditorDialog::handleSlotAdded(DesignEqualsImplementationClassSlot *slotAdded)
{
    //TODOreq: visualize added slot in dialog
}