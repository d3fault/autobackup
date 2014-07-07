#include "slotinvocationdialog.h"

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

SlotInvocationDialog::SlotInvocationDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum slotInvocationDialogMode, DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke, bool sourceIsActor, DesignEqualsImplementationClassSlot *slotWithCurrentContext_OrZeroIfSourceIsActor, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    //, m_UnitOfExecutionContainingSlotToInvoke(unitOfExecutionContainingSlotToInvoke) //TODOreq: it's worth noting that the unit of execution is only the DESIRED unit of execution, and that it might not be invokable from the source unit of execution (at the time of writing, that is actor... so... lol)
    , m_Layout(new QVBoxLayout())
    , m_SlotToInvoke(0)
    , m_SourceIsActor(sourceIsActor)
    , m_ArgsFillingInWidget(0)
{
    //Note: our use of the dialogMode excludes UseCaseSlotEventType altogether (it is used in the backend)

    //TODOoptional: combo boxes can be editable, but after thinking about it briefly I think adding a new slot like that would require that libclang interaction.. so for now KISS and just do existing slots (or add new slot with different

    //Signals
    QHBoxLayout *signalHLayout = new QHBoxLayout();
    QCheckBox *signalCheckbox = new QCheckBox(tr("Signals")); //TODOreq: the signal checkbox is itself disabled when in UseCaseSignalSlotEventType, forcing the signals to be enabled. TODOreq: checkbox hovertext when disabled should explain why disabled
    signalCheckbox->setChecked(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType);
    signalCheckbox->setDisabled(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType);
    signalHLayout->addWidget(signalCheckbox);

    QWidget *newSignalAndExistingSignalsWidget = new QWidget();
    newSignalAndExistingSignalsWidget->setDisabled(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType);
    QVBoxLayout *signalsLayout = new QVBoxLayout();

    //new signal
    QLineEdit *autoParsedSignalNameWithAutoCompleteForExistingSignals = new QLineEdit(); //TODOreq
    //existing signal
    m_ExistingSignalsComboBox = new QComboBox();
    if(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType)
        m_ExistingSignalsComboBox->setDisabled(true);
    m_ExistingSignalsComboBox->addItem(tr("Select signal...")); //TODOreq: for both signals and slots, make the NAME of it bold (args are less important). This should be anywhere the signature is seen
    if(!sourceIsActor)
    {
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, slotWithCurrentContext_OrZeroIfSourceIsActor->ParentClass->Signals)
        {
            m_ExistingSignalsComboBox->addItem(currentSignal->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSignal));
        }
    }
    signalsLayout->addWidget(autoParsedSignalNameWithAutoCompleteForExistingSignals);
    signalsLayout->addWidget(m_ExistingSignalsComboBox);

    signalHLayout->addWidget(newSignalAndExistingSignalsWidget);
    newSignalAndExistingSignalsWidget->setLayout(signalsLayout);
    m_Layout->addLayout(signalHLayout);



    if(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType)
        m_Layout->addWidget(new QWidget(), 1); //TODOreq: spacer inserted to space slots and space away from each other when in signal mode, verify it worked



    //Slots
    QHBoxLayout *slotHLayout = new QHBoxLayout();
    QCheckBox *slotCheckbox = new QCheckBox(tr("Slots"));
    slotCheckbox->setChecked(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType);
    slotHLayout->addWidget(slotCheckbox);

    QWidget *newSlotAndExistingSlotsWidget = new QWidget();
    QVBoxLayout *slotsLayout = new QVBoxLayout();

    //new slot
    QLineEdit *autoParsedSlotNameWithAutoCompleteForExistingSignals = new QLineEdit(); //TODOreq
    //existing slot
    m_SlotsComboBox = new QComboBox();
    m_SlotsComboBox->addItem(tr("Select slot...")); //TODOreq: qlistwidget only takes one click insead of two (precious seconds when you're trying to keep a complicated design in your head)... but the trade off is that it takes up more space (not really though, once the combo box is expanded...)
    if(slotInvocationDialogMode != DesignEqualsImplementationUseCase::UseCaseSignalEventType)
    {
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, unitOfExecutionContainingSlotToInvoke->designEqualsImplementationClassLifeLine()->designEqualsImplementationClass()->Slots)
        {
            m_SlotsComboBox->addItem(currentSlot->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSlot));
        }
    }
    else
    {
        //no destination context, so they'd have to select an object/lifeline/unit-of-exectution or create one on the fly TODOreq
    }
    connect(m_SlotsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSlotsComboBoxICurrentIndexChanged(int)));
    newSlotAndExistingSlotsWidget->setDisabled(slotInvocationDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType);
    slotsLayout->addWidget(autoParsedSlotNameWithAutoCompleteForExistingSignals);
    slotsLayout->addWidget(m_SlotsComboBox);

    slotHLayout->addWidget(newSlotAndExistingSlotsWidget);
    newSlotAndExistingSlotsWidget->setLayout(slotsLayout);
    m_Layout->addLayout(slotHLayout);

    QHBoxLayout *cancelOkRow = new QHBoxLayout();

    m_OkButton = new QPushButton(tr("Ok"), this); //TODOreq: button stays below arg filling in
    m_OkButton->setDefault(true);
    m_OkButton->setDisabled(true);

    connect(m_OkButton, SIGNAL(clicked()), this, SLOT(accept()));

    cancelOkRow->addWidget(m_OkButton);

    m_Layout->addLayout(cancelOkRow);
    setLayout(m_Layout);

    if(!sourceIsActor)
    {
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
}
DesignEqualsImplementationClassSlot *SlotInvocationDialog::slotToInvoke() const
{
    return m_SlotToInvoke;
}
SignalEmissionOrSlotInvocationContextVariables SlotInvocationDialog::slotInvocationContextVariables() const
{
    //Doesn't do validation checking. The dialog returning Accepted does though
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables;
    Q_FOREACH(QComboBox *currentArg, m_AllArgSatisfiers)
    {
        slotInvocationContextVariables.OrderedListOfNamesOfVariablesWithinScopeWhenSignalEmissionOrSlotInvocationOccurrs_ToUseForSignalEmissionOrSlotInvocationArguments.append(qvariant_cast<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*>(currentArg->currentData())->VariableName);
    }
    return slotInvocationContextVariables;
}
void SlotInvocationDialog::showSlotArgFillingIn()
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
void SlotInvocationDialog::collapseSlotArgFillingIn()
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
bool SlotInvocationDialog::allArgSatisfiersAreValid()
{
    Q_FOREACH(QComboBox *currentComboBox, m_AllArgSatisfiers)
    {
        if(currentComboBox->currentIndex() == 0)
            return false;
    }
    return true;
}
void SlotInvocationDialog::handleSlotsComboBoxICurrentIndexChanged(int newIndex)
{
    if(newIndex == 0)
    {
        collapseSlotArgFillingIn();
        m_OkButton->setDisabled(true);
        return;
    }

    m_SlotToInvoke = qvariant_cast<DesignEqualsImplementationClassSlot*>(m_SlotsComboBox->itemData(newIndex));

    //TODOreq: present selectable context variables for the slot args. This is required before OK is enabled, but not when it's the first/actor->slotInvoke (grayed out in that case)
    showSlotArgFillingIn();
}
void SlotInvocationDialog::handleArgSatisfierChosen()
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
