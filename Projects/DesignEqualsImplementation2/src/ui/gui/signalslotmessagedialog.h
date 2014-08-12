#ifndef SIGNALSLOTMESSAGEDIALOG_H
#define SIGNALSLOTMESSAGEDIALOG_H

#include <QDialog>

#include "../../designequalsimplementationusecase.h"
#include "../../signalemissionorslotinvocationcontextvariables.h"

class QVBoxLayout;
class QCheckBox;
class QComboBox;
class QPushButton;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLine;
//class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class IHaveTypeAndVariableNameAndPreferredTextualRepresentation;

class SignalSlotMessageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SignalSlotMessageDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode , DesignEqualsImplementationClassSlot *destinationSlotToInvoke_OrZeroIfNoDest, bool sourceIsActor,  bool destinationIsActor, DesignEqualsImplementationClassLifeLine *sourceClassLifeLine_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLine *destinationClassLifeLine_OrZeroIfNoDest, DesignEqualsImplementationClassSlot *sourceSlot_OrZeroIfSourceIsActor, QWidget *parent = 0, Qt::WindowFlags f = 0);
    DesignEqualsImplementationClassSignal *signalToEmit_OrZeroIfNone() const;
    DesignEqualsImplementationClassSlot *slotToInvoke_OrZeroIfNone() const;
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecutionContainingSlotToInvoke;
    QVBoxLayout *m_Layout;
    QCheckBox *m_SignalsCheckbox;
    QCheckBox *m_SlotsCheckbox;
    QComboBox *m_ExistingSignalsComboBox;
    QComboBox *m_ExistingSlotsComboBox;
    DesignEqualsImplementationClassSignal *m_SignalToEmit;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    QPushButton *m_OkButton;
    bool m_SourceIsActor;
    bool m_DestinationIsActor;
    //DesignEqualsImplementationClassSlot *m_SlotWithCurrentContext_OrZeroIfSourceIsActor;
    QList<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*> m_VariablesAvailableToSatisfyArgs;
    QList<QComboBox*> m_AllArgSatisfiers;
    QWidget *m_SignalArgsFillingInWidget;
    QWidget *m_SlotArgsFillingInWidget;

    void showSignalArgFillingIn();
    void collapseSignalArgFillingIn();

    void showSlotArgFillingIn();
    void collapseSlotArgFillingIn();

    bool allArgSatisfiersAreValid();
    //bool allSlotsArgsMatchedUpWithSignalArgsIfSlotEvenChecked();
private slots:
    void handleSignalCheckboxToggled(bool checked);
    void handleSlotCheckboxToggled(bool checked);
    void handleExistingSignalComboBoxIndexChanged(int newIndex);
    void handleExistingSlotsComboBoxCurrentIndexChanged(int newIndex);
    void tryValidatingDialog();
};

#endif // SIGNALSLOTMESSAGEDIALOG_H
