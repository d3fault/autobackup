#include "mouseandormotionviewmakerwidget.h"

#include <QVBoxLayout>

MouseAndOrMotionViewMakerWidget::MouseAndOrMotionViewMakerWidget(const QSize &viewSize, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_ViewLabel = new QLabel();
    m_ViewLabel->resize(viewSize.width(), viewSize.height());
    myLayout->addWidget(m_ViewLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    setLayout(myLayout);
}
MouseAndOrMotionViewMakerWidget::~MouseAndOrMotionViewMakerWidget()
{ }
void MouseAndOrMotionViewMakerWidget::presentPixmapForViewing(const QPixmap &tehPixmap)
{
    m_ViewLabel->setPixmap(tehPixmap);
}
