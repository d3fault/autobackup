#include "mouseandormotionviewmakerwidget.h"

#include <QVBoxLayout>

MouseAndOrMotionViewMakerWidget::MouseAndOrMotionViewMakerWidget(const QSize &viewSize, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_ViewLabel = new QLabel();
    m_ViewLabel->resize(viewSize.width(), viewSize.height());
    myLayout->addWidget(m_ViewLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    myLayout->setContentsMargins(0,0,0,0);
    setLayout(myLayout);
}
MouseAndOrMotionViewMakerWidget::~MouseAndOrMotionViewMakerWidget()
{ }
void MouseAndOrMotionViewMakerWidget::presentPixmapForViewing(const QPixmap &tehPixmap)
{
    m_ViewLabel->setPixmap(tehPixmap);
}
