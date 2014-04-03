#include "mouseormotionormysexyfaceviewmakerwidget.h"

#include <QVBoxLayout>

MouseOrMotionOrMySexyFaceViewMakerWidget::MouseOrMotionOrMySexyFaceViewMakerWidget(const QSize &viewSize, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_ViewLabel = new QLabel();
    m_ViewLabel->resize(viewSize.width(), viewSize.height());
    myLayout->addWidget(m_ViewLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    setLayout(myLayout);
}
MouseOrMotionOrMySexyFaceViewMakerWidget::~MouseOrMotionOrMySexyFaceViewMakerWidget()
{ }
void MouseOrMotionOrMySexyFaceViewMakerWidget::presentPixmapForViewing(const QPixmap &tehPixmap)
{
    m_ViewLabel->setPixmap(tehPixmap);
}
