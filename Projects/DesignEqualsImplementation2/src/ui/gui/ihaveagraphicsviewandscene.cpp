#include "ihaveagraphicsviewandscene.h"

#ifndef QT_NO_OPENGL
    #include <QtOpenGL/QGLWidget>
#endif
#include <QVBoxLayout>
#include <QGraphicsView>

#include "designequalsimplementationguicommon.h"

//TODOoptional: Qt makes this easy, should allow ability to pop out additional windows (graphics views of same scene). It would maybe be helpful to have one at "zoomed out" and primary/working-one all zoomed in and shit... multiple monitors etc etc. So you don't lose sight of the big picture ;-P
IHaveAGraphicsViewAndScene::IHaveAGraphicsViewAndScene(QGraphicsScene *graphicsScene, QWidget *parent)
    : QWidget(parent)
    , m_GraphicsScene(graphicsScene)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->setContentsMargins(DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS, DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS, DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS, DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS);
    m_GraphicsScene->setParent(this);
    //m_GraphicsScene->setSceneRect(QRectF(0, 0, 5000, 5000));
    QGraphicsView *graphicsView = new QGraphicsView(m_GraphicsScene);
#ifndef QT_NO_OPENGL //TODOoptional: perhaps a run-time switch for gl that is only available if open gl was available at compile time
    graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    myLayout->addWidget(graphicsView, 1);
    setLayout(myLayout);
}
