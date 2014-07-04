#include "designequalsimplementationclassdiagramaswidgetfortab.h"

#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMutexLocker>
#include <QDropEvent>
#include <QMimeData>
#ifndef QT_NO_OPENGL
    #include <QtOpenGL/QGLWidget>
#endif

#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"
#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationclass.h"

//TODOreq: at the time of writing, the only two uml items I can think of for class diagrams are "interfaces" and "classes", heh. Fucking UML spec is so bloated. Oh actually after looking at Dia just now, I think "notes" are pretty handy too (but I want to have them more "attached" to various objects instead of just eh floating)... but notes are hardly UML xD. OT: weird in Dia there are two "implements a specific interface" lines, and idfk the difference...
//TODOoptional: "search", which highlights uml items if they start with what the user types in
DesignEqualsImplementationClassDiagramAsWidgetForTab::DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : QWidget(parent)
    , m_ClassDiagramScene(new QGraphicsScene(this))
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    QGraphicsView *graphicsView = new QGraphicsView(m_ClassDiagramScene);
#ifndef QT_NO_OPENGL //TODOoptional: perhaps a run-time switch for gl that is only available if open gl was available at compile time
    graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    graphicsView->setAcceptDrops(true);
    myLayout->addWidget(graphicsView, 1);
    setLayout(myLayout);
}
void DesignEqualsImplementationClassDiagramAsWidgetForTab::dropEvent(QDropEvent *event)
{
    if(event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_CLASS_DIAGRAM_OBJECT)) //TODOreq: not sure if this will be a problem, but two instances of the app should not be able to share designed classes (which are only visible when on use case tab anyways). may segfault, may do nothing, idfk. for class diagram view it doesn't matter though lol
    {
        //textBrowser->setPlainText(event->mimeData()->text());
        //TODOreq: emit a request to backend for class to be added at that point

        //TODOreq: at start up, backend should create "uml item factories" and emit them to front end. front end adds them to "uml items" dockwidget. the drag drop event specifies which factory (HOW? i don't think i can do a pointer), that is then emitted as argument here in "emit addClassRequested(UmlItemFactory*)"; since backend made it, backend must call it (frontend just holds pointer to it). IUmlFactory just contains 2 pure virtuals: umlItemName and newUmlItem....... of course i could just use a quint16 that is a key to a hash of factories instead if the pointer-in-qvariant method isn't determined :-P

        event->acceptProposedAction();
    }
}
void DesignEqualsImplementationClassDiagramAsWidgetForTab::handleClassAdded(DesignEqualsImplementationClass *classAdded)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene *designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene = new DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(classAdded); //scene takes ownership at addItem

    connect(classAdded, SIGNAL(propertyAdded(DesignEqualsImplementationClassProperty*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handlePropertyAdded(DesignEqualsImplementationClassProperty*)));
    connect(classAdded, SIGNAL(hasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handleHasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)));

    //LISTS!?!?

    //QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    //QList<DesignEqualsImplementationClassSlot*> Slots;
    //QList<DesignEqualsImplementationClassSignal*> Signals;

    m_ClassDiagramScene->addItem(designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene); //TODOreq: coordinates. TODOoptimization: when open existing project, perhaps addItem should be delayed until after the backend says "all project details emitted", so that tons of repaints aren't triggered. there's lots of solutions to this problem however
    QMetaObject::invokeMethod(classAdded, "emitAllClassDetails");
}
