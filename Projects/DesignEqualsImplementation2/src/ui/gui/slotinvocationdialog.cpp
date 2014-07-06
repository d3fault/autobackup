#include "slotinvocationdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>

#include "../../designequalsimplementationclassslot.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclasslifeline.h"
#include "../../designequalsimplementationclasslifelineunitofexecution.h"

SlotInvocationDialog::SlotInvocationDialog(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecutionContainingSlotToInvoke, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    //, m_UnitOfExecutionContainingSlotToInvoke(unitOfExecutionContainingSlotToInvoke) //TODOreq: it's worth noting that the unit of execution is only the DESIRED unit of execution, and that it might not be invokable from the source unit of execution (at the time of writing, that is actor... so... lol)
    , m_SelectedSlotToInvoke(0)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    //TODOoptional: combo box can be editable, but after thinking about it briefly I think adding a new slot like that would require that libclang interaction.. so for now KISS and just do existing slots (or add new slot with different line edits for args etc)

    m_SlotsComboBox = new QComboBox();
    m_SlotsComboBox->addItem(tr("Select slot...")); //TODOreq: qlistwidget only takes one click insead of two (precious seconds when you're trying to keep a complicated design in your head)... but the trade off is that it takes up more space (not really though, once the combo box is expanded...)
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, unitOfExecutionContainingSlotToInvoke->designEqualsImplementationClassLifeLine()->designEqualsImplementationClass()->Slots)
    {
        m_SlotsComboBox->addItem(currentSlot->methodSignatureWithoutReturnType(), QVariant::fromValue(currentSlot));
    }
    connect(m_SlotsComboBox, SIGNAL(activated(int)), this, SLOT(handleSlotsComboBoxItemActivated(int)));

    QHBoxLayout *cancelOkRow = new QHBoxLayout();

    m_OkButton = new QPushButton(tr("Ok"), this);
    m_OkButton->setDefault(true);
    m_OkButton->setDisabled(true); //TODOreq: only ever enabled when dialog contents are (had:selected and) valid

    connect(m_OkButton, SIGNAL(clicked()), this, SLOT(accept()));

    cancelOkRow->addWidget(m_OkButton);

    myLayout->addWidget(m_SlotsComboBox);
    myLayout->addLayout(cancelOkRow);

    setLayout(myLayout);
}
DesignEqualsImplementationClassSlot *SlotInvocationDialog::slotToInvoke() const
{
    return m_SelectedSlotToInvoke;
}
SignalEmissionOrSlotInvocationContextVariables SlotInvocationDialog::slotInvocationContextVariables() const
{
    return m_SlotInvocationContextVariables;
}
void SlotInvocationDialog::handleSlotsComboBoxItemActivated(int newIndex)
{
    if(newIndex == 0)
    {
        m_OkButton->setDisabled(true);
        return;
    }

    m_OkButton->setDisabled(false);
    m_SelectedSlotToInvoke = qvariant_cast<DesignEqualsImplementationClassSlot*>(m_SlotsComboBox->itemData(newIndex));

    //TODOreq: present selectable context variables for the slot args. This is required before OK is enabled, but not when it's the first/actor->slotInvoke (grayed out in that case)
}
