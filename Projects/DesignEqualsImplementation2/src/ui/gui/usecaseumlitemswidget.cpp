#include "usecaseumlitemswidget.h"

#include <QDataStream>
#include <QMimeData>
#include <QDrag>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"

#define UseCaseUmlItemsWidget_ACTOR_STRING "Actor"

//TODOreq: while valgrinding (sloooow), I saw 2 copies of Foo in this widget. I did new project -> new use case (so there were two projects open, accounting for the two Foos). is race condition methinks
UseCaseUmlItemsWidget::UseCaseUmlItemsWidget(QWidget *parent)
    : QListWidget(parent)
{
    setDragEnabled(true);
    setViewMode(QListView::IconMode);
    //setIconSize
    setSpacing(5);
    setDropIndicatorShown(true);

    addActor();
}
void UseCaseUmlItemsWidget::startDrag(Qt::DropActions supportedActions)
{
    //OT/Random: HOWEVER i get the class into mimeData (thinking about it now), it would be cool regardless to provide the entire generated source for that class as a "text" mime, should they drag it onto a 3rd party notepad/etc ;-P. that idea derived from 'set class name as text' (for same purpose). but eh since we're defining use cases, the class is likely incomplete at the moment so eh the idea isn't that great. it would make more sense from the class diagram perspective TODOfun

    Q_UNUSED(supportedActions)
    QListWidgetItem *item = currentItem();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    bool isActor = qvariant_cast<bool>(item->data(Qt::UserRole));
    dataStream << isActor;
    QMimeData *mimeData = new QMimeData();
    if(!isActor)
    {
        DesignEqualsImplementationClass *classBeingAddedToUseCase = qvariant_cast<DesignEqualsImplementationClass*>(item->data(Qt::UserRole+1));
        dataStream << reinterpret_cast<quintptr>(classBeingAddedToUseCase); //TODOreq: ugly (but works). I don't think implicit sharing would help here, but really the fucking class name would be sufficient (would require refactor in backend, BUT even there class name is fine for identification (C++ spec demands that ;-P)
        QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
        mimeData->setText(classBeingAddedToUseCase->ClassName);
    }
    else
    {
        mimeData->setText(UseCaseUmlItemsWidget_ACTOR_STRING);
    }

    mimeData->setData(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT, itemData);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    //drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    //drag->setPixmap(pixmap);
    drag->exec(Qt::LinkAction, Qt::LinkAction);
}
void UseCaseUmlItemsWidget::addActor()
{
    QListWidgetItem *actor = new QListWidgetItem(tr(UseCaseUmlItemsWidget_ACTOR_STRING), this);
    actor->setData(Qt::UserRole, QVariant(true)); //TODOreq: false indicates that UserRole+1 contains a pointer to a class
    //umlClass->setIcon(QIcon(pixmap));
    actor->setFlags(DESIGNEQUALSIMPLEMENTATION_GUI_DRAG_DROP_LIST_WIDGET_ITEM_FLAGS);
    addItem(actor);
}
void UseCaseUmlItemsWidget::addClass(DesignEqualsImplementationClass *designEqualsImplementationClass)
{
    QListWidgetItem *newClassListWidgetItem = new QListWidgetItem(designEqualsImplementationClass->ClassName, this);
    newClassListWidgetItem->setData(Qt::UserRole, QVariant(false));
    newClassListWidgetItem->setData(Qt::UserRole+1, QVariant::fromValue<DesignEqualsImplementationClass*>(designEqualsImplementationClass));
    //umlClass->setIcon(QIcon(pixmap));
    newClassListWidgetItem->setFlags(DESIGNEQUALSIMPLEMENTATION_GUI_DRAG_DROP_LIST_WIDGET_ITEM_FLAGS);
    addItem(newClassListWidgetItem);
}
void UseCaseUmlItemsWidget::handleNowShowingProject(DesignEqualsImplementationProject *project)
{
    clear();
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, project->classes())
    {
        addClass(currentClass);
    }
}
void UseCaseUmlItemsWidget::handleClassAdded(DesignEqualsImplementationClass *designEqualsImplementationClass)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    addClass(designEqualsImplementationClass);
}
