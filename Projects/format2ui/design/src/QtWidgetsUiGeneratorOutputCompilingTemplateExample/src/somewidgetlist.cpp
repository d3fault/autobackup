/*format2ui-import-as-triggered-sdlfjsdofuo08340832408934*/
#include "somewidgetlist.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"
#include "somewidgetlistentrytypewidget.h"

SomeWidgetList::SomeWidgetList(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayoutAndAddNewRowButtonBelow = new QVBoxLayout(this);

    m_MyLayout = new QVBoxLayout();
    addNewRow();

    myLayoutAndAddNewRowButtonBelow->addLayout(m_MyLayout);
    QPushButton *addNewRowButton = new QPushButton(tr("Add New Row"));
    myLayoutAndAddNewRowButtonBelow->addWidget(addNewRowButton);

    connect(addNewRowButton, &QPushButton::clicked, this, &SomeWidgetList::addNewRow);
}
QList<SomeWidgetListEntryType> SomeWidgetList::someWidgetListValues() const
{
    QList<SomeWidgetListEntryType> ret;
    const QList<SomeWidgetListEntryTypeWidget*> &constSomeWidgetListEntryTypeWidgets = m_SomeWidgetListEntryTypeWidgets;
    for(auto &&i : constSomeWidgetListEntryTypeWidgets)
    {
        SomeWidgetListEntryType entryData;
        entryData.SomeWidgetList_AnEntry0 = i->anEntry0();
        entryData.SomeWidgetList_AnotherEntry1 = i->anotherEntry1();
        ret << entryData;
    }
    return ret;
}
void SomeWidgetList::addNewRow()
{
    QVBoxLayout *someWidgetListEntryTypeLayout = new QVBoxLayout();
    int currentNum = m_SomeWidgetListEntryTypeWidgets.size();
    QString humanReadableWithNum("Some Widget List #" + QString::number(currentNum));
    QLabel *label = new QLabel(humanReadableWithNum);
    someWidgetListEntryTypeLayout->addWidget(label);

    SomeWidgetListEntryTypeWidget *someWidgetListEntryWidget = new SomeWidgetListEntryTypeWidget();
    someWidgetListEntryTypeLayout->addWidget(someWidgetListEntryWidget);
    m_SomeWidgetListEntryTypeWidgets.append(someWidgetListEntryWidget);
#if 0
    QLineEdit *chocolate0ALineEdit = new QLineEdit();
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::addLineEditToLayout("Chocolate 0A", chocolate0ALineEdit, someWidgetListEntryTypeLayout);
    QLineEdit *vanilla1BLineEdit = new QLineEdit();
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::addLineEditToLayout("Vanilla 1B", vanilla1BLineEdit, someWidgetListEntryTypeLayout);
#endif

#if 0
    m_Top5MoviesLineEdits.append(lineEdit);
#endif

    m_MyLayout->addLayout(someWidgetListEntryTypeLayout);
}
