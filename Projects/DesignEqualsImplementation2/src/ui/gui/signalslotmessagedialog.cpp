#include "signalslotmessagedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>

#include "../../designequalsimplementationclassslot.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

//TODOreq: SignalSlotMessageEditorDialog (creation + editting-later-on using same widget) would be best
//TODOreq: signal/slot mode, slot args populated by signal args, can have less than signal arg count, but arg-ordering and arg-type matter
SignalSlotMessageDialog::SignalSlotMessageDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke_OrZeroIfNoDest, bool sourceIsActor, DesignEqualsImplementationClassSlot *slotWithCurrentContext_OrZeroIfSourceIsActor, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    //, m_UnitOfExecutionContainingSlotToInvoke(unitOfExecutionContainingSlotToInvoke) //TODOreq: it's worth noting that the unit of execution is only the DESIRED unit of execution, and that it might not be invokable from the source unit of execution (at the time of writing, that is actor... so... lol)
    , m_Layout(new QVBoxLayout())
    , m_SignalToEmit(0)
    , m_SlotToInvoke(0)
    , m_SourceIsActor(sourceIsActor)
    , m_ArgsFillingInWidget(0)
{
    //Note: our use of the dialogMode excludes UseCaseSlotEventType altogether (it is used in the backend)

    //TODOoptional: combo boxes can be editable, but after thinking about it briefly I think adding a new slot like that would require that libclang interaction.. so for now KISS and just do existing slots (or add new slot with different


    //Layout placement

    //Signals
    QHBoxLayout *signalHLayout = new QHBoxLayout();
    QCheckBox *signalCheckbox = new QCheckBox(tr("Signals")); //TODOreq: the signal checkbox is itself disabled when in UseCaseSignalSlotEventType, forcing the signals to be enabled. TODOreq: checkbox hovertext when disabled should explain why disabled

    signalHLayout->addWidget(signalCheckbox);

    QWidget *newSignalAndExistingSignalsWidget = new QWidget();
    QVBoxLayout *signalsLayout = new QVBoxLayout();

    //new signal
    QLineEdit *autoParsedSignalNameWithAutoCompleteForExistingSignals = new QLineEdit(); //TODOreq
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
    QCheckBox *slotCheckbox = new QCheckBox(tr("Slots"));
    slotCheckbox->setChecked(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType);
    slotHLayout->addWidget(slotCheckbox);

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

    m_OkButton = new QPushButton(tr("Ok"), this); //TODOreq: button stays below arg filling in
    m_OkButton->setDefault(true);
    m_OkButton->setDisabled(true);

    cancelOkRow->addWidget(m_OkButton);

    m_Layout->addLayout(cancelOkRow);


    //Layout visibility and poulating

    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType)
    {
        //no destination context, so they'd have to select an object/lifeline/unit-of-exectution or create one on the fly TODOreq
        newSlotAndExistingSlotsWidget->setDisabled(true);
        signalCheckbox->setChecked(true);
        newSignalAndExistingSignalsWidget->setDisabled(true);
        signalCheckbox->setToolTip(tr("You can't NOT have a signal when you don't have a destination"));
    }
    else
    {
        newSignalAndExistingSignalsWidget->setDisabled(true);
        slotCheckbox->setChecked(true);
        newSlotAndExistingSlotsWidget->setDisabled(false);
        if(unitOfExecutionContainingSlotToInvoke_OrZeroIfNoDest) //I think this is impied != 0 when not UseCaseSignalEventType, however I'm not sure of it and the check is cheap and prevents segfault lolol
        {
            Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, unitOfExecutionContainingSlotToInvoke_OrZeroIfNoDest->designEqualsImplementationClassLifeLine()->designEqualsImplementationClass()->Slots)
            {
                m_ExistingSlotsComboBox->addItem(currentSlot->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSlot));
            }
        }
    }

    if(sourceIsActor)
    {
        signalCheckbox->setChecked(false);
        signalCheckbox->setDisabled(true);
        signalCheckbox->setToolTip(tr("You can't have a signal when the source is actor")); //For now. In the future we'll be able to convert
    }
    else
    {
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, slotWithCurrentContext_OrZeroIfSourceIsActor->ParentClass->Signals)
        {
            m_ExistingSignalsComboBox->addItem(currentSignal->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSignal));
        }

        //fill in list of variables in current context to use for satisfying whatever slot they choose's arguments
        //m_VariablesAvailableToSatisfyArgs.append(*(slotWithCurrentContext_OrZeroIfSourceIsActor->Arguments));
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentArg, slotWithCurrentContext_OrZeroIfSourceIsActor->Arguments)
        {
            m_VariablesAvailableToSatisfyArgs.append(currentArg);
        }
        //m_VariablesAvailableToSatisfyArgs.append(*slotWithCurrentContext_OrZeroIfSourceIsActor->ParentClass->HasA_PrivateMemberClasses);
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentHasAClass, slotWithCurrentContext_OrZeroIfSourceIsActor->ParentClass->HasA_PrivateMemberClasses)
        {
            m_VariablesAvailableToSatisfyArgs.append(currentHasAClass);
        }
    }


    //Connections
    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType)
    {
        //Can't disable signals in other mode, so no point in listening
        connect(signalCheckbox, SIGNAL(toggled(bool)), newSignalAndExistingSignalsWidget, SLOT(setEnabled(bool)));
        connect(signalCheckbox, SIGNAL(toggled(bool)), this, SLOT(handleSignalCheckboxToggled(bool)));

        //No slot in other mode, so no point in listening
        connect(slotCheckbox, SIGNAL(toggled(bool)), newSlotAndExistingSlotsWidget, SLOT(setEnabled(bool)));
        connect(slotCheckbox, SIGNAL(toggled(bool)), this, SLOT(handleSlotCheckboxToggled(bool)));
        connect(m_ExistingSlotsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExistingSlotsComboBoxICurrentIndexChanged(int)));
    }

    connect(m_ExistingSignalsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExistingSignalComboBoxIndexChanged(int)));
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
}
void SignalSlotMessageDialog::collapseSignalArgFillingIn()
{
    //TODOreq
}
void SignalSlotMessageDialog::showSlotArgFillingIn()
{
    if(m_SourceIsActor)
    {
        m_OkButton->setDisabled(false);
        return;
    }

    if(m_ArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_ArgsFillingInWidget;
        m_ArgsFillingInWidget = 0;
    }
    if(m_SlotToInvoke->Arguments.isEmpty())
    {
        m_OkButton->setDisabled(false);
        return;
    }
    m_ArgsFillingInWidget = new QWidget();
    QVBoxLayout *argsFillingInLayout = new QVBoxLayout(); //TODOreq: a scroll bar may be needed if the slot has too many args, but really 10 is a decent soft limit that Qt uses also... any more and you suck at designing :-P
    argsFillingInLayout->addWidget(new QLabel(QObject::tr("Fill in the arguments for: ") + m_SlotToInvoke->Name), 0, Qt::AlignLeft);
    Q_FOREACH(DesignEqualsImplementationClassMethodArgument* currentArgument, m_SlotToInvoke->Arguments)
    {
        QHBoxLayout *currentArgRow = new QHBoxLayout(); //TODOoptimization: one grid layout instead? fuck it
        currentArgRow->addWidget(new QLabel(currentArgument->preferredTextualRepresentation()));
        QComboBox *currentArgSatisfiersComboBox = new QComboBox(); //instead of listening to signals, i should just manually validate the dialog when ok is pressed (keep a list of combo boxes, ensure all indexes aren't zero)... only downside to that is that the ok button now can't be disabled :(... fffff. i guess whole dialog validation on ANY combo box signal change is a hacky/easy/unoptimal/functional solution TODOoptimization proper dat shit
        connect(currentArgSatisfiersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleArgSatisfierChosen()));
        currentArgSatisfiersComboBox->addItem(tr("Select variable for this arg..."));
        m_AllArgSatisfiers.append(currentArgSatisfiersComboBox);
        Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentArgSatisfier, m_VariablesAvailableToSatisfyArgs)
        {
            currentArgSatisfiersComboBox->addItem(currentArgSatisfier->preferredTextualRepresentation(), QVariant::fromValue(currentArgSatisfier));
        }
        currentArgRow->addWidget(currentArgSatisfiersComboBox);
        argsFillingInLayout->addLayout(currentArgRow);
    }
    m_ArgsFillingInWidget->setLayout(argsFillingInLayout);
    m_Layout->addWidget(m_ArgsFillingInWidget);
    m_OkButton->setDisabled(true);
}
void SignalSlotMessageDialog::collapseSlotArgFillingIn()
{
    if(m_SourceIsActor)
        return;

    if(m_ArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_ArgsFillingInWidget;
        m_ArgsFillingInWidget = 0;
    }
}
bool SignalSlotMessageDialog::allArgSatisfiersAreValid()
{
    Q_FOREACH(QComboBox *currentComboBox, m_AllArgSatisfiers)
    {
        if(currentComboBox->currentIndex() == 0)
            return false;
    }
    return true;
}
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
        collapseSlotArgFillingIn();
        return;
    }
    m_SignalToEmit = qvariant_cast<DesignEqualsImplementationClassSignal*>(m_ExistingSignalsComboBox->itemData(newIndex));
    showSignalArgFillingIn();
}
void SignalSlotMessageDialog::handleExistingSlotsComboBoxICurrentIndexChanged(int newIndex)
{
    if(newIndex == 0)
    {
        collapseSlotArgFillingIn();
        m_OkButton->setDisabled(true);
        return;
    }
    m_SlotToInvoke = qvariant_cast<DesignEqualsImplementationClassSlot*>(m_ExistingSlotsComboBox->itemData(newIndex));
    //TODOreq: present selectable context variables for the slot args. This is required before OK is enabled, but not when it's the first/actor->slotInvoke (grayed out in that case)
    showSlotArgFillingIn();
}
void SignalSlotMessageDialog::handleArgSatisfierChosen()
{
    //validate them all, enable disable ok button accordingly
    if(allArgSatisfiersAreValid())
    {
        m_OkButton->setDisabled(false);
    }
    else
    {
        m_OkButton->setDisabled(true);
    }
}
