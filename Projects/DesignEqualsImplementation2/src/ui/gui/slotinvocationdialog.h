#ifndef SLOTINVOCATIONDIALOG_H
#define SLOTINVOCATIONDIALOG_H

#include <QDialog>

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
    explicit SlotInvocationDialog(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke, bool sourceIsActor, DesignEqualsImplementationClassSlot *slotWithCurrentContext_OrZeroIfSourceIsActor, QWidget *parent = 0, Qt::WindowFlags f = 0);
    DesignEqualsImplementationClassSlot *slotToInvoke() const;
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecutionContainingSlotToInvoke;
    QVBoxLayout *m_Layout;
    QComboBox *m_SlotsComboBox;
    DesignEqualsImplementationClassSlot *m_SlotToInvoke;
    SignalEmissionOrSlotInvocationContextVariables m_SlotInvocationContextVariables;
    QPushButton *m_OkButton;
    bool m_SourceIsActor;
    //DesignEqualsImplementationClassSlot *m_SlotWithCurrentContext_OrZeroIfSourceIsActor;
    QList<IHaveTypeAndVariableNameAndPreferredTextualRepresentation*> m_VariablesAvailableToSatisfyArgs;
    QWidget *m_ArgsFillingInWidget;

    void showSlotArgFillingIn();
    void collapseSlotArgFillingIn();
private slots:
    void handleSlotsComboBoxItemActivated(int newIndex);
};

#endif // SLOTINVOCATIONDIALOG_H
