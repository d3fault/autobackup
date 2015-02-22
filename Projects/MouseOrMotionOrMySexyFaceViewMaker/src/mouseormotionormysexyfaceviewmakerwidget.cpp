#include "mouseormotionormysexyfaceviewmakerwidget.h"

#include <QVBoxLayout>
#include <QMouseEvent>

MouseOrMotionOrMySexyFaceViewMakerWidget::MouseOrMotionOrMySexyFaceViewMakerWidget(const QSize &viewSize, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_ViewLabel = new QLabel();
    m_ViewLabel->resize(viewSize.width(), viewSize.height());
    myLayout->addWidget(m_ViewLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    myLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(myLayout);
}
MouseOrMotionOrMySexyFaceViewMakerWidget::~MouseOrMotionOrMySexyFaceViewMakerWidget()
{ }
void MouseOrMotionOrMySexyFaceViewMakerWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        emit setMySexyFaceViewModeRequested();
    }
    else if(e->button() == Qt::LeftButton)
    {
        emit setMouseOrMotionOrMySexyFaceViewModeRequested();
    }
    else if(e->button() == Qt::MiddleButton)
    {
        close();
    }
}
void MouseOrMotionOrMySexyFaceViewMakerWidget::presentPixmapForViewing(const QPixmap &tehPixmap)
{
    m_ViewLabel->setPixmap(tehPixmap);
}
