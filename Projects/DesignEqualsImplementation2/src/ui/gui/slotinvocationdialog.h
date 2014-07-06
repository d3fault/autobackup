#ifndef SLOTINVOCATIONDIALOG_H
#define SLOTINVOCATIONDIALOG_H

#include <QDialog>

#include "../../signalemissionorslotinvocationcontextvariables.h"

class QComboBox;
class QPushButton;

class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;

class SlotInvocationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SlotInvocationDialog(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke, QWidget *parent = 0, Qt::WindowFlags f = 0);
    DesignEqualsImplementationClassSlot *slotToInvoke() const;
    SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecutionContainingSlotToInvoke;
    QComboBox *m_SlotsComboBox;
    DesignEqualsImplementationClassSlot *m_SelectedSlotToInvoke;
    SignalEmissionOrSlotInvocationContextVariables m_SlotInvocationContextVariables;
    QPushButton *m_OkButton;
private slots:
    void handleSlotsComboBoxItemActivated(int newIndex);
};

#endif // SLOTINVOCATIONDIALOG_H
