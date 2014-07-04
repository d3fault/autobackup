#include "usecaseumlitemswidget.h"

UseCaseUmlItemsWidget::UseCaseUmlItemsWidget(QWidget *parent)
    : QListWidget(parent)
{
    setDragEnabled(true);
    setViewMode(QListView::IconMode);
    //setIconSize
    setSpacing(5);
    setDropIndicatorShown(true);
}
