#include "signalslotmessagedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>

#include "../../designequalsimplementationcommon.h"
#include "../../designequalsimplementationclassslot.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

//TODOreq: SignalSlotMessageEditorDialog (creation + editting-later-on using same widget) would be best
//TODOreq: signal/slot mode, slot args populated by signal args, can have less than signal arg count, but arg-ordering and arg-type matter
//TODOreq: if Foo hasA Bar, we cannot invoke handleFooSignal DIRECTLY from within barSlot. Bar needs either barSignal to be connected to handleFooSignal in the UML/Design (easy), or a pointer to Foo must be somehow communicated to Bar (during construction works). As of right now, trying to INVOKE (directly) handleBarSignal from barSlot gives an invalid slot invocation statement (there is no variable name of Foo, so it looks like this: "invokeMethod(, "handleBarSignal);, obviously not valid. Ideally we could support both variations, but until then the GUI should at least not allow direct invocation on anything that doesn't have a name to refer to the target by
//TODOreq: Possibly disallow selecting target slots already existing on the current class lifeline, because I'm not sure that makes any sense. However, my instincts tell me it's ok to target an already-on-this-class-lifeline slot when it occurs on a different lifeline. Recursion might mean that putting the same slot on one class lifeline multiple times is fine and dandy, but I'm having a hard time wrapping my head around how that would work
//TODOoptional: if instanceType isn't set for the destination slot, we can provide a optional "express" button to give it the signal's object as it's parent. it streamlines the left to right flow and helps minimize necessary clicks. there could be other options such as making them share a parent, or the reverse where the signal object becomes the child. as you can see there are lots of options, but the first one mentioned is the most common (followed by second mentioned) and the "saved clicks" value is what makes it the most beneficiail. by far. obviously, if the instanceType indicates one is already chosen, the button is not shown at all and only the regular "ok" is shown. perhaps the express button, when present, should be the "default" button (pressing enter blindly at dialog)
SignalSlotMessageDialog::SignalSlotMessageDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode, DesignEqualsImplementationClassSlot *destinationSlotToInvoke_OrZeroIfNoDest, bool sourceIsActor, bool destinationIsActor, DesignEqualsImplementationClassSlot *sourceSlot_OrZeroIfSourceIsActor, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    //, m_UnitOfExecutionContainingSlotToInvoke(unitOfExecutionContainingSlotToInvoke) //TODOreq: it's worth noting that the unit of execution is only the DESIRED unit of execution, and that it might not be invokable from the source unit of execution (at the time of writing, that is actor... so... lol)
    , m_Layout(new QVBoxLayout())
    , m_SignalToEmit(0)
    , m_SlotToInvoke(0)
    , m_SourceIsActor(sourceIsActor)
    , m_DestinationIsActor(destinationIsActor)
    , m_SignalArgsFillingInWidget(0)
    , m_SlotArgsFillingInWidget(0)
{
    setWindowTitle(tr("Signal/Slot Message"));
    //Note: our use of the dialogMode excludes UseCaseSlotEventType altogether (it is used in the backend)

    //TODOoptional: combo boxes can be editable, but after thinking about it briefly I think adding a new slot like that would require that libclang interaction.. so for now KISS and just do existing slots (or add new slot with different


    //Layout placement

    //Signals
    QHBoxLayout *signalHLayout = new QHBoxLayout();
    m_SignalsCheckbox = new QCheckBox(tr("Signal")); //TODOreq: the signal checkbox is itself disabled when in UseCaseSignalSlotEventType, forcing the signals to be enabled. TODOreq: checkbox hovertext when disabled should explain why disabled

    signalHLayout->addWidget(m_SignalsCheckbox);

    QWidget *newSignalAndExistingSignalsWidget = new QWidget();
    QVBoxLayout *signalsLayout = new QVBoxLayout();

    //new signal
    QLineEdit *autoParsedSignalNameWithAutoCompleteForExistingSignals = new QLineEdit(); //TODOreq. TODOreq: "create handler in slot" (checks Slot checkbox if needed). ex: "handleFooSignal"
    //existing signal
    m_ExistingSignalsComboBox = new QComboBox();
    m_ExistingSignalsComboBox->addItem(tr("Select signal...")); //TODOreq: for both signals and slots, make the NAME of it bold (args are less important). This should be anywhere the signature is seen
    signalsLayout->addWidget(autoParsedSignalNameWithAutoCompleteForExistingSignals);
    signalsLayout->addWidget(m_ExistingSignalsComboBox);

    signalHLayout->addWidget(newSignalAndExistingSignalsWidget);
    newSignalAndExistingSignalsWidget->setLayout(signalsLayout);
    m_Layout->addLayout(signalHLayout);

    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType)
        m_Layout->addWidget(new QWidget(), 1); //TODOreq: spacer inserted to space slots and space away from each other when in signal mode, verify it worked

    //Slots
    QHBoxLayout *slotHLayout = new QHBoxLayout();
    m_SlotsCheckbox = new QCheckBox(tr("Slot"));
    m_SlotsCheckbox->setChecked(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType);
    slotHLayout->addWidget(m_SlotsCheckbox);

    QWidget *newSlotAndExistingSlotsWidget = new QWidget();
    QVBoxLayout *slotsLayout = new QVBoxLayout();

    //new slot
    QLineEdit *autoParsedSlotNameWithAutoCompleteForExistingSignals = new QLineEdit(); //TODOreq
    //existing slot
    m_ExistingSlotsComboBox = new QComboBox();
    m_ExistingSlotsComboBox->addItem(tr("Select slot...")); //TODOreq: qlistwidget only takes one click insead of two (precious seconds when you're trying to keep a complicated design in your head)... but the trade off is that it takes up more space (not really though, once the combo box is expanded...)

    slotsLayout->addWidget(autoParsedSlotNameWithAutoCompleteForExistingSignals);
    slotsLayout->addWidget(m_ExistingSlotsComboBox);

    slotHLayout->addWidget(newSlotAndExistingSlotsWidget);
    newSlotAndExistingSlotsWidget->setLayout(slotsLayout);
    m_Layout->addLayout(slotHLayout);

    QHBoxLayout *cancelOkRow = new QHBoxLayout();

    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    m_OkButton = new QPushButton(tr("Ok"), this); //TODOreq: button stays below arg filling in
    m_OkButton->setDefault(true);
    m_OkButton->setDisabled(true);

    cancelOkRow->addWidget(cancelButton);
    cancelOkRow->addWidget(m_OkButton);

    m_Layout->addLayout(cancelOkRow);


    //Layout visibility and poulating

    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType)
    {
        //no destination context, so they'd have to select an object/lifeline/unit-of-exectution or create one on the fly TODOreq
        newSlotAndExistingSlotsWidget->setDisabled(true);
        m_SignalsCheckbox->setChecked(true);
        newSignalAndExistingSignalsWidget->setDisabled(false);
        m_SignalsCheckbox->setToolTip(tr("You can't NOT have a signal when you don't have a destination"));
        m_SlotsCheckbox->setDisabled(true);
        m_SlotsCheckbox->setToolTip(tr("You can't have a slot when you don't have a destination")); //TODOreq: create destination+slot on the fly
    }
    else
    {
        newSignalAndExistingSignalsWidget->setDisabled(true);
        m_SlotsCheckbox->setChecked(true);
        m_SlotsCheckbox->setDisabled(true);
        m_SlotsCheckbox->setToolTip(tr("You have a destination object, therefore you must have a slot")); //TODOreq: actor as destination breaks rule bleh (don't think it matters because actor uses UseCaseSignalEventType?)
        newSlotAndExistingSlotsWidget->setDisabled(false);
        if(destinationSlotToInvoke_OrZeroIfNoDest) //I think this is impied != 0 when not UseCaseSignalEventType, however I'm not sure of it and the check is cheap and prevents segfault lolol
        {
            Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, destinationSlotToInvoke_OrZeroIfNoDest->ParentClass->mySlots()) //List the slots on the target slot, derp
            {
                if(currentSlot->Name == UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING)
                    continue;
                m_ExistingSlotsComboBox->addItem(currentSlot->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSlot));
            }
        }
    }

    if(sourceIsActor)
    {
        m_SignalsCheckbox->setChecked(false);
        m_SignalsCheckbox->setDisabled(true);
        m_SignalsCheckbox->setToolTip(tr("You can't have a signal when the source is actor")); //For now. In the future we'll be able to convert
    }
    else
    {
        if(!sourceSlot_OrZeroIfSourceIsActor) //hack that i put this here, it's where a segfault occured :)
        {
            QMessageBox::information(this, tr("Error"), tr("Source must be connected/named/entered first")); //TODOreq: the source should be [defined as] just an unnamed-as-of-yet  "interface invocation" (the interface and the invocation upon it can both be nameless at any time, except when generating code (an auto-namer util could be used for those who can't be fucked TODOoptional))
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
            return;
        }
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, sourceSlot_OrZeroIfSourceIsActor->ParentClass->mySignals())
        {
            m_ExistingSignalsComboBox->addItem(currentSignal->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSignal));
        }

        //fill in list of variables in current context to use for satisfying whatever slot they choose's arguments. TODOreq: prefix the "source" of the arg satisfier, and perhaps sort them by that too. "my-method-arguments", "my-class-members", etc)
        //m_VariablesAvailableToSatisfyArgs.append(*(slotWithCurrentContext_OrZeroIfSourceIsActor->Arguments));
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentArg, sourceSlot_OrZeroIfSourceIsActor->arguments())
        {
            m_VariablesAvailableToSatisfyArgs.append(currentArg);
        }
        //m_VariablesAvailableToSatisfyArgs.append(*slotWithCurrentContext_OrZeroIfSourceIsActor->ParentClass->HasA_PrivateMemberClasses);
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentHasAClass, sourceSlot_OrZeroIfSourceIsActor->ParentClass->hasA_Private_Classes_Members())
        {
            m_VariablesAvailableToSatisfyArgs.append(currentHasAClass);
        }
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentHasAPod, sourceSlot_OrZeroIfSourceIsActor->ParentClass->hasA_Private_PODorNonDesignedCpp_Members())
        {
            m_VariablesAvailableToSatisfyArgs.append(currentHasAPod);
        }
    }

    if(destinationIsActor)
    {
        m_SlotsCheckbox->setChecked(false);
        m_SlotsCheckbox->setDisabled(true);
        newSlotAndExistingSlotsWidget->setDisabled(true);
        m_SlotsCheckbox->setToolTip(tr("You can't choose a slot when the destination is actor"));
    }

    //Connections
    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType)
    {
        //Can't disable signals in other mode, so no point in listening
        connect(m_SignalsCheckbox, SIGNAL(toggled(bool)), newSignalAndExistingSignalsWidget, SLOT(setEnabled(bool)));
        connect(m_SignalsCheckbox, SIGNAL(toggled(bool)), this, SLOT(handleSignalCheckboxToggled(bool)));

        //No slot in other mode, so no point in listening
        connect(m_SlotsCheckbox, SIGNAL(toggled(bool)), newSlotAndExistingSlotsWidget, SLOT(setEnabled(bool)));
        connect(m_SlotsCheckbox, SIGNAL(toggled(bool)), this, SLOT(handleSlotCheckboxToggled(bool)));
        connect(m_ExistingSlotsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExistingSlotsComboBoxCurrentIndexChanged(int)));
    }

    connect(m_ExistingSignalsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExistingSignalComboBoxIndexChanged(int)));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_OkButton, SIGNAL(clicked()), this, SLOT(accept()));

    setLayout(m_Layout);
}
DesignEqualsImplementationClassSignal *SignalSlotMessageDialog::signalToEmit_OrZeroIfNone() const //TODOreq: set m_SignalToEmit to zero when user unchecks "Signal"
{
    return m_SignalToEmit;
}
DesignEqualsImplementationClassSlot *SignalSlotMessageDialog::slotToInvoke_OrZeroIfNone() const //TODOreq: set m_SlotToInvoke to zero when user unchecks "Slot"
{
    return m_SlotToInvoke;
}
SignalEmissionOrSlotInvocationContextVariables SignalSlotMessageDialog::slotInvocationContextVariables() const
{
    //Doesn't do validation checking. The dialog returning Accepted does though
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables;
    Q_FOREACH(QComboBox *currentArg, m_AllArgSatisfiers)
    {
        slotInvocationContextVariables.OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments.append(qvariant_cast<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*>(currentArg->currentData())->VariableName);
    }
    return slotInvocationContextVariables;
}
void SignalSlotMessageDialog::showSignalArgFillingIn()
{
    //TODOreq

    if(m_SlotArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_SlotArgsFillingInWidget;
        m_SlotArgsFillingInWidget = 0;
    }
    if(m_SignalArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_SignalArgsFillingInWidget;
        m_SignalArgsFillingInWidget = 0;
    }
    if(m_SignalToEmit->arguments().isEmpty())
    {
        if(!m_SlotToInvoke)
            m_OkButton->setDisabled(false); //signal emit with no listeners
        return;
    }

    m_SignalArgsFillingInWidget = new QWidget();
    QVBoxLayout *argsFillingInLayout = new QVBoxLayout(); //TODOreq: a scroll bar may be needed if the slot has too many args, but really 10 is a decent soft limit that Qt uses also... any more and you suck at designing :-P
    argsFillingInLayout->addWidget(new QLabel(QObject::tr("Fill in the arguments for: ") + m_SignalToEmit->Name), 0, Qt::AlignLeft);
    Q_FOREACH(DesignEqualsImplementationClassMethodArgument* currentArgument, m_SignalToEmit->arguments())
    {
        QHBoxLayout *currentArgRow = new QHBoxLayout();
        currentArgRow->addWidget(new QLabel(currentArgument->preferredTextualRepresentationOfTypeAndVariableTogether()));
        QComboBox *currentArgSatisfiersComboBox = new QComboBox();
        connect(currentArgSatisfiersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(tryValidatingDialog()));
        currentArgSatisfiersComboBox->addItem(tr("Select variable for this arg..."));
        m_AllArgSatisfiers.append(currentArgSatisfiersComboBox);
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentArgSatisfier, m_VariablesAvailableToSatisfyArgs)
        {
            currentArgSatisfiersComboBox->addItem(currentArgSatisfier->preferredTextualRepresentationOfTypeAndVariableTogether(), QVariant::fromValue(currentArgSatisfier));
        }
        currentArgRow->addWidget(currentArgSatisfiersComboBox);
        argsFillingInLayout->addLayout(currentArgRow);
    }
    m_SignalArgsFillingInWidget->setLayout(argsFillingInLayout);
    m_Layout->addWidget(m_SignalArgsFillingInWidget);
    m_OkButton->setDisabled(true);

    if(m_SlotsCheckbox->isChecked() && m_ExistingSlotsComboBox->currentIndex() != 0)
    {
        showSlotArgFillingIn();
    }
}
void SignalSlotMessageDialog::collapseSignalArgFillingIn()
{
    if(m_SignalArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_SignalArgsFillingInWidget;
        m_SignalArgsFillingInWidget = 0;
    }
    if(m_SlotsCheckbox->isChecked() && m_ExistingSlotsComboBox->currentIndex() != 0)
    {
        showSlotArgFillingIn(); //what was "slot arg match up with signal arg" becomes "choose context variables to satisfy slot args"
    }
}
void SignalSlotMessageDialog::showSlotArgFillingIn()
{
    if(m_SourceIsActor)
    {
        return;
    }

    if(m_SlotArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_SlotArgsFillingInWidget;
        m_SlotArgsFillingInWidget = 0;
    }
    if(m_SlotsCheckbox->isChecked() && m_ExistingSlotsComboBox->currentIndex() != 0 && m_SlotToInvoke->arguments().isEmpty())
    {
        return;
    }

    if(!m_SignalsCheckbox->isChecked())
    {
        m_SlotArgsFillingInWidget = new QWidget();
        QVBoxLayout *argsFillingInLayout = new QVBoxLayout(); //TODOreq: a scroll bar may be needed if the slot has too many args, but really 10 is a decent soft limit that Qt uses also... any more and you suck at designing :-P
        argsFillingInLayout->addWidget(new QLabel(QObject::tr("Fill in the arguments for: ") + m_SlotToInvoke->Name), 0, Qt::AlignLeft);
        Q_FOREACH(DesignEqualsImplementationClassMethodArgument* currentArgument, m_SlotToInvoke->arguments())
        {
            QHBoxLayout *currentArgRow = new QHBoxLayout(); //TODOoptimization: one grid layout instead? fuck it
            currentArgRow->addWidget(new QLabel(currentArgument->preferredTextualRepresentationOfTypeAndVariableTogether()));
            QComboBox *currentArgSatisfiersComboBox = new QComboBox(); //instead of listening to signals, i should just manually validate the dialog when ok is pressed (keep a list of combo boxes, ensure all indexes aren't zero)... only downside to that is that the ok button now can't be disabled :(... fffff. i guess whole dialog validation on ANY combo box signal change is a hacky/easy/unoptimal/functional solution TODOoptimization proper dat shit
            connect(currentArgSatisfiersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(tryValidatingDialog()));
            currentArgSatisfiersComboBox->addItem(tr("Select variable for this arg..."));
            m_AllArgSatisfiers.append(currentArgSatisfiersComboBox);
            Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentArgSatisfier, m_VariablesAvailableToSatisfyArgs)
            {
                currentArgSatisfiersComboBox->addItem(currentArgSatisfier->preferredTextualRepresentationOfTypeAndVariableTogether(), QVariant::fromValue(currentArgSatisfier));
            }
            currentArgRow->addWidget(currentArgSatisfiersComboBox);
            argsFillingInLayout->addLayout(currentArgRow);
        }
        m_SlotArgsFillingInWidget->setLayout(argsFillingInLayout);
        m_Layout->addWidget(m_SlotArgsFillingInWidget);
    }
}
void SignalSlotMessageDialog::collapseSlotArgFillingIn()
{
    if(m_SourceIsActor)
        return;

    if(m_SlotArgsFillingInWidget)
    {
        if(!m_SignalArgsFillingInWidget)
            m_AllArgSatisfiers.clear();
        delete m_SlotArgsFillingInWidget;
        m_SlotArgsFillingInWidget = 0;
    }
}
bool SignalSlotMessageDialog::allArgSatisfiersAreValid()
{
    //Check arg satisfiers validity (for either signal or slot)
    Q_FOREACH(QComboBox *currentComboBox, m_AllArgSatisfiers)
    {
        if(currentComboBox->currentIndex() == 0) //TODOlater: type checking before even putting it in combo box of selectable items (inheritence means this is a bitch)
            return false;
#if 0
        if(m_SignalsCheckbox->isChecked())
        {
            if(m_ExistingSignalsComboBox->currentIndex() == 0)
                return false;
        }
#endif
    }

    //Check signal arg count >= slot arg count, and check signal arg types match slot arg types
    if(m_SignalsCheckbox->isChecked() && m_SlotsCheckbox->isChecked())
    {
        if(m_ExistingSignalsComboBox->currentIndex() != 0 && m_ExistingSlotsComboBox->currentIndex() != 0)
        {
            //TODOreq: verify signal/slot arg compatibility, return true if checks out (TODOoptional: highlight in red the slot so they know it's not the arg satisfiers)
            if(m_SignalToEmit->arguments().size() < m_SlotToInvoke->arguments().size())
                return false;
            int currentArgIndex = 0;
            Q_FOREACH(DesignEqualsImplementationClassMethodArgument *currentSlotArgument, m_SlotToInvoke->arguments())
            {
                const QByteArray &slotArgTypeCstr = currentSlotArgument->typeString().toUtf8();
                const QByteArray &signalArgTypeCstr = m_SignalToEmit->arguments().at(currentArgIndex++)->typeString().toUtf8();
                if(QMetaObject::normalizedType(slotArgTypeCstr.constData()) != QMetaObject::normalizedType(signalArgTypeCstr.constData())) //TODOoptional: maybe QMetaObject::checkConnectArgs handles inheritence stuff for me? I might need to register the to-be-generated types at runtime for it to work?
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    return true;
}
#if 0
bool SignalSlotMessageDialog::allSlotsArgsMatchedUpWithSignalArgsIfSlotEvenChecked()
{
    //returns true if slots not even checked
    if(m_SlotsCheckbox->isChecked())
        return true;

    //TODOreq: QComboBox for each slot arg, populated with signal arg. Wait wtf is the point of this? QObject::connect() argument type/position editting is more a class diagram perspective thing
    //Still, the signal types should satisfy the slot types (take out const and "&" when comparing), AND there must be at least as many signal arguments as there are slot args (no default values for now) TODOreq... and for now you have to choose a different slot before teh ok button is enabled (future versions have in-line editting of the slot to make it work)
    if(m_ExistingSlotsComboBox->currentIndex() == 0)
        return false;
}
#endif
void SignalSlotMessageDialog::handleSignalCheckboxToggled(bool checked)
{
    if(!checked)
        m_SignalToEmit = 0;
}
void SignalSlotMessageDialog::handleSlotCheckboxToggled(bool checked)
{
    if(!checked)
        m_SlotToInvoke = 0;
}
void SignalSlotMessageDialog::handleExistingSignalComboBoxIndexChanged(int newIndex)
{
    if(newIndex == 0)
    {
        collapseSignalArgFillingIn();
    }
    else
    {
        m_SignalToEmit = qvariant_cast<DesignEqualsImplementationClassSignal*>(m_ExistingSignalsComboBox->itemData(newIndex));
        showSignalArgFillingIn();
    }
    tryValidatingDialog();
}
void SignalSlotMessageDialog::handleExistingSlotsComboBoxCurrentIndexChanged(int newIndex)
{
    if(newIndex == 0)
    {
        collapseSlotArgFillingIn();
    }
    else
    {
        m_SlotToInvoke = qvariant_cast<DesignEqualsImplementationClassSlot*>(m_ExistingSlotsComboBox->itemData(newIndex));
        //TODOreq: present selectable context variables for the slot args. This is required before OK is enabled, but not when it's the first/actor->slotInvoke (grayed out in that case)
        showSlotArgFillingIn();
    }
    tryValidatingDialog();
}
void SignalSlotMessageDialog::tryValidatingDialog()
{
    //validate them all, enable disable ok button accordingly
    if(allArgSatisfiersAreValid()/* && allSlotsArgsMatchedUpWithSignalArgsIfSlotEvenChecked()*/)
    {
        m_OkButton->setDisabled(false);
    }
    else
    {
        m_OkButton->setDisabled(true);
    }
}
