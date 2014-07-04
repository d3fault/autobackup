#include "umlitemswidget.h"

#include <QListWidgetItem>
#include <QDataStream>
#include <QMimeData>
#include <QDrag>

#include "../../designequalsimplementationcommon.h"
#include "designequalsimplementationguicommon.h"

UmlItemsWidget::UmlItemsWidget(QWidget *parent)
    : QListWidget(parent)
{
    //TODOreq: wondering how closely the backend/frontend objects can/should relate (can/should they be one?)

    setDragEnabled(true);
    setViewMode(QListView::IconMode); //TODOoptional: icon + static mode !?!?
    //setIconSize
    setSpacing(5);
    setDropIndicatorShown(true);

    QListWidgetItem *umlClass = new QListWidgetItem(tr("Class"), this);
    umlClass->setData(Qt::UserRole, QVariant(static_cast<UmlItemsTypedef>(DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS)));
    //umlClass->setIcon(QIcon(pixmap));
    umlClass->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

    QListWidgetItem *umlClassInterface = new QListWidgetItem(tr("Abstract Class (Interface)"), this);
    umlClassInterface->setData(Qt::UserRole, QVariant(static_cast<UmlItemsTypedef>(DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS_INTERFACE)));
    //umlClass->setIcon(QIcon(pixmap));
    umlClassInterface->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}
void UmlItemsWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QListWidgetItem *item = currentItem();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    UmlItemsTypedef umlItemType = qvariant_cast<UmlItemsTypedef>(item->data(Qt::UserRole));
    dataStream << umlItemType;
    QMimeData *mimeData = new QMimeData();
    mimeData->setData(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT, itemData);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    //drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    //drag->setPixmap(pixmap);
    drag->exec(Qt::LinkAction, Qt::LinkAction);
}
