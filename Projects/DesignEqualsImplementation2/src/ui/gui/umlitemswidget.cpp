#include "umlitemswidget.h"

#include <QBoxLayout>
#include <QList>

UmlItemsWidget::UmlItemsWidget(QWidget *parent, QBoxLayout::Direction direction, int columnsIfVertical_RowsIfHorizontal)
    : QWidget(parent)
    , m_Layout(new QBoxLayout(direction))
{
    //TODOreq: wondering how closely the backend/frontend objects can/should relate (can/should they be one?)
    setLayout(m_Layout);
}
