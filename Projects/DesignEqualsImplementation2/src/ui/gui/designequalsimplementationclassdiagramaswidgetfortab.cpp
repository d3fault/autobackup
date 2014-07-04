#include "designequalsimplementationclassdiagramaswidgetfortab.h"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QMutexLocker>
#ifndef QT_NO_OPENGL
    #include <QtOpenGL/QGLWidget>
#endif

#include "designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h"
#include "classdiagramgraphicsscene.h"
#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationproject.h"
#include "../../designequalsimplementationclass.h"

//TODOreq: at the time of writing, the only two uml items I can think of for class diagrams are "interfaces" and "classes", heh. Fucking UML spec is so bloated. Oh actually after looking at Dia just now, I think "notes" are pretty handy too (but I want to have them more "attached" to various objects instead of just eh floating)... but notes are hardly UML xD. OT: weird in Dia there are two "implements a specific interface" lines, and idfk the difference...
//TODOoptional: "search", which highlights uml items if they start with what the user types in
DesignEqualsImplementationClassDiagramAsWidgetForTab::DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : QWidget(parent)
    , m_ClassDiagramScene(new ClassDiagramGraphicsScene(this))
{
    connect(m_ClassDiagramScene, SIGNAL(addUmlItemRequested(UmlItemsTypedef,QPointF)), designEqualsImplementationProject, SLOT(handleAddUmlItemRequested(UmlItemsTypedef,QPointF)));

    QVBoxLayout *myLayout = new QVBoxLayout();
    QGraphicsView *graphicsView = new QGraphicsView(m_ClassDiagramScene);
#ifndef QT_NO_OPENGL //TODOoptional: perhaps a run-time switch for gl that is only available if open gl was available at compile time
    graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    myLayout->addWidget(graphicsView, 1);
    setLayout(myLayout);
}
void DesignEqualsImplementationClassDiagramAsWidgetForTab::handleClassAdded(DesignEqualsImplementationClass *classAdded)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene *designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene = new DesignEqualsImplementationClassAsQGraphicsItemForClassDiagramScene(classAdded); //scene takes ownership at addItem
    designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene->setPos(classAdded->Position);

    connect(classAdded, SIGNAL(propertyAdded(DesignEqualsImplementationClassProperty*)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handlePropertyAdded(DesignEqualsImplementationClassProperty*)));
    connect(classAdded, SIGNAL(hasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)), designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene, SLOT(handleHasAPrivateMemberClassAdded(HasA_PrivateMemberClasses_ListEntryType)));

    //LISTS!?!?

    //QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    //QList<DesignEqualsImplementationClassSlot*> Slots;
    //QList<DesignEqualsImplementationClassSignal*> Signals;

    m_ClassDiagramScene->addItem(designEqualsImplementationClassAsQGraphicsItemForClassDiagramScene); //TODOreq: coordinates. TODOoptimization: when open existing project, perhaps addItem should be delayed until after the backend says "all project details emitted", so that tons of repaints aren't triggered. there's lots of solutions to this problem however
    QMetaObject::invokeMethod(classAdded, "emitAllClassDetails");
}
