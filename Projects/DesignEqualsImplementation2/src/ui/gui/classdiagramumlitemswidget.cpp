#include "classdiagramumlitemswidget.h"

#include <QListWidgetItem>
#include <QDataStream>
#include <QMimeData>
#include <QDrag>

#include "../../designequalsimplementationcommon.h"
#include "designequalsimplementationguicommon.h"

//TODOreq: double-clicking a class adds it to the far right (?). maybe this idea sucks. maybe double clicking brings to already added one if there and nothing else? or combination of these two ideas, or none! double clicking a class (ANYWHERE) should bring up class editor rarararara! (lol at umbrello having random ass color config editor in some views when double clicking a class -_-)
ClassDiagramUmlItemsWidget::ClassDiagramUmlItemsWidget(QWidget *parent)
    : QListWidget(parent)
{
    //TODOreq: wondering how closely the backend/frontend objects can/should relate (can/should they be one?)

    setDragEnabled(true); //TODOreq: OT: all use cases click-n-drag onto any graphics scene = open that use case in tab
    setViewMode(QListView::IconMode); //TODOoptional: icon + static mode !?!?
    //setIconSize
    setSpacing(5);
    setDropIndicatorShown(true);

    QListWidgetItem *umlClassInterface = new QListWidgetItem(tr("Abstract Class (Interface)"), this);
    umlClassInterface->setData(Qt::UserRole, QVariant(static_cast<UmlItemsTypedef>(DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS_INTERFACE)));
    //umlClass->setIcon(QIcon(pixmap));
    umlClassInterface->setFlags(DESIGNEQUALSIMPLEMENTATION_GUI_DRAG_DROP_LIST_WIDGET_ITEM_FLAGS);

    QListWidgetItem *umlClass = new QListWidgetItem(tr("Class"), this);
    umlClass->setData(Qt::UserRole, QVariant(static_cast<UmlItemsTypedef>(DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS)));
    //umlClass->setIcon(QIcon(pixmap));
    umlClass->setFlags(DESIGNEQUALSIMPLEMENTATION_GUI_DRAG_DROP_LIST_WIDGET_ITEM_FLAGS);

    QListWidgetItem *implicitlySharedDataType = new QListWidgetItem(tr("Implicitly Shared Data Type"), this);
    implicitlySharedDataType->setData(Qt::UserRole, QVariant(static_cast<UmlItemsTypedef>(DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_IMPLICITLY_SHARED_DATA_TYPE))); //TODOoptional: would have been much smarter to store Type* factories in the data, but whatever
    implicitlySharedDataType->setFlags(DESIGNEQUALSIMPLEMENTATION_GUI_DRAG_DROP_LIST_WIDGET_ITEM_FLAGS);
}
void ClassDiagramUmlItemsWidget::startDrag(Qt::DropActions supportedActions)
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
