#include "classeditordialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>

#include "methodsingleargumentwidget.h"
#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationlenientpropertydeclarationparser.h"
#include "../../designequalsimplementationprojectgenerator.h"

//modeless yet still cancelable would be best, but for now i'll settle for modal and cancelable. actually fuck that shit, the editor is going to modify the backend object directly for now (fuck the police)
//TODOoptional: "types" can be either internal/designed types (sup) or "Qt/C++ built-ins (or basically ANY type, but similarly simply referred to by string). If you choose a "string" type but later convert it to an internal type, the app could ask you if you want to scan for other uses of that string type to convert them to the internal types as well, saving time/effort/etcS
//TODOoptional: auto-completion when typing in the quick add line edit for already existing types (and the C[++] types) would be nice, but probably pretty difficult. Much easier would be a read-only combo-box containing those types so that by just selecting one in the drop-down, it is pasted into the line edit wherever the cursor is (err does the cursos stay when I click the combo box? i guess append at end is decent enough)
//TODOreq: hasAprivateClassesMembers adding... which up till now has only ever been done programmatically :-/. highly considering merging it with Q_PROPERTY and just calling it/them "member"... and the way to make it a Q_PROPERTY would become a non-default action of a QToolButton. default action should be private member
//TODOoptional: An always accessible (read-only? methinks yes but idfk) combo box throughout entire app that copies the type you select to your clipboard. It's a hacky/easy solution to the auto-completion problem (auto-completion is easy if the line edit is only expecting a TYPE, but auto-completion is [probably? (libclang idfk)] a bitch when a line edit can contain a member declaration (property, signal, or slot). Differentiating from the "type", "qualifiers" and "name" parts would be a bitch... but meh actually maybe not but idk if it's worth diving into research mode in libclang [again(although admittedly last time was totally fucking worth it)]
//TODOreq: class renames aren't reflected in "available items" in use case view
//TODOreq: class renames aren't reflected in class diagram. they are shown when you rename and then add a signal/slot/property though
//TODOreq: if they 'delete' the class name, we probably shouldn't change the class name to an empty string? might segfault, might be fine actually
//TODOreq: any time the quick signal/slot/property buttons are clicked, focus should be returned to the quick add line edit. the focus already stays when one of the alt+key shortcuts are used, but not when the mouse is used to click the buttons they are shortcuts to
//TODOoptional: the class overview could be in html and use anchor/hrefs for each piece, where clicking on that piece brings you to the correct tab (property/signal/slot) and selects the right property/signal/slot in the list, and additionally puts the cursor focus (and maybe selects all) in the line edit used to edit that piece that was clicked. this could additionally be done in class diagram view at all times, but then click-n-drag moving around the objects might become a pain (though i doubt it (click-n-drag vs. click) and like this feature very much in BOTH places!)
//TODOreq: member name collissions (off the top of my head, a signal and slot can't both be named "x")
//TODOoptional: if something is typed in quick add line edit[s] and done is pressed, we assume that either property/signal/slot was meant to be pressed and show an "are you sure?" dialog so they have to confirm the erasal of their quick add line edit typings
//TODOoptional: when there's a signal, we can have multiple slots. they may all be created IN the signal slot editor. as a special case, pressing enter in the "quick add new slot" line edit should not accept the dialog (like usual) but should instead add new line edit for the next slot. Pressing enter when the line edit is empty (so pressing enter twice in a row) accepts the dialog like normal
//TODOreq: class editor -> slots tab... to allow them to do the non-quick-add, or re-order/re-name/etc, which after dialog is 'done' means we need to refresh ourself (just like after instance chooser is accepted)
//TODOreq: s/Class/Type
ClassEditorDialog::ClassEditorDialog(DesignEqualsImplementationType *typeToEdit, DesignEqualsImplementationProject *currentProject, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_TypeBeingEditted(typeToEdit)
    , m_TypeIsQObjectDerived(false)
    , m_CurrentProject(currentProject)
{
    if(qobject_cast<DefinedElsewhereType*>(typeToEdit))
    {
        QMessageBox::warning(this, tr("Warning!"), tr("Attempted to edit a 'Defined Elsewhere Type. This is a programming bug and should never happen."));
        accept();
        return;
    }
    if(DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(typeToEdit))
    {
        m_TypeIsQObjectDerived = true;

        connect(typeAsClass, SIGNAL(signalAdded(DesignEqualsImplementationClassSignal*)), this, SLOT(updateClassOverviewLabel()));
        connect(typeAsClass, SIGNAL(slotAdded(DesignEqualsImplementationClassSlot*)), this, SLOT(updateClassOverviewLabel()));
    }

    setWindowTitle(tr("Class Editor"));

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *classNameRow = new QHBoxLayout();
    QLabel *classNameLabel = new QLabel(tr("Class &Name:"));
    QLineEdit *classNameLineEdit = new QLineEdit(typeToEdit->Name);
    QPushButton *editCppButton = new QPushButton(tr("Edit C++"));
    classNameLabel->setBuddy(classNameLineEdit);
    classNameRow->addWidget(classNameLabel);
    classNameRow->addWidget(classNameLineEdit);
    classNameRow->addWidget(editCppButton);

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

    QTabWidget *classEditorTabWidget = new QTabWidget();
    classEditorTabWidget->addTab(createMainClassEditorTab(), tr("Class Overview"));
    //classEditorTabWidget->addTab(createPropertiesClassEditorTab(), tr("Members"));
    //classEditorTabWidget->addTab(createSignalsClassEditorTab(), tr("Signals"));
    //classEditorTabWidget->addTab(createSlotsClassEditorTab(), tr("Slots"));

    //TODOreq: the old/ugly ways of adding properties/slots/signals should go on their own tabs. the first/front tab should only have a quick add line edit and a visualization of the class itself (practically identical to the class diagram graphics item). The one cool thing the old ugly ways do have that's nice is the argument re-ordering using up/down/delete buttons... but still I need to keep the first/front page clutter free

    //Done button
    QPushButton *doneButton = new QPushButton(tr("Done"));

    myLayout->addLayout(classNameRow);
    //myLayout->addLayout(validStateNameRow);
    myLayout->addWidget(classEditorTabWidget);
    myLayout->addWidget(doneButton);

    setLayout(myLayout);

    connect(classNameLineEdit, SIGNAL(textChanged(QString)), m_TypeBeingEditted, SLOT(setClassName(QString))); //TODOreq: sanitize: no spaces etc. a libclang syntax check would be best, but also maybe overkill
    connect(editCppButton, SIGNAL(clicked()), this, SLOT(handleEditCppButtonClicked()));

    connect(doneButton, SIGNAL(clicked()), this, SLOT(handleDoneButtonClicked()));
    //connect(doneButton, SIGNAL(clicked()), this, SLOT(accept())); //or reject or done or close, no matter. might make this modeless. TODOreq: process any fields that may have been typed into. Like basically "done" could be a shortcut for "add slot" (+ done) or "add property" (+ done) etc. HOWEVER this may not be desireable so maybe we should prompt the user. Additionally, which of the unfinished edits to we process? All of them? Perhaps just a warning with cancel/ok whenever un-committed edits are detected? Perhaps even a checkbox for to do the speedy "add + done" thing to save clicks, opt in by default but saving whether or not it's checked

    //reactor pattern, gui responding to our own edits <3
    connect(typeToEdit, SIGNAL(nonFunctionMemberAdded(NonFunctionMember*)), this, SLOT(updateClassOverviewLabel()));
#if 0
    connect(classToEdit, SIGNAL(nonFunctionMemberAdded(NonFunctionMember*)), this, SLOT(handleNonFunctionMemberAdded(NonFunctionMember*)));
    connect(classToEdit, SIGNAL(signalAdded(DesignEqualsImplementationClassSignal*)), this, SLOT(handleSignalAdded(DesignEqualsImplementationClassSignal*)));
    connect(classToEdit, SIGNAL(slotAdded(DesignEqualsImplementationClassSlot*)), this, SLOT(handleSlotAdded(DesignEqualsImplementationClassSlot*)));
#endif

    connect(this, SIGNAL(editCppModeRequested(DesignEqualsImplementationType*)), currentProject, SLOT(handleEditCppModeRequested(DesignEqualsImplementationType*)));

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
QWidget *ClassEditorDialog:: createMainClassEditorTab()
{
    QWidget *mainClassEditorTabWidget = new QWidget();
    QVBoxLayout *mainClassEditorTabLayout = new QVBoxLayout();

    m_QuickMemberAddLineEdit = new QLineEdit(); //TODOoptional: decide what to do when return is pressed lol. slot? if that's the case then slot should become the left-most of the three and bolded
    m_QuickMemberAddLineEdit->setPlaceholderText(tr("New member signature..."));

    if(m_TypeIsQObjectDerived)
        mainClassEditorTabLayout->addWidget(createQObjectDerivedMainClassEditorBodyWidget());
    else
        mainClassEditorTabLayout->addWidget(createNonQObjectDerivedMainClassEditorBodyWidget());

    //Class Overview (HTML)
    m_ClassOverviewLabel = new QLabel();
    m_ClassOverviewLabel->setTextFormat(Qt::RichText);
    updateClassOverviewLabel();
    mainClassEditorTabLayout->addWidget(m_ClassOverviewLabel);

    mainClassEditorTabWidget->setLayout(mainClassEditorTabLayout);

    return mainClassEditorTabWidget;
}
QWidget *ClassEditorDialog::createQObjectDerivedMainClassEditorBodyWidget()
{
    QGroupBox *ret = new QGroupBox(tr("&Add Member"));
    //TODOoptional: figure out how to set the group box title thing to bold without setting all children of the group box's font to bold also. QFont boldFont = quickMemberAddGroupBox->font();
    //boldFont.setBold(true);
    //quickMemberAddGroupBox->setFont(boldFont);
    QHBoxLayout *addMemberRow = new QHBoxLayout();
    m_QuickMemberAddLineEdit->setToolTip(tr("Signals and Slots _MUST_ have a void return type. You are not required to type the [void] return type, however.\nJust start typing the signal/slot NAME and you're golden. The semi-colon at the end is optional as well.\n\nExample signal or slot (signal and slot signatures use the same syntax):\nnewSlot(int x, const SomeNewTypeThatWillBeCreatedOnTheFly &y);\n\nExample property:\nint MyProperty;\nint MyProperty = 0;\nint MyProperty(0);\n\nSome shortcuts when typing:\n\tAlt+P = Property\n\tAlt+I =  Signal\n\tAlt+L = Slot"));
    //TODOreq: visibility radio for members+slots, and I'm starting to think maybe Q_PROPERTY could ~sorta~ be a 'visibility' (in the enum too), but obviously n/a to slots. OT: I don't think I've ever used a protected slot:, but I would think it's possible/useful-in-some-places. for the TIME BEING (cleopatra), I'm fine with all slots being public and visibility only applying to NonFunctionMembers
    QPushButton *quickAddNewPropertyButton = new QPushButton(tr("&Property"));
    QPushButton *quickAddNewSignalButton = new QPushButton(tr("S&ignal"));
    QPushButton *quickAddNewSlotButton = new QPushButton(tr("S&lot"));
    addMemberRow->addWidget(m_QuickMemberAddLineEdit);
    addMemberRow->addWidget(quickAddNewPropertyButton);
    addMemberRow->addWidget(quickAddNewSignalButton);
    addMemberRow->addWidget(quickAddNewSlotButton);
    ret->setLayout(addMemberRow);

    connect(quickAddNewPropertyButton, SIGNAL(clicked()), this, SLOT(addNewNonFunctionMember()));
    connect(quickAddNewSignalButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewSignalButtonClicked()));
    connect(quickAddNewSlotButton, SIGNAL(clicked()), this, SLOT(handleQuickAddNewSlotButtonClicked()));

    return ret;
}
QWidget *ClassEditorDialog::createNonQObjectDerivedMainClassEditorBodyWidget()
{
    QGroupBox *ret = new QGroupBox(tr("&Add Member"));
    QHBoxLayout *addMemberRow = new QHBoxLayout();
    m_QuickMemberAddLineEdit->setToolTip(tr("Example member:\nint MyMember;\nint MyMember = 0;\nint MyMember(0);\n\nThe semi-colon is optional"));
    QPushButton *addMemberButton = new QPushButton(tr("Add Member"));
    addMemberRow->addWidget(m_QuickMemberAddLineEdit);
    addMemberRow->addWidget(addMemberButton);
    ret->setLayout(addMemberRow);
    connect(addMemberButton, SIGNAL(clicked()), this, SLOT(addNewNonFunctionMember()));
    connect(m_QuickMemberAddLineEdit, SIGNAL(returnPressed()), addMemberButton, SLOT(click()));
    return ret;
}
QWidget *ClassEditorDialog::createPropertiesClassEditorTab()
{
    QWidget *propertiesClassEditorTabWidget = new QWidget();
    QVBoxLayout *propertiesClassEditorTabLayout = new QVBoxLayout();

    //Add property -- merge with hasA private classes members? They are both technically "members" of a class... but of course the hasA private classes members show up in the class lifeline instance chooser dialog (a qobject that is also a q_property could show up there too!)
    QGroupBox *addPropertyGroupBox = new QGroupBox(tr("New Member"));
    QHBoxLayout *addPropertyRow = new QHBoxLayout();
    m_AddPropertyTypeLineEdit = new QLineEdit();
    m_AddPropertyTypeLineEdit->setPlaceholderText(tr("Member Type"));
    m_AddPropertyNameLineEdit = new QLineEdit();
    m_AddPropertyNameLineEdit->setPlaceholderText(tr("Member Name"));
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

    propertiesClassEditorTabLayout->addWidget(addPropertyGroupBox);
    propertiesClassEditorTabWidget->setLayout(propertiesClassEditorTabLayout);

    //Property
    connect(addPropertyButton, SIGNAL(clicked()), this, SLOT(handleAddPropertyButtonClicked()));

    return propertiesClassEditorTabWidget;
}
QWidget *ClassEditorDialog::createSignalsClassEditorTab()
{
    return new QWidget(); //TODOreq, more or less copy/past job from slots
}
QWidget *ClassEditorDialog::createSlotsClassEditorTab()
{
    QWidget *slotsClassEditorTabWidget = new QWidget();
    QVBoxLayout *slotsClassEditorTabLayout = new QVBoxLayout();

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

    slotsClassEditorTabLayout->addWidget(addSlotGroupBox);
    slotsClassEditorTabWidget->setLayout(slotsClassEditorTabLayout);

    //Slot
    connect(m_AddSlotNameLineEdit, SIGNAL(returnPressed()), this, SLOT(handleAddSlotButtonClicked()));
    connect(addSlotAddArgButton, SIGNAL(clicked()), this, SLOT(handleAddSlotAddArgButtonClicked()));
    connect(addSlotButton, SIGNAL(clicked()), this, SLOT(handleAddSlotButtonClicked()));

    return slotsClassEditorTabWidget;
}
QString ClassEditorDialog::classDetailsAsHtmlString()
{
    QString classContentsString;

    //NonFunctionMembers
    if(m_TypeBeingEditted->nonFunctionMembers().size() > 0)
    {
        classContentsString.append("<br /><b>Non-Function Members</b>");
    }
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, m_TypeBeingEditted->nonFunctionMembers())
    {
        classContentsString.append("<br />" + currentNonFunctionMember->preferredTextualRepresentationOfTypeAndVariableTogether()); //TODOreq: try to cast to property, display it differently
    }
    if(m_TypeBeingEditted->nonFunctionMembers().size() > 0)
    {
        classContentsString.append("<br />");
    }

    if(DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(m_TypeBeingEditted))
    {
        //Private Methods
        if(typeAsClass->PrivateMethods.size() > 0)
        {
            classContentsString.append("<br /><b>Private Methods<b>");
        }
        Q_FOREACH(DesignEqualsImplementationClassPrivateMethod *currentPrivateMethod, typeAsClass->PrivateMethods)
        {
            classContentsString.append("<br />" + currentPrivateMethod->Name); //TODOreq: methodSignature
        }
        if(typeAsClass->PrivateMethods.size() > 0)
        {
            classContentsString.append("<br />");
        }

        //Signals
        if(typeAsClass->mySignals().size() > 0)
        {
            classContentsString.append("<br /><b>Signals</b>");
        }
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, typeAsClass->mySignals())
        {
            classContentsString.append("<br />" + currentSignal->methodSignatureWithoutReturnType());
        }
        if(typeAsClass->mySignals().size() > 0)
        {
            classContentsString.append("<br />");
        }

        //Slots
        bool noSlotsThatArentTempSlotMagicalNameString = true;
        QString slotTempHtml;
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, typeAsClass->mySlots())
        {
            if(!currentSlot->Name.startsWith(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX))
            {
                slotTempHtml.append("<br />" + currentSlot->methodSignatureWithoutReturnType());
                noSlotsThatArentTempSlotMagicalNameString = false;
            }
            else
            {
                //TODOoptional: we definitely don't want to SHOW the name of the unnamed slots (ugly temp hack string), but maybe indicate how many unnamed slots the class has (but don't say "zero" ofc -- when in the class editor, such unnamed slots would be double-clickable and that takes us to the use case they are in)
            }
        }
        if(!noSlotsThatArentTempSlotMagicalNameString)
        {
            classContentsString.append("<br /><b>Slots</b>");
            classContentsString.append(slotTempHtml);
            classContentsString.append("<br />");
        }
    }

    return classContentsString;
}
void ClassEditorDialog::handleEditCppButtonClicked()
{
    emit editCppModeRequested(m_TypeBeingEditted); //TODOoptional: each slot in the class could also have similar "Edit C++" buttons (the backend logic for this is already implemented)
}
void ClassEditorDialog::addNewNonFunctionMember()
{
    //TODOoptional(nvm): this applies to non-quick-add as well: whenever the user specifies a type that is either an "internally designed type" _OR_ a qobject (all my internally designed types are qobjects, so...) and they don't choose for it to be a "pointer" like all my hasAprivateClassesMembers currently are, we should warn them with a modal dialog asking them if they're sure they want it to be a "plain member" ---- FFFFF nvm. the rational that i'd explain to the user was so that proper qobject parenting can be set up so that moveToThread calls work... BUT I JUST REALIZED FOR THE FIRST TIME EVER that even a non-pointer member can still be initialized in it's parent constructor list thingy ( : blah(), x(), etc ) to still use the proper parenting! in that case it would have the same parent twice! once in the OO heirarchy and again in the qobject heirarchy

    //TODOreq: differentiating member pointer owned (instantiated) vs. not owned (not instantiated, passed in somehow) = ??????????????????


    DesignEqualsImplementationLenientPropertyDeclarationParser nonFunctionMemberParser(m_QuickMemberAddLineEdit->text(), m_CurrentProject->allKnownTypesNames());
    if(nonFunctionMemberParser.hasError())
    {
        QMessageBox::critical(this, tr("Error"), nonFunctionMemberParser.mostRecentError()); //TODOreq: show the details in-app in a qplaintextedit
        return;
    }

    //the property might be a new type, so ask the user how to handle it
    if(!nonFunctionMemberParser.newTypesSeenInPropertyDeclaration().isEmpty())
    {
        NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(nonFunctionMemberParser.newTypesSeenInPropertyDeclaration(), m_CurrentProject, (m_TypeIsQObjectDerived ? NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::TypesCanBeQObjectDerived : NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::TypesCannotBeQObjectDerived), this);
        if(newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog.exec() != QDialog::Accepted)
            return;
    }

    bool makeItAQ_PROPERTY = false;
    if(m_TypeIsQObjectDerived)
    {
        //TODOreq: once I have the Q_PROPERTY checkbox, i'd then check it here. for now all QObject's members are Q_PROPERTIES kek
        makeItAQ_PROPERTY = true;
    }

    //now create the new member itself
    if(makeItAQ_PROPERTY)
    {
        static_cast<DesignEqualsImplementationClass*>(m_TypeBeingEditted)->createNewProperty(m_CurrentProject->getOrCreateTypeFromName(nonFunctionMemberParser.parsedPropertyUnqualifiedType()), nonFunctionMemberParser.parsedPropertyQualifiedType(), nonFunctionMemberParser.parsedPropertyName(), nonFunctionMemberParser.hasInit(), nonFunctionMemberParser.optionalInit(), false, true); //TODOoptional: toolbutton for read-only or non-notifying etc etc
    }
    else
    {
        //not a Q_PROPERTY
        m_TypeBeingEditted->createNewNonFunctionMember(m_CurrentProject->getOrCreateTypeFromName(nonFunctionMemberParser.parsedPropertyUnqualifiedType()), nonFunctionMemberParser.parsedPropertyQualifiedType(), DesignEqualsImplementationProjectGenerator::memberNameForProperty(nonFunctionMemberParser.parsedPropertyName()) /*TODOreq: do I really want to add m_?*/, Visibility::Private /*TODOreq: visibility radio*/, TypeInstance::isPointer(nonFunctionMemberParser.parsedPropertyQualifiedType()) ? TypeInstanceOwnershipOfPointedToDataIfPointer::DoesNotOwnPointedToData : TypeInstanceOwnershipOfPointedToDataIfPointer::NotPointer /*TODOreq: ask them if they want to own the pointer. one way to automagically determine that they do want to own the pointer is if the qualified type is pointer AND they put " = new Bar" in the OptionalInit. if we only detect it being a pointer, then we should ask. aww fuck atm OptionalInit and OwnsPointedToData are mutually exclusive, and an init of "= new Bar" wants to be both*/, nonFunctionMemberParser.hasInit(), nonFunctionMemberParser.optionalInit());
    }

    m_QuickMemberAddLineEdit->clear();
    m_QuickMemberAddLineEdit->setFocus();


    //TODOreq: maybe when they make the type an internally-designed type and it's also a pointer, i simply modal dialog ask them if they want to own/instantiate that object (makes it a hasA) or if it's just a pointer to one owned elsewhere. of course, if they type out the " = new class();" as the property initializer, we need to make sure not to conflict with their shit or overwrite it
}
void ClassEditorDialog::handleQuickAddNewSignalButtonClicked()
{
    if(!DesignEqualsImplementationGuiCommon::parseNewSignalDefinition_then_askWhatToDoWithNewSignalArgTypes_then_createNewSignal(this, static_cast<DesignEqualsImplementationClass*>(m_TypeBeingEditted), m_QuickMemberAddLineEdit->text()))
        return;

    m_QuickMemberAddLineEdit->clear();
    m_QuickMemberAddLineEdit->setFocus();
}
//TODOreq: either manually filter "_Bool" to "bool", or figure out how to get clang to give me the cpp version of a bool :-P
void ClassEditorDialog::handleQuickAddNewSlotButtonClicked()
{
    if(!DesignEqualsImplementationGuiCommon::parseNewSlotDefinition_then_askWhatToDoWithNewSlotArgTypes_then_createNewSlot(this, static_cast<DesignEqualsImplementationClass*>(m_TypeBeingEditted), m_QuickMemberAddLineEdit->text()))
        return;

    m_QuickMemberAddLineEdit->clear();
    m_QuickMemberAddLineEdit->setFocus();
}
void ClassEditorDialog::handleAddPropertyButtonClicked()
{
#if 0
    if(addPropertyFieldsAreSane())
    {
        //signal, slot invoke, or direct method call? blah, semantecs at this point...
        m_TypeBeingEditted->createNewProperty(m_CurrentProject->getOrCreateTypeFromName(m_AddPropertyTypeLineEdit->text().trimmed()), m_AddPropertyTypeLineEdit->text().trimmed(), m_AddPropertyNameLineEdit->text().trimmed(), false, "", m_AddPropertyReadOnlyCheckbox->isChecked(), m_AddPropertyNotifiesOnChangeCheckbox->isChecked()); //TODOoptional: init checkbox and init line edit (checkbox because they might want to init to empty string). the init line edit could be disabled when checkbox is unchecked
        m_AddPropertyTypeLineEdit->clear();
        m_AddPropertyNameLineEdit->clear();
    }
#endif
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
#if 0
    //TODOreq: a lot of this method, including calls it makes, is unfinished.. but i am also considering refactoring it tons so...

    if(addSlotFieldsAreSane())
    {
        QList<FunctionArgumentTypedef> slotArguments;
        int argCount = m_AddSlotArgumentsVLayout->count();
        for(int i = 0; i < argCount; ++i)
        {
            MethodSingleArgumentWidget *currentArgumentWidget = static_cast<MethodSingleArgumentWidget*>(m_AddSlotArgumentsVLayout->itemAt(i)->widget());
            const QString &argumentType = currentArgumentWidget->argumentType();
            const QString &argumentName = currentArgumentWidget->argumentName();
            if(argumentType.isEmpty() || argumentName.isEmpty())
                continue; //return? would there be more if an empty row is encountered? guess it depends on user. continue is best i guess
            FunctionArgumentTypedef slotArgument; //TODOreq: delete these non-clang member adder tabs, since I RELY on libclang to get the unqualified type out of the qualified type
            slotArguments.append(slotArgument);
            //methodToAddArgumentsTo->createNewArgument(argumentType, argumentName);
        }

        m_TypeBeingEditted->createwNewSlot(m_AddSlotNameLineEdit->text().trimmed(), slotArguments);
        //addAllArgsInLayoutToMethod(newSlot, m_AddSlotArgumentsVLayout);

        m_AddSlotNameLineEdit->clear();
        //TODOreq: reset the args layout
    }
#endif
}
void ClassEditorDialog::handleDoneButtonClicked()
{
    if(!m_QuickMemberAddLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::StandardButton questionResponse = QMessageBox::question(this, tr("Warning"), tr("The quick add line edit has content that has not been utilized yet. Are you sure you want to close this dialog? Anything typed into the quick add line edit will be lost forever."));
        if(questionResponse != QMessageBox::Yes)
        {
            return;
        }
    }
    accept();
}
void ClassEditorDialog::updateClassOverviewLabel()
{
    m_ClassOverviewLabel->setText(classDetailsAsHtmlString());
}
#if 0
void ClassEditorDialog::handleNonFunctionMemberAdded(NonFunctionMember *propertyAdded)
{
    //TODOreq: visualize added property in dialog
}
void ClassEditorDialog::handleSignalAdded(DesignEqualsImplementationClassSignal *signalAdded)
{
    //TODOreq: visualize added signal in dialog
}
void ClassEditorDialog::handleSlotAdded(DesignEqualsImplementationClassSlot *slotAdded)
{
    //TODOreq: visualize added slot in dialog
}
#endif
