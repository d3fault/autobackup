#ifndef SLOTINVOCATIONDIALOG_H
#define SLOTINVOCATIONDIALOG_H

#include <QDialog>

#include "../../designequalsimplementationusecase.h"
#include "../../signalemissionorslotinvocationcontextvariables.h"

class QVBoxLayout;
class QComboBox;
class QPushButton;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class IHaveTypeAndVariableNameAndPreferredTextualRepresentation;

class SlotInvocationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SlotInvocationDialog(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum slotInvocationDialogMode , DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke, bool sourceIsActor, DesignEqualsImplementationClassSlot *slotWithCurrentContext_OrZeroIfSourceIsActor, QWidget *parent = 0, Qt::WindowFlags f = 0);
    DesignEqualsImplementationClassSlot *slotToInvoke() const;
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecutionContainingSlotToInvoke;
    QVBoxLayout *m_Layout;
    QComboBox *m_ExistingSignalsComboBox;
    QComboBox *m_SlotsComboBox;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    QPushButton *m_OkButton;
    bool m_SourceIsActor;
    //DesignEqualsImplementationClassSlot *m_SlotWithCurrentContext_OrZeroIfSourceIsActor;
    QList<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*> m_VariablesAvailableToSatisfyArgs;
    QList<QComboBox*> m_AllArgSatisfiers;
    QWidget *m_ArgsFillingInWidget;

    void showSlotArgFillingIn();
    void collapseSlotArgFillingIn();

    bool allArgSatisfiersAreValid();
private slots:
    void handleSlotsComboBoxICurrentIndexChanged(int newIndex);
    void handleArgSatisfierChosen();
};

#endif // SLOTINVOCATIONDIALOG_H
