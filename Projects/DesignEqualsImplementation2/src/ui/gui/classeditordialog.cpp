#include "classeditordialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>

#include "methodsingleargumentwidget.h"
#include "newtypeseen_createdesignequalsclassfromit_ornoteasdefinedelsewheretype_dialog.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationfunctiondeclarationparser.h"

//modeless yet still cancelable would be best, but for now i'll settle for modal and cancelable. actually fuck that shit, the editor is going to modify the backend object directly for now (fuck the police)
//TODOoptional: "types" can be either internal/designed types (sup) or "Qt/C++ built-ins (or basically ANY type, but similarly simply referred to by string). If you choose a "string" type but later convert it to an internal type, the app could ask you if you want to scan for other uses of that string type to convert them to the internal types as well, saving time/effort/etcS
//TODOoptional: auto-completion when typing in the quick add line edit for already existing types (and the C[++] types) would be nice, but probably pretty difficult. Much easier would be a read-only combo-box containing those types so that by just selecting one in the drop-down, it is pasted into the line edit wherever the cursor is (err does the cursos stay when I click the combo box? i guess append at end is decent enough)
ClassEditorDialog::ClassEditorDialog(DesignEqualsImplementationClass *classToEdit, DesignEqualsImplementationProject *currentProject, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ClassBeingEditted(classToEdit)
    , m_CurrentProject(currentProject)
{
    setWindowTitle(tr("Class Editor"));

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *classNameRow = new QHBoxLayout();
    QLabel *classNameLabel = new QLabel(tr("Class &Name:"));
    QLineEdit *m_ClassNameLineEdit = new QLineEdit(classToEdit->ClassName);
    classNameLabel->setBuddy(m_ClassNameLineEdit);
    classNameRow->addWidget(classNameLabel);
    classNameRow->addWidget(m_ClassNameLineEdit);

#if 0 //TODOoptional
    QHBoxLayout *validStateNameRow = new QHBoxLayout();
    QLabel *validStateNameLabel = new QLabel(tr("Optional 'ready' state name:")); //TODOreq: use for RAII constructor, a better name for the "initialized" signal, and the async initialize slot itself (RAII constructor calls said slot directly)
    QString validStateTooltip("Choose an arbitrary name for when this class/object is valid (open, ready, etc). \"is\" (ex: isReady) will be prepended to it");
    validStateNameLabel->setToolTip(validStateTooltip);
    QLineEdit *m_ValidStateNameLineEdit = new QLineEdit();
    m_ValidStateNameLineEdit->setToolTip(validStateTooltip);
    validStateNameLabel->setBuddy(m_ValidStateNameLineEdit);
    validStateNameRow->addWidget(validStateNameLabel);
    validStateNameRow->addWidget(m_ValidStateNameLineEdit);
#endif

    //Quick add
    QGroupBox *quickMemberAddGroupBox = new QGroupBox(tr("&Quick Add"));
    //TODOoptional: figure out how to set the group box title thing to bold without setting all children of the group box's font to bold also. QFont boldFont = quickMemberAddGroupBox->font();
    //boldFont.setBold(true);
    //quickMemberAddGroupBox->setFont(boldFont);
    QHBoxLayout *quickMemberAddRow = new QHBoxLayout();
    m_QuickMemberAddLineEdit = new QLineEdit();
    m_QuickMemberAddLineEdit->setPlaceholderText(tr("New member signature..."));
    m_QuickMemberAddLineEdit->setToolTip(tr("Signals and Slots _MUST_ have a void return type"));
    QPushButton *quickAddNewPropertyButton = new QPushButton(tr("Property"));
    QPushButton *quickAddNewSignalButton = new QPushButton(tr("Signal"));
    QPushButton *quickAddNewSlotButton = new QPushButton(tr("Slot"));
    quickMemberAddRow->addWidget(m_QuickMemberAddLineEdit);
    quickMemberAddRow->addWidget(quickAddNewPropertyButton);
    quickMemberAddRow->addWidget(quickAddNewSignalButton);
    quickMemberAddRow->addWidget(quickAddNewSlotButton);
    quickMemberAddGroupBox->setLayout(quickMemberAddRow);

    //TODOreq: the old/ugly ways of adding properties/slots/signals should go on their own tabs. the first/front tab should only have a quick add line edit and a visualization of the class itself (practically identical to the class diagram graphics item). The one cool thing the old ugly ways do have that's nice is the argument re-ordering using up/down/delete buttons... but still I need to keep the first/front page clutter free

    //Add property -- merge with hasA private classes members? They are both technically "members" of a class... but of course the hasA private classes members show up in the class lifeline instance chooser dialog (a qobject that is also a q_property could show up there too!)
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
    //TODOreq: since idk how to make clang parse a "type only", I'll just use this old version to compose a function/method/slot signature, then use the quick add logic to parse it again muahahaha. refactor required but whatever. It's also better to have one code path for the signal/slot/property creation stuff instead of two.
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
    //myLayout->addLayout(validStateNameRow);
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


    m_QuickMemberAddLineEdit->setFocus();
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
    //TODOoptional(nvm): this applies to non-quick-add as well: whenever the user specifies a type that is either an "internally designed type" _OR_ a qobject (all my internally designed types are qobjects, so...) and they don't choose for it to be a "pointer" like all my hasAprivateClassesMembers currently are, we should warn them with a modal dialog asking them if they're sure they want it to be a "plain member" ---- FFFFF nvm. the rational that i'd explain to the user was so that proper qobject parenting can be set up so that moveToThread calls work... BUT I JUST REALIZED FOR THE FIRST TIME EVER that even a non-pointer member can still be initialized in it's parent constructor list thingy ( : blah(), x(), etc ) to still use the proper parenting! in that case it would have the same parent twice! once in the OO heirarchy and again in the qobject heirarchy

    //TODOreq: differentiating member pointer owned (instantiated) vs. not owned (not instantiated, passed in somehow) = ??????????????????
}
void ClassEditorDialog::handleQuickAddNewSignalButtonClicked()
{

}
//TODOreq: either manually filter "_Bool" to "bool", or figure out how to get clang to give me the cpp version of a bool :-P
void ClassEditorDialog::handleQuickAddNewSlotButtonClicked()
{
    QList<QString> allKnownTypes;
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_CurrentProject->classes())
    {
        allKnownTypes.append(currentClass->ClassName);
    }
    Q_FOREACH(const QString &currentDefinedElsewhereType, m_CurrentProject->definedElsewhereTypes())
    {
        allKnownTypes.append(currentDefinedElsewhereType);
    }
    DesignEqualsImplementationFunctionDeclarationParser functionDeclarationParser(m_QuickMemberAddLineEdit->text(), allKnownTypes);
    //connect -- awww shit forgot can't emit signals in constructor LoL
    if(functionDeclarationParser.hasUnrecoverableSyntaxError())
    {
        QMessageBox::critical(this, tr("Syntax Error"), tr("There was a syntax error in your slot declaration. Check stderr for the details")); //TODOreq: show the details in-app in a qplaintextedit
        return;
    }

    //first we account for any new param type that is unknown to us
    if(!functionDeclarationParser.newTypesSeenInFunctionDeclaration().isEmpty())
    {
        NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(functionDeclarationParser.newTypesSeenInFunctionDeclaration()); //intuitive naming ;-P
        if(newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog.exec() != QDialog::Accepted)
            return;
        Q_FOREACH(const QString &currentNewDesignEqualsClass, newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog.typesDecidedToBeDesignEqualsImplementationClass())
        {
            m_CurrentProject->createNewClass(currentNewDesignEqualsClass);
        }
        Q_FOREACH(const QString &currentNewDefinedElsewhereType, newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog.typesDecidedToBeDefinedElsewhere())
        {
            m_CurrentProject->noteDefinedElsewhereType(currentNewDefinedElsewhereType);
        }
    }

    //now create the new slot itself
    m_ClassBeingEditted->createwNewSlot(functionDeclarationParser.parsedFunctionName(), functionDeclarationParser.parsedFunctionArguments());

    //TODOreq: add the args to the new slot, i think i need to refactor some though... because the argument types can be either internally designed classes or defined elsewhere types... and additionally they can have modifiers such as "references", "pointers", "consts", etc... that we want to KEEP for the slot declaration. so i guess i'll just stop coding and sit here staring at the blinking cursor comatose until i figure out what to do

    m_QuickMemberAddLineEdit->clear();
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
    //TODOreq: a lot of this method, including calls it makes, is unfinished.. but i am also considering refactoring it tons so...

    if(addSlotFieldsAreSane())
    {
        QList<MethodArgumentTypedef> slotArguments;
        int argCount = m_AddSlotArgumentsVLayout->count();
        for(int i = 0; i < argCount; ++i)
        {
            MethodSingleArgumentWidget *currentArgumentWidget = static_cast<MethodSingleArgumentWidget*>(m_AddSlotArgumentsVLayout->itemAt(i)->widget());
            const QString &argumentType = currentArgumentWidget->argumentType();
            const QString &argumentName = currentArgumentWidget->argumentName();
            if(argumentType.isEmpty() || argumentName.isEmpty())
                continue; //return? would there be more if an empty row is encountered? guess it depends on user. continue is best i guess
            slotArguments.append(qMakePair(argumentType, argumentName));
            //methodToAddArgumentsTo->createNewArgument(argumentType, argumentName);
        }

        DesignEqualsImplementationClassSlot *newSlot = m_ClassBeingEditted->createwNewSlot(m_AddSlotNameLineEdit->text().trimmed(), slotArguments);
        //addAllArgsInLayoutToMethod(newSlot, m_AddSlotArgumentsVLayout);

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
