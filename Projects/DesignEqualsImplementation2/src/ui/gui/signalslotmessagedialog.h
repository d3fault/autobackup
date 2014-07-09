#ifndef SIGNALSLOTMESSAGEDIALOG_H
#define SIGNALSLOTMESSAGEDIALOG_H

#include <QDialog>

#include "../../designequalsimplementationusecase.h"
#include "../../signalemissionorslotinvocationcontextvariables.h"

class QVBoxLayout;
class QComboBox;
class QPushButton;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class IHaveTypeAndVariableNameAndPreferredTextualRepresentation;

class SignalSlotMessageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SignalSlotMessageDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum messageEditorDialogMode , DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke_OrZeroIfNoDest, bool sourceIsActor, DesignEqualsImplementationClassSlot *slotWithCurrentContext_OrZeroIfSourceIsActor, QWidget *parent = 0, Qt::WindowFlags f = 0);
    DesignEqualsImplementationClassSignal *signalToEmit_OrZeroIfNone() const;
    DesignEqualsImplementationClassSlot *slotToInvoke_OrZeroIfNone() const;
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecutionContainingSlotToInvoke;
    QVBoxLayout *m_Layout;
    QComboBox *m_ExistingSignalsComboBox;
    QComboBox *m_ExistingSlotsComboBox;
    DesignEqualsImplementationClassSignal *m_SignalToEmit;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    QPushButton *m_OkButton;
    bool m_SourceIsActor;
    //DesignEqualsImplementationClassSlot *m_SlotWithCurrentContext_OrZeroIfSourceIsActor;
    QList<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*> m_VariablesAvailableToSatisfyArgs;
    QList<QComboBox*> m_AllArgSatisfiers;
    QWidget *m_ArgsFillingInWidget;

    void showSignalArgFillingIn();
    void collapseSignalArgFillingIn();

    void showSlotArgFillingIn();
    void collapseSlotArgFillingIn();

    bool allArgSatisfiersAreValid();
private slots:
    void handleSignalCheckboxToggled(bool checked);
    void handleSlotCheckboxToggled(bool checked);
    void handleExistingSignalComboBoxIndexChanged(int newIndex);
    void handleExistingSlotsComboBoxICurrentIndexChanged(int newIndex);
    void handleArgSatisfierChosen();
};

#endif // SIGNALSLOTMESSAGEDIALOG_H
