#include "mouseandormotionviewmakerwidget.h"

#include <QVBoxLayout>

MouseAndOrMotionViewMakerWidget::MouseAndOrMotionViewMakerWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_ViewLabel = new QLabel();
    m_ViewLabel->resize(800, 600);
    myLayout->addWidget(m_ViewLabel);
    setLayout(myLayout);
}
MouseAndOrMotionViewMakerWidget::~MouseAndOrMotionViewMakerWidget()
{

}
void MouseAndOrMotionViewMakerWidget::handleD(const QString &msg)
{

}
void MouseAndOrMotionViewMakerWidget::presentPixmapForViewing(const QPixmap &tehPixmap)
{
    m_ViewLabel->setPixmap(tehPixmap);
}
