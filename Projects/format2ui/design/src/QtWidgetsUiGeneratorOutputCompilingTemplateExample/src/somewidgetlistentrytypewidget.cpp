#include "somewidgetlistentrytypewidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"

SomeWidgetListEntryTypeWidget::SomeWidgetListEntryTypeWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout(this);

    m_AnEntry0LineEdit = new QLineEdit();
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::addLineEditToLayout("An Entry 0", m_AnEntry0LineEdit, myLayout);
    m_AnotherEntry1LineEdit = new QLineEdit();
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::addLineEditToLayout("Another Entry 1", m_AnotherEntry1LineEdit, myLayout);
}
QString SomeWidgetListEntryTypeWidget::anEntry0() const
{
    return m_AnEntry0LineEdit->text();
}
QString SomeWidgetListEntryTypeWidget::anotherEntry1() const
{
    return m_AnotherEntry1LineEdit->text();
}
