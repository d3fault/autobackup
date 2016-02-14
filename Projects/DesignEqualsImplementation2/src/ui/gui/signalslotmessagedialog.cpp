#include "signalslotmessagedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QToolButton>
#include <QAction>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>

#include "designequalsimplementationguicommon.h"
#include "classinstancechooserdialog.h"
#include "../../designequalsimplementationcommon.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclassslot.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"
#include "../../designequalsimplementationlenientsignalorslotsignaturerparser.h"

//TODOreq: SignalSlotMessageEditorDialog (creation + editting-later-on using same widget) would be best
//TODOreq: signal/slot mode, slot args populated by signal args, can have less than signal arg count, but arg-ordering and arg-type matter
//TODOreq: if Foo hasA Bar, we cannot invoke handleFooSignal DIRECTLY from within barSlot. Bar needs either barSignal to be connected to handleFooSignal in the UML/Design (easy), or a pointer to Foo must be somehow communicated to Bar (during construction works). As of right now, trying to INVOKE (directly) handleBarSignal from barSlot gives an invalid slot invocation statement (there is no variable name of Foo, so it looks like this: "invokeMethod(, "handleBarSignal);, obviously not valid. Ideally we could support both variations, but until then the GUI should at least not allow direct invocation on anything that doesn't have a name to refer to the target by
//TODOreq: Possibly disallow selecting target slots already existing on the current class lifeline, because I'm not sure that makes any sense. However, my instincts tell me it's ok to target an already-on-this-class-lifeline slot when it occurs on a different lifeline. Recursion might mean that putting the same slot on one class lifeline multiple times is fine and dandy, but I'm having a hard time wrapping my head around how that would work
//TODOoptional: if instanceType isn't set for the destination slot, we can provide a optional "express" button to give it the signal's object as it's parent. it streamlines the left to right flow and helps minimize necessary clicks. there could be other options such as making them share a parent, or the reverse where the signal object becomes the child. as you can see there are lots of options, but the first one mentioned is the most common (followed by second mentioned) and the "saved clicks" value is what makes it the most beneficiail. by far. obviously, if the instanceType indicates one is already chosen, the button is not shown at all and only the regular "ok" is shown. perhaps the express button, when present, should be the "default" button (pressing enter blindly at dialog)
//^ a) when the source hasA member of type dest, and when dest has no instanceType, the express button offers to use the existing member
//  b) otherwise, we allow the CREATION (and corresponding assignation) of a member on the fly for the express button
//TODOoptional: setDisabled on checkbox grays out the checkbox label. i want the label to stay colored but the checkbox to gray out. could split into checkbox and label ez, or perhaps setReadOnly is my friend?
//TODOreq: dialog should never validate when neither signal nor slot is checked
//TODOoptional: when the signal and slot messages have identical arguments, we should pre-select the drop-down "fill in args" combo boxes (but of course, still allow them to be changed)
//TODOreq: Properties show up in the "fill in the arguments" combo boxes
//TODOoptional: selecting an existing slot from combo box adds selection to list + resets combo box to the default. We re-use the combo box instead of having a new combo box for every slot invocation. Might change my mind on this though and have each slot BE a combo box
//TODOreq: the first time you use the "quick assign instance" on an installation (ie, per 'user' settings (once they exist)), you are asked if you want to make that quick assign action the button's default action (the typical "ok" [WITHOUT quick assign instance] becomes an option in the toolbutton's menu now, they swapped places basically). goes without saying that you should be able to undo (and also just plain do) this in the options menu (once it exists)
//TODOreq: quick assign instance should contain "Ok", "Ok and create and assign instance...", and "Ok and assign instance...". The latter two have sub-menus (no default action) that finish off the "...". That is to say, two options under "Ok and create and assign instance..." could be "as child of sender named m_Foo0" (this one is already implemented) and "as child of sender with name of my choosing" (QInputDialog). The sub-menu of "Ok and assign instance..." should list existing instances, but to keep things nice and organized we could make them choose the "parent" class first, and the parent's sub-menu lists its existing instances. When listing the classes that are available for parenting in the "Ok and assign instance..." menu, the sender should be listed at the top.
//TODOreq: ^related to above-ish, the instance creating/choosing should be merged into a common backend. Sure create/assign is just two steps, but it's better to re-use a method that does that rather than do those two actions myself whenever relevant. The instance chooser dialog can/should also inherit the functionality described in the above req, namedly a drill-down "tree" view for selecting parents/instances.
//TODOoptional: "local" variables defined in "chunk of raw c++" can be used as signal emit args (or invoke method args (etc)), if I use libclang intelligently.... but eh fuck that for now, for now "chunk of raw c++" must modify a member in order for it to be used in a signal emit/slot invoke/etc
//TODOreq: dynamic tab ordering. So for example in the most common case we are focused on the "signal line edit" right when the dialog opens. If they type something in here, then press tab, the place focus is transferred to should be the "slot line edit". But if they leave the "signal line edit" empty before pressing tab, the focus should be trasferred to the "existing signals combo box" (and the same functionality happens below for the "existing slots combo box" if "slot line edit" is empty when tab is pressed. Should be noted that a tab press when"existing signals combo box" is focused should still move focus to "slot line edit" (since whether or not we use an existing signal has no bearing on whether or not we're going to use an existing slot)
SignalSlotMessageDialog::SignalSlotMessageDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode, DesignEqualsImplementationClassSlot *destinationSlotToInvoke_OrZeroIfNoDest, bool sourceIsActor, bool destinationIsActor, DesignEqualsImplementationClassLifeLine *sourceClassLifeLine_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLine *destinationClassLifeLine_OrZeroIfNoDest, DesignEqualsImplementationClassSlot *sourceSlot_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    //, m_UnitOfExecutionContainingSlotToInvoke(unitOfExecutionContainingSlotToInvoke) //TODOreq: it's worth noting that the unit of execution is only the DESIRED unit of execution, and that it might not be invokable from the source unit of execution (at the time of writing, that is actor... so... lol)
    , m_Layout(new QVBoxLayout())
    , m_SourceSlot_OrZeroIfSourceIsActor(sourceSlot_OrZeroIfSourceIsActor)
    , m_DestinationSlot_OrZeroIfNoDest(destinationSlotToInvoke_OrZeroIfNoDest)
    , m_SignalToEmit(0)
    , m_SlotToInvoke(0)
    , m_SourceIsActor(sourceIsActor)
    , m_DestinationIsActor(destinationIsActor)
    , m_SignalArgsFillingInWidget(0)
    , m_SlotArgsFillingInWidget(0)
    , m_SourceClassLifeline_OrZeroIfSourceIsActor(sourceClassLifeLine_OrZeroIfSourceIsActor)
    , m_DestinationClassLifeline_OrZeroIfNoDest(destinationClassLifeLine_OrZeroIfNoDest)
    , m_SignalIsAlreadyPlacedInUseCaseGraphicsScene(sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement != 0)
    , m_SignalResultType(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction)
    , m_SlotResultType(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction)
{
    setWindowTitle(tr("Signal/Slot Message"));
    //Note: our use of the dialogMode excludes UseCaseSlotEventType altogether (it is used in the backend)

    //TODOoptional: combo boxes can be editable, but after thinking about it briefly I think adding a new slot like that would require that libclang interaction.. so for now KISS and just do existing slots (or add new slot with different

    //Layout placement

    //Signals
    QGroupBox *signalGroupBox = new QGroupBox();
    QHBoxLayout *signalHLayout = new QHBoxLayout();
    m_SignalsCheckbox = new QCheckBox(tr("Signal")); //TODOreq: the signal checkbox is itself disabled when in UseCaseSignalSlotEventType, forcing the signals to be enabled. TODOreq: checkbox hovertext when disabled should explain why disabled

    signalHLayout->addWidget(m_SignalsCheckbox);

    m_SignalsWidget = new QWidget();
    QVBoxLayout *signalsLayout = new QVBoxLayout();

    //new signal
    //QLineEdit *autoParsedSignalNameWithAutoCompleteForExistingSignals = new QLineEdit(); //TODOreq. TODOreq: "create handler in slot" (checks Slot checkbox if needed). ex: "handleFooSignal". When pressed, focus should change to the slot signature and have that "handleFooSignal" highlighted so they can type in a better name. It saves them from having to type the arguments portion of the signature
    //existing signal
    m_ExistingSignalsComboBox = new ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot();
    if(!m_ExistingSignalsComboBox->isInitialized())
    {
        setDisabled(true);
        //return;
    }
    //TODOreq: for both signals and slots, make the NAME of it bold (args are less important). This should be anywhere the signature is seen
    //signalsLayout->addWidget(autoParsedSignalNameWithAutoCompleteForExistingSignals);
    signalsLayout->addWidget(m_ExistingSignalsComboBox);

    signalHLayout->addWidget(m_SignalsWidget);
    m_SignalsWidget->setLayout(signalsLayout);
    QVBoxLayout *signalVLayout = new QVBoxLayout();
    signalVLayout->addLayout(signalHLayout);
    signalGroupBox->setLayout(signalVLayout);
    m_Layout->addWidget(signalGroupBox);

    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType)
        m_Layout->addWidget(new QWidget(), 1); //TODOreq: spacer inserted to space slots and space away from each other when in signal mode, verify it worked

    //Slots
    QGroupBox *slotGroupBox = new QGroupBox();
    QHBoxLayout *slotHLayout = new QHBoxLayout();
    m_SlotsCheckbox = new QCheckBox(tr("Slot"));
    m_SlotsCheckbox->setChecked(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType);
    slotHLayout->addWidget(m_SlotsCheckbox);

    m_SlotsWidget = new QWidget();
    QVBoxLayout *slotsLayout = new QVBoxLayout();

    //new slot
    //QLineEdit *autoParsedSlotNameWithAutoCompleteForExistingSlots = new QLineEdit(); //TODOreq
    //existing slot
    m_ExistingSlotsComboBox = new ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot();
    if(!m_ExistingSlotsComboBox->isInitialized())
    {
        setDisabled(true);
        //return; //TODOreq: leaks memory. all the widgets above this line not yet added to a layout, and even the layouts themselves which haven't been set to a widget. "no naked new"
    }
    //TODOreq: qlistwidget only takes one click insead of two (precious seconds when you're trying to keep a complicated design in your head)... but the trade off is that it takes up more space (not really though, once the combo box is expanded...)

    //slotsLayout->addWidget(autoParsedSlotNameWithAutoCompleteForExistingSlots);
    slotsLayout->addWidget(m_ExistingSlotsComboBox);

    slotHLayout->addWidget(m_SlotsWidget);
    m_SlotsWidget->setLayout(slotsLayout);
    slotGroupBox->setLayout(slotHLayout);
    m_Layout->addWidget(slotGroupBox);

    QHBoxLayout *cancelOkRow = new QHBoxLayout();

    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    if(sourceSlot_OrZeroIfSourceIsActor && destinationClassLifeLine_OrZeroIfNoDest && destinationClassLifeLine_OrZeroIfNoDest->instanceType() == DesignEqualsImplementationClassLifeLine::NoInstanceChosen) //"ok" + quick assign instance tool button(s?)
    {
        QToolButton *okToolButton = new QToolButton(this);
        QAction *okAction = new QAction(tr("Ok"), this);
        QString okAndMakeChildOfSignalSenderText = "Ok and make this instance of " + destinationSlotToInvoke_OrZeroIfNoDest->ParentClass->ClassName + " a child member of " + sourceSlot_OrZeroIfSourceIsActor->ParentClass->ClassName + " named: " + sourceSlot_OrZeroIfSourceIsActor->ParentClass->autoNameForNewChildMemberOfType(destinationSlotToInvoke_OrZeroIfNoDest->ParentClass);
        QAction *okAndMakeChildOfSignalSenderAction = new QAction(okAndMakeChildOfSignalSenderText, this); //TODOoptional: IDEALLY there'd be another action in the toolbutton to let you choose the member name too))
        okToolButton->setDefaultAction(okAction);
        okToolButton->addAction(okAndMakeChildOfSignalSenderAction);
        okToolButton->setPopupMode(QToolButton::MenuButtonPopup);
        m_OkButton = okToolButton;
        connect(okAction, SIGNAL(triggered()), this, SLOT(askUserWhatToDoWithNewArgTypesInNewSignalOrSlotsDeclarationIfAny_then_jitMaybeCreateSignalAndOrSlot_then_setSignalSlotResultPointersAsAppropriate_then_acceptDialog()));
        connect(okAndMakeChildOfSignalSenderAction, SIGNAL(triggered()), this, SLOT(handleOkAndMakeChildOfSignalSenderActionTriggered()));
    }
    else //regular ok button
    {
        QPushButton *okPushButton = new QPushButton(tr("Ok"), this); //TODOreq: button stays below arg filling in;
        connect(okPushButton, SIGNAL(clicked()), this, SLOT(askUserWhatToDoWithNewArgTypesInNewSignalOrSlotsDeclarationIfAny_then_jitMaybeCreateSignalAndOrSlot_then_setSignalSlotResultPointersAsAppropriate_then_acceptDialog()));
        okPushButton->setDefault(true);
        m_OkButton = okPushButton;
    }
    m_OkButton->setDisabled(true);

    cancelOkRow->addWidget(cancelButton);
    cancelOkRow->addWidget(m_OkButton, 1);

    m_Layout->addLayout(cancelOkRow);


    //Layout visibility and poulating

    m_ExistingSignalsComboBox->lineEdit()->setPlaceholderText(tr("Select existing signal, or declare new signal..."));
    m_ExistingSlotsComboBox->lineEdit()->setPlaceholderText(tr("Select existing slot, or declare new slot..."));

    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalEventType)
    {
        //no destination context, so they'd have to select an object/lifeline/unit-of-exectution or create one on the fly TODOreq
        m_SignalsCheckbox->setChecked(true);
        m_SignalsCheckbox->setDisabled(true);
        m_SignalsWidget->setDisabled(false);
        m_SignalsCheckbox->setToolTip(tr("You can't NOT have a signal when you don't have a destination"));
        m_SlotsCheckbox->setDisabled(true);
        m_SlotsCheckbox->setToolTip(tr("You can't have a slot when you don't have a destination")); //TODOreq: create destination+slot on the fly
        m_SlotsWidget->setDisabled(true);
    }
    else
    {
        m_SignalsWidget->setDisabled(true);
        m_SlotsCheckbox->setChecked(true);
        m_SlotsCheckbox->setDisabled(true);
        m_SlotsCheckbox->setToolTip(tr("You have a destination object, so you must have a slot")); //TODOreq: actor as destination breaks rule bleh (don't think it matters because actor uses UseCaseSignalEventType?)
        m_SlotsWidget->setDisabled(false);
        if(destinationSlotToInvoke_OrZeroIfNoDest) //I think this is impied != 0 when not UseCaseSignalEventType, however I'm not sure of it and the check is cheap and prevents segfault lolol
        {
            if(sourceSlot_OrZeroIfSourceIsActor && (!sourceIsActor))
            {
                if(sourceClassLifeLine_OrZeroIfSourceIsActor)
                {
                    if(sourceClassLifeLine_OrZeroIfSourceIsActor->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline)
                    {
#if 0
                        if(sourceClassLifeLine_OrZeroIfSourceIsActor && sourceClassLifeLine_OrZeroIfSourceIsActor->instanceType() != DesignEqualsImplementationClassLifeLine::NoInstanceChosen)
                        {
                            //either root or child of some other, we know at least that our instance is chosen, which means we are capable of having children. we have a list of children to check against that we know is valid (but perhaps empty)
#endif

#if 0 //applicable but unsolved -- not even sure there's a problem, but pretty sure there is -- i need to demonstrate the problem in a way that doesn't result in an infinite parent/child relationship loop, perhaps using zed. bleh maybe not a problem at all actually. after staring/thinking about it for a long time, the only time it can occur is when there is the infinite parent/child relationship loop (might be wrong) -- so teh answer is that the UI should not allow such infinite parent/child relationship loops to begin with. when the user tries to do it, we should qmessagebox::critical explaining why they can't do that. 'illegal operation' or whatever
                            bool signalMandatory = false;

                            /* TODOreq BUG
                            Unable to validate signal/slot dialog when doing:
                            Actor -> Foo::fooSlot -> Bar::barSlot (instance = Foo::m_Bar) -> Foo::blahFooSlot (instance = Foo::m_Bar::m_BarsFoo). Both of those arrows are (or want to be) plain slot invokes.
                            The reason is because Foo hasA Bar, so it thinks Bar is reporting back to the first foo.. in which case yes a signal is required. BUT since it's a different instance of foo (and in fact, Bar hasA that other instance of Foo), the signal should not be necessary. The "if a hasA b and b sends message to a, require signal" logic is not utilizing instance information, only class type, which is an error

                            funny i think i solved this once already and it's a regression since the instancing refactor xD
                            my head hurts i'm not sure this is a problem. if foo hasa bar hasa foo, aren't there infinite foos and bars? TODOreq: maybe i should disallow such infinite loop but idfk
                            */

                            if(destinationClassLifeLine_OrZeroIfNoDest) //pre-precondition xD
                            {
                                if(
                                        destinationClassLifeLine_OrZeroIfNoDest->instanceType() == DesignEqualsImplementationClassLifeLine::UseCasesRootClassLifeline //precondition to allow us to do non-instance-aware check for determining whether or not signal is mandatory
                                        && sourceClassLifeLine_OrZeroIfSourceIsActor->instanceInOtherClassIfApplicable()->parentClass() == destinationSlotToInvoke_OrZeroIfNoDest->ParentClass  //we can only check the types are the same (dest has no 'instance in other class' because it is root), but that's good enough in this case
                                        )
                                {
                                    signalMandatory = true;
                                }
                                else if(
                                        destinationClassLifeLine_OrZeroIfNoDest->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline //precondition to allow us to do instance-aware check for determining whether or not a signal is mandatory. my nose hurts. painfully itchy
                                        && destinationClassLifeLine_OrZeroIfNoDest->designEqualsImplementationClass()->hasA_Private_Classes_Members().contains(sourceClassLifeLine_OrZeroIfSourceIsActor->instanceInOtherClassIfApplicable()) //destination hasA this-source-instance, not just this-source-TYPE -- TODOreq: this STILL doesn't fix it because there are two instances of Foo and therefore two instances of Bar, but just like long ago we have no way of differentiating between those two instances of Bar gg, fucking instancing refactor fucked everything up again, so confusing
                                        )
                                {
                                    signalMandatory = true;
                                }
                                else if(true)
                                {
                                    //TODOreq: what do i do for the NoInstanceChosen case? anything? nothing?
                                }
                            }

                            if(signalMandatory)
#endif
                            if(sourceClassLifeLine_OrZeroIfSourceIsActor->instanceInOtherClassIfApplicable()->parentClass() == destinationSlotToInvoke_OrZeroIfNoDest->ParentClass)
                            {
                                //when the connection-activatation-line _destination_(slot) is parent (hasA relationship) of the _source_(signal), a signal is mandatory in the signal/slot message dialog before the dialog can be accepted. a slot is also mandatory since there is a destination, but that is already implemenented(implied?)
                                m_SignalsCheckbox->setChecked(true);
                                m_SignalsCheckbox->setDisabled(true);
                                m_SignalsWidget->setDisabled(false);
                                m_SignalsCheckbox->setToolTip(tr("When the destination-object hasA the source-object, a signal is mandatory"));
                            }

                            //if two copies of bar have the same parent (foo), connecting a line between the two bars requires a signal
                            if(
                                    destinationClassLifeLine_OrZeroIfNoDest && !destinationIsActor && destinationClassLifeLine_OrZeroIfNoDest->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline //precondition

                                    && destinationClassLifeLine_OrZeroIfNoDest->designEqualsImplementationClass() == sourceClassLifeLine_OrZeroIfSourceIsActor->designEqualsImplementationClass() //type (two copies of bar)

                                    && destinationClassLifeLine_OrZeroIfNoDest->instanceInOtherClassIfApplicable()->parentClass() == sourceClassLifeLine_OrZeroIfSourceIsActor->instanceInOtherClassIfApplicable()->parentClass() //instance (same parent)
                                    )
                            {
                                m_SignalsCheckbox->setChecked(true);
                                m_SignalsCheckbox->setDisabled(true);
                                m_SignalsWidget->setDisabled(false);
                                m_SignalsCheckbox->setToolTip(tr("When connecting two members of the same type, the connection statement must go in the owner of said two members. Hence, a signal is required for one member to communicate with the other")); //TODOreq: slot required to, but that is implied already since there is a dest
                            }
                        }
                        else if(sourceClassLifeLine_OrZeroIfSourceIsActor->instanceType() == DesignEqualsImplementationClassLifeLine::NoInstanceChosen)
                        {
                            //source does not have instance, so warn them that if they don't select a signal (they only select slot invoke), dest must be made a child of source before source can be generated
                            m_SignalsCheckbox->setChecked(true);
                            m_SignalsWidget->setDisabled(false);
                            m_SignalsCheckbox->setText(m_SignalsCheckbox->text() + "*");

                            QHBoxLayout *warningRow = new QHBoxLayout();
                            QLabel *warningLabel = new QLabel(tr("* = WARNING: Your source class lifeline has not been assigned an instance, so it is highly recommended that you use a signal and do not use a plain slot invoke. If you do choose slot invoke without a signal, know that you must make the destination class lifeline a child of the source class lifeline (source must hasA dest as it's member). Failing to do this will lead to undefined results"));
                            warningLabel->setWordWrap(true);
                            warningRow->addWidget(warningLabel); //TODOoptional: only show the warning when the signal checkbox is unchecked. Hide it again when signal checkbox is re-checked

                            QPushButton *chooseSourceInstanceButton = new QPushButton(tr("Choose Source Instance"));
                            warningRow->addWidget(chooseSourceInstanceButton);
                            signalVLayout->addLayout(warningRow);

                            connect(chooseSourceInstanceButton, SIGNAL(clicked()), this, SLOT(handleChooseSourceInstanceButtonClicked()));

                            m_SourceClassLifeline_OrZeroIfSourceIsActor = sourceClassLifeLine_OrZeroIfSourceIsActor;
                        }
                    }

#if 0

                if(destinationClassLifeLine_OrZeroIfNoDest && destinationClassLifeLine_OrZeroIfNoDest->instanceType() == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline )//precondition that it dest is a child at all
                {
                    //if(dest)
                }

                Q_FOREACH(HasA_Private_Classes_Member *currentHasA_Private_Classes_Member, destinationSlotToInvoke_OrZeroIfNoDest->ParentClass->hasA_Private_Classes_Members())
                {

                }

                //if(destinationSlotToInvoke_OrZeroIfNoDest->ParentClass->containsHasA_Private_Classes_Members().contains())
#endif

            }

            Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, destinationSlotToInvoke_OrZeroIfNoDest->ParentClass->mySlots()) //List the slots on the target slot, derp
            {
                if(currentSlot->Name.startsWith(UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX))
                    continue;
                m_ExistingSlotsComboBox->addItem(currentSlot->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSlot));
            }
            m_ExistingSlotsComboBox->insertKnownTypes(destinationSlotToInvoke_OrZeroIfNoDest->ParentClass->m_ParentProject->allKnownTypes());
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
        if(!sourceSlot_OrZeroIfSourceIsActor)
        {
            //hack that i put this here, it's where a segfault occured :)
            QMessageBox::information(this, tr("Error"), tr("Source must be connected/named/entered first")); //TODOreq: the source should be [defined as] just an unnamed-as-of-yet  "interface invocation" (the interface and the invocation upon it can both be nameless at any time, except when generating code (an auto-namer util could be used for those who can't be fucked TODOoptional))
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
            return;
        }

        if(sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement)
        {
            //TODOreq: show the existing signal in a read-only combo-box (so it doesn't need to be full or even function). if they wish to choose a different signal, they need to cancel (i guess this doesn't need to apply if there are no other slots connected to this signal. but when there are other signals, it would be insane to allow us to choose a different signal for all of those slots too)
            m_ExistingSignalsComboBox->addItem(sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement->methodSignatureWithoutReturnType(), QVariant::fromValue(sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement)); //TODOreq: i think i need a [returned] flag for knowing after accept() has been called that the signal the dialog retrieved was an existing signal. It will be propagated to the backend differently
            //m_ExistingSignalsComboBox->setCurrentIndex(m_ExistingSignalsComboBox->count()-1);
            int signalIndex = m_ExistingSignalsComboBox->count()-1;
            QMetaObject::invokeMethod(m_ExistingSignalsComboBox, "setCurrentIndex", Qt::QueuedConnection, Q_ARG(int, signalIndex)); //queued because we haven't connected to it and will do so later in this slot. we want the listener to do it's magic, especially the equivalent of this: m_SignalToEmit = sourceExistingSignalStatement_OrZeroIfSourceIsNotExistingSignalStatement;
            m_ExistingSignalsComboBox->setDisabled(true);
            m_SignalsCheckbox->setChecked(true);
            m_SignalsCheckbox->setDisabled(true);
            m_SignalsCheckbox->setToolTip(tr("You can't change the signal when connecting from an existing signal"));
            m_ExistingSignalsComboBox->setToolTip(tr("You can't change the signal when connecting from an existing signal"));

            //TODOoptional: remove the already selected slots (since it might be the same dest class lifeline) from the exist slots combo box? i'm actually leaning towards NO to this, if they want one signal to call the same slot twice, so be it
        }
        else
        {
            Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, sourceSlot_OrZeroIfSourceIsActor->ParentClass->mySignals())
            {
                m_ExistingSignalsComboBox->addItem(currentSignal->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSignal));
            }
            m_ExistingSignalsComboBox->insertKnownTypes(sourceSlot_OrZeroIfSourceIsActor->ParentClass->m_ParentProject->allKnownTypes());

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
#if 0 //TODOoptional: properties? i seem to have invented like 20 different variants of the same thing, so that decision is pending the refactor/consolidation
            Q_FOREACH(IHaveTypeAndVariableNameAndPreferredTextualRepresentation *currentHasAPod, sourceSlot_OrZeroIfSourceIsActor->ParentClass->hasA_Private_PODorNonDesignedCpp_Members())
            {
                m_VariablesAvailableToSatisfyArgs.append(currentHasAPod);
            }
#endif
        }
    }

    if(destinationIsActor)
    {
        m_SlotsCheckbox->setChecked(false);
        m_SlotsCheckbox->setDisabled(true);
        m_SlotsWidget->setDisabled(true);
        m_SlotsCheckbox->setToolTip(tr("You can't choose a slot when the destination is actor"));
    }

    //Connections
    if(messageEditorDialogMode == DesignEqualsImplementationUseCase::UseCaseSignalSlotEventType)
    {
        //Can't disable signals in other mode, so no point in listening
        connect(m_SignalsCheckbox, SIGNAL(toggled(bool)), m_SignalsWidget, SLOT(setEnabled(bool)));
        connect(m_SignalsCheckbox, SIGNAL(toggled(bool)), this, SLOT(handleSignalCheckboxToggled(bool)));

        //No slot in other mode, so no point in listening
        connect(m_SlotsCheckbox, SIGNAL(toggled(bool)), m_SlotsWidget, SLOT(setEnabled(bool)));
        connect(m_SlotsCheckbox, SIGNAL(toggled(bool)), this, SLOT(handleSlotCheckboxToggled(bool)));
        //connect(m_ExistingSlotsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExistingSlotsComboBoxCurrentIndexChanged(int)));
        //connect(m_ExistingSlotsComboBox, SIGNAL(resultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)), this, SLOT(handleSlotsComboBoxResultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)));
        //connect(m_ExistingSlotsComboBox, SIGNAL(syntaxIsValidChanged(bool)), this, SLOT(handleSlotsComboBoxSyntaxIsValidChanged(bool)));
        //TODOreq: whenever the parsed function name or args changes we collapse and re-whow the arg fillers. signals combo box does same. we can't rely on "syntax is valid" and "result type changed" to give us the most up to date parsing results, for example "someSlot0" would get NewResult/ValidSyntax on the first "s". Of course, the "parsed X changed" signals do imply NewResult and ValidSyntax.
        //connect(m_ExistingSlotsComboBox, SIGNAL(parsedFunctionNameChanged(QString)), this, SLOT(handleParsedSlotNameChanged(QString)));
        //connect(m_ExistingSlotsComboBox, SIGNAL(parsedFunctionArgumentsChanged(QList<MethodArgumentTypedef>)), this, SLOT(handleParsedSlotArgumentsChanged(QList<MethodArgumentTypedef>)));
        connect(m_ExistingSlotsComboBox, SIGNAL(selectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)), this, SLOT(handleSelectedSlotChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)));
    }

    //connect(m_ExistingSignalsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExistingSignalComboBoxIndexChanged(int)));
    //connect(m_ExistingSignalsComboBox, SIGNAL(resultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)), this, SLOT(handleSignalsComboBoxResultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)));
    //connect(m_ExistingSignalsComboBox, SIGNAL(syntaxIsValidChanged(bool)), this, SLOT(handleSignalsComboBoxSyntaxIsValidChanged(bool)));
    //connect(m_ExistingSignalsComboBox, SIGNAL(parsedFunctionNameChanged(QString)), this, SLOT(handleParsedSignalNameChanged(QString)));
    //connect(m_ExistingSignalsComboBox, SIGNAL(parsedFunctionArgumentsChanged(QList<MethodArgumentTypedef>)), this, SLOT(handleParsedSignalArgumentsChanged(QList<MethodArgumentTypedef>)));
    connect(m_ExistingSignalsComboBox, SIGNAL(selectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)), this, SLOT(handleSelectedSignalChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    setLayout(m_Layout);

    if(m_SignalsWidget->isEnabled())
        m_ExistingSignalsComboBox->setFocus();
    else if(m_SlotsWidget->isEnabled())
        m_ExistingSlotsComboBox->setFocus();

    //connect(this, SIGNAL(accepted()), this, SLOT(askUserWhatToDoWithNewArgTypesInNewSignalOrSlotsDeclarationIfAny_then_jitMaybeCreateSignalAndOrSlot_then_setSignalSlotResultPointersAsAppropriate_then_acceptDialog()));
}
DesignEqualsImplementationClassSignal *SignalSlotMessageDialog::signalToEmit_OrZeroIfNone() const
{
    return m_SignalToEmit;
}
DesignEqualsImplementationClassSlot *SignalSlotMessageDialog::slotToInvoke_OrZeroIfNone() const
{
    return m_SlotToInvoke;
}
SignalEmissionOrSlotInvocationContextVariables SignalSlotMessageDialog::slotInvocationContextVariables() const
{
    //Doesn't do validation checking. The dialog returning Accepted does though
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables;
    Q_FOREACH(QComboBox *currentArg, m_AllArgSatisfiers)
    {
        slotInvocationContextVariables.append(qvariant_cast<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*>(currentArg->currentData())->VariableName);
    }
    return slotInvocationContextVariables;
}
bool SignalSlotMessageDialog::signalIsAlreadyPlacedInUseCaseGraphicsScene() const
{
    //to determine when connecting a new slot to an existing signal
    return m_SignalIsAlreadyPlacedInUseCaseGraphicsScene;
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
    if(m_SignalArgumentsBeingFilledIn.isEmpty())
    {
        if(m_SlotNameHavingArgsFilledIn.isEmpty())
            m_OkButton->setDisabled(false); //signal emit with no listeners
        return;
    }

    m_SignalArgsFillingInWidget = new QWidget();
    QVBoxLayout *argsFillingInLayout = new QVBoxLayout(); //TODOreq: a scroll bar may be needed if the slot has too many args, but really 10 is a decent soft limit that Qt uses also... any more and you suck at designing :-P
    argsFillingInLayout->addWidget(new QLabel(QObject::tr("Fill in the arguments for: ") + m_SignalNameHavingArgsFilledIn), 0, Qt::AlignLeft);
    Q_FOREACH(MethodArgumentTypedef currentArgument, m_SignalArgumentsBeingFilledIn)
    {
        QHBoxLayout *currentArgRow = new QHBoxLayout();
        DesignEqualsImplementationClassMethodArgument arg(currentArgument.second);
        arg.Type = currentArgument.first;
        currentArgRow->addWidget(new QLabel(arg.preferredTextualRepresentationOfTypeAndVariableTogether()));
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

    maybeShowSlotArgFillingInUsingAppropriateComboBoxValues();
}
void SignalSlotMessageDialog::collapseSignalArgFillingIn()
{
    if(m_SignalArgsFillingInWidget)
    {
        m_AllArgSatisfiers.clear();
        delete m_SignalArgsFillingInWidget;
        m_SignalArgsFillingInWidget = 0;
    }
    maybeShowSlotArgFillingInUsingAppropriateComboBoxValues(); //what was "slot arg match up with signal arg" becomes "choose context variables to satisfy slot args"
}
void SignalSlotMessageDialog::maybeShowSlotArgFillingInUsingAppropriateComboBoxValues()
{
    if((!m_SlotsCheckbox->isChecked()) || (!m_ExistingSlotsComboBox->syntaxIsValid()) || m_SlotResultType == ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction)
    {
        collapseSlotArgFillingIn();
        return;
    }
    showSlotArgFillingIn();
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
    if(m_SlotsCheckbox->isChecked() && m_SlotResultType != ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction && m_SlotArgumentsBeingFilledIn.isEmpty())
        return;

    if(!m_SignalsCheckbox->isChecked())
    {
        m_SlotArgsFillingInWidget = new QWidget();
        QVBoxLayout *argsFillingInLayout = new QVBoxLayout(); //TODOreq: a scroll bar may be needed if the slot has too many args, but really 10 is a decent soft limit that Qt uses also... any more and you suck at designing :-P
        argsFillingInLayout->addWidget(new QLabel(QObject::tr("Fill in the arguments for: ") + m_SlotNameHavingArgsFilledIn), 0, Qt::AlignLeft);
        Q_FOREACH(MethodArgumentTypedef currentArgument, m_SlotArgumentsBeingFilledIn)
        {
            QHBoxLayout *currentArgRow = new QHBoxLayout(); //TODOoptimization: one grid layout instead? fuck it
            DesignEqualsImplementationClassMethodArgument arg(currentArgument.second);
            arg.Type = currentArgument.first;
            currentArgRow->addWidget(new QLabel(arg.preferredTextualRepresentationOfTypeAndVariableTogether()));
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
        if((m_SignalResultType == ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction) || (!m_ExistingSignalsComboBox->syntaxIsValid()) || (m_SlotResultType == ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction) || (!m_ExistingSlotsComboBox->syntaxIsValid()))
            return false;
        //TODOreq: verify signal/slot arg compatibility, return true if checks out (TODOoptional: highlight in red the slot so they know it's not the arg satisfiers)
        if(m_SignalArgumentsBeingFilledIn.size() < m_SlotArgumentsBeingFilledIn.size())
            return false; //TODOmb: notify them of why the dialog can't be accept()'d
        int currentArgIndex = 0;
        Q_FOREACH(MethodArgumentTypedef currentSlotArgument, m_SlotArgumentsBeingFilledIn)
        {
            const QByteArray &slotArgTypeCstr = currentSlotArgument.first.toUtf8();
            const QByteArray &signalArgTypeCstr = m_SignalArgumentsBeingFilledIn.at(currentArgIndex++).first.toUtf8();
            if(QMetaObject::normalizedType(slotArgTypeCstr.constData()) != QMetaObject::normalizedType(signalArgTypeCstr.constData())) //TODOoptional: maybe QMetaObject::checkConnectArgs handles inheritence stuff for me? I might need to register the to-be-generated types at runtime for it to work? Pretty sure libclang has "inherits or is type" TODOlater
            {
                return false;
            }
        }
        return true;
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
    {
        m_SignalNameHavingArgsFilledIn.clear();
        m_SignalArgumentsBeingFilledIn.clear();
        m_ExistingSignalsComboBox->setEditText("");
        m_ExistingSignalsComboBox->setCurrentIndex(0);
        collapseSignalArgFillingIn();
    }
}
void SignalSlotMessageDialog::handleSlotCheckboxToggled(bool checked)
{
    if(!checked)
    {
        m_SlotNameHavingArgsFilledIn.clear();
        m_SlotArgumentsBeingFilledIn.clear();
        m_ExistingSlotsComboBox->setEditText("");
        m_ExistingSlotsComboBox->setCurrentIndex(0);
        collapseSlotArgFillingIn();
    }
}
void SignalSlotMessageDialog::handleSelectedSignalChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType)
{
    m_SignalResultType = resultType;
    switch(resultType)
    {
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction:
        m_SignalNameHavingArgsFilledIn.clear();
        m_SignalArgumentsBeingFilledIn.clear();
        collapseSignalArgFillingIn();
        m_OkButton->setDisabled(true);
    break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ExistingFunction:
    {
        collapseSignalArgFillingIn();
        DesignEqualsImplementationClassSignal *selectedSignal = qvariant_cast<DesignEqualsImplementationClassSignal*>(m_ExistingSignalsComboBox->itemData(m_ExistingSignalsComboBox->currentIndex()));
        m_SignalNameHavingArgsFilledIn = selectedSignal->Name;
        m_SignalArgumentsBeingFilledIn = selectedSignal->argumentsAsMethodArgumentTypedefList();
        showSignalArgFillingIn();
        tryValidatingDialog();
    }
    break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction:
    {
        collapseSignalArgFillingIn();
        m_SignalNameHavingArgsFilledIn = m_ExistingSignalsComboBox->parsedFunctionName();
        m_SignalArgumentsBeingFilledIn = m_ExistingSignalsComboBox->parsedFunctionArguments();
        if(m_ExistingSignalsComboBox->syntaxIsValid())
        {
            showSignalArgFillingIn();
            tryValidatingDialog();
        }
        else
            m_OkButton->setDisabled(true);
    }
    break;
    }
}
void SignalSlotMessageDialog::handleSelectedSlotChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType)
{
    m_SlotResultType = resultType;
    switch(resultType)
    {
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction:
        m_SlotNameHavingArgsFilledIn.clear();
        m_SlotArgumentsBeingFilledIn.clear();
        collapseSlotArgFillingIn();
        m_OkButton->setDisabled(true);
    break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ExistingFunction:
    {
        collapseSlotArgFillingIn();
        DesignEqualsImplementationClassSlot *selectedSlot = qvariant_cast<DesignEqualsImplementationClassSlot*>(m_ExistingSlotsComboBox->itemData(m_ExistingSlotsComboBox->currentIndex()));
        m_SlotNameHavingArgsFilledIn = selectedSlot->Name;
        m_SlotArgumentsBeingFilledIn = selectedSlot->argumentsAsMethodArgumentTypedefList();
        showSlotArgFillingIn();
        tryValidatingDialog();
    }
    break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction:
    {
        collapseSlotArgFillingIn();
        m_SlotNameHavingArgsFilledIn = m_ExistingSlotsComboBox->parsedFunctionName();
        m_SlotArgumentsBeingFilledIn = m_ExistingSlotsComboBox->parsedFunctionArguments();
        if(m_ExistingSlotsComboBox->syntaxIsValid())
        {
            showSlotArgFillingIn();
            tryValidatingDialog();
        }
        else
            m_OkButton->setDisabled(true);
    }
    break;
    }
}
//TODOreq: this applies to slots too... but say they type in a second arg after typing the first and "filling in" the first. atm we'd be clearing in their filling in work every time they change ANY aspect of an arg (type, name, num args, etc)... which is stupid ofc... but KISS
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
void SignalSlotMessageDialog::handleChooseSourceInstanceButtonClicked() //corner-case
{
    ClassInstanceChooserDialog classInstanceChooserDialog(m_SourceClassLifeline_OrZeroIfSourceIsActor); //m_SourceClassLifeline_OrZeroIfSourceIsActor is already known to be valid or else we'd never get here
    if(classInstanceChooserDialog.exec() == QDialog::Accepted)
    {
        //TODOreq: re-do the visualization stuff (for example, the warning/button that brought is here would probably now be hidden since they've chosen an instance). i suppose i need "reset" and "setupVisuals" methods

        //TODOoptional: pre-Generate-source-code sanitization. I am only hesitant to do that stage of sanitization because eventually there hopefully won't be the requirement of objects being children of each other in order to communicate (the source would still need a pointer to dest, even if he doesn't own dest)
    }
}
void SignalSlotMessageDialog::handleOkAndMakeChildOfSignalSenderActionTriggered()
{
    if(!askUserWhatToDoWithNewArgTypesInNewSignalOrSlotsDeclarationIfAny_then_jitMaybeCreateSignalAndOrSlot_then_setSignalSlotResultPointersAsAppropriate_then_acceptDialog())
        return; //if they hit "cancel" n the new types dialog, they have to re-request the "make child of sender" thing

    //the toolbutton to get here wouldn't be shown if source is actor or if there's no dest
    DesignEqualsImplementationClass *sourceClass = m_SourceSlot_OrZeroIfSourceIsActor->ParentClass;
    HasA_Private_Classes_Member *newHasAmember = sourceClass->createHasA_Private_Classes_Member(m_DestinationSlot_OrZeroIfNoDest->ParentClass);
    m_DestinationClassLifeline_OrZeroIfNoDest->setInstanceInOtherClassIfApplicable(newHasAmember);
    //accept();
}
bool SignalSlotMessageDialog::askUserWhatToDoWithNewArgTypesInNewSignalOrSlotsDeclarationIfAny_then_jitMaybeCreateSignalAndOrSlot_then_setSignalSlotResultPointersAsAppropriate_then_acceptDialog()
{
    //syntax is already known to be valid, otherwise we never would have gotten here because this slot was invoked by the accepted() signal

    //if using signal and result type == new, jit create the d=i signal, then set m_SignalToEmit pointer to it. ez pz

    //but first, create or mark as defined elsewhere, any new types seen in signal/slot args
    DesignEqualsImplementationProject *currentProject;
    QList<QString> newSignalOrSlotArgTypesSeen;
    if(m_SignalsCheckbox->isChecked() && m_SignalResultType == ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction && (!m_ExistingSignalsComboBox->newTypesSeenInParsedFunctionDeclaration().isEmpty()))
    {
        currentProject = m_SourceClassLifeline_OrZeroIfSourceIsActor->designEqualsImplementationClass()->m_ParentProject;
        newSignalOrSlotArgTypesSeen.append(m_ExistingSignalsComboBox->newTypesSeenInParsedFunctionDeclaration());
    }
    else if(m_SlotsCheckbox->isChecked() && m_SlotResultType == ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction && (!m_ExistingSlotsComboBox->newTypesSeenInParsedFunctionDeclaration().isEmpty()))
    {
        currentProject = m_DestinationClassLifeline_OrZeroIfNoDest->designEqualsImplementationClass()->m_ParentProject;
        newSignalOrSlotArgTypesSeen.append(m_ExistingSlotsComboBox->newTypesSeenInParsedFunctionDeclaration());
    }
    if(!newSignalOrSlotArgTypesSeen.isEmpty())
    {
        NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(newSignalOrSlotArgTypesSeen, currentProject, this);
        if(newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog.exec() != QDialog::Accepted)
        {
            return false;
        }
    }

    if(m_SignalsCheckbox->isChecked())
    {
        switch(m_SignalResultType)
        {
        case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction:
            m_SignalToEmit = m_SourceClassLifeline_OrZeroIfSourceIsActor->designEqualsImplementationClass()->createNewSignal(m_ExistingSignalsComboBox->parsedFunctionName(), m_ExistingSignalsComboBox->parsedFunctionArguments());
        break;
        case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ExistingFunction:
            m_SignalToEmit = qvariant_cast<DesignEqualsImplementationClassSignal*>(m_ExistingSignalsComboBox->itemData(m_ExistingSignalsComboBox->currentIndex()));
        break;
        case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction:
            m_SignalToEmit = 0;
            qFatal("The signals/slots message dialog was accept()'d with the signal checkbox checked but no signal selected");
            return false;
        break;
        }
    }
    else
        m_SignalToEmit = 0;

    //do same thing for slot
    if(m_SlotsCheckbox->isChecked())
    {
        switch(m_SlotResultType)
        {
        case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction:
            m_SlotToInvoke = m_DestinationClassLifeline_OrZeroIfNoDest->designEqualsImplementationClass()->createwNewSlot(m_ExistingSlotsComboBox->parsedFunctionName(), m_ExistingSlotsComboBox->parsedFunctionArguments());
        break;
        case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ExistingFunction:
            m_SlotToInvoke = qvariant_cast<DesignEqualsImplementationClassSlot*>(m_ExistingSlotsComboBox->itemData(currentm_ExistingSlotsComboBox->currentIndex()));
        break;
        case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction:
            m_SlotToInvoke = 0;
            qFatal("The signals/slots message dialog was accept()'d with the slot checkbox checked but no slot selected");
            return false;
        break;
        }
    }
    else
        m_SlotToInvoke = 0;

    accept();
    return true;
}
