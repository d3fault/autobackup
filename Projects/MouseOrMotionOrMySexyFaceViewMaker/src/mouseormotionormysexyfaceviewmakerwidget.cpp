#include "mouseormotionormysexyfaceviewmakerwidget.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QApplication>

#define MOMOMSF_TITLE_SUFFIX " - MOMOMSF"
#define MOMOMSF_TITLE_DEFAULT_AUTO "A" MOMOMSF_TITLE_SUFFIX
#define MOMOMSF_TITLE_MY_SEXY_FACE "SF" MOMOMSF_TITLE_SUFFIX
#define MOMOMSF_TITLE_MSF_FROZEN_ERROR "~ERROR~" MOMOMSF_TITLE_SUFFIX

MouseOrMotionOrMySexyFaceViewMakerWidget::MouseOrMotionOrMySexyFaceViewMakerWidget(const QSize &viewSize, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
{
    setWindowTitle(MOMOMSF_TITLE_DEFAULT_AUTO);
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_ViewLabel = new QLabel();
    m_ViewLabel->resize(viewSize.width(), viewSize.height());
    myLayout->addWidget(m_ViewLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    myLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(myLayout);
}
void MouseOrMotionOrMySexyFaceViewMakerWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        if(windowTitle() != MOMOMSF_TITLE_MSF_FROZEN_ERROR)
            setWindowTitle(MOMOMSF_TITLE_MY_SEXY_FACE);

        emit setMySexyFaceViewModeRequested();
    }
    else if(e->button() == Qt::LeftButton)
    {
        if(windowTitle() != MOMOMSF_TITLE_MSF_FROZEN_ERROR)
            setWindowTitle(MOMOMSF_TITLE_DEFAULT_AUTO);
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
void MouseOrMotionOrMySexyFaceViewMakerWidget::handleMySexyFaceStreamIsFrozen()
{
    setWindowTitle(MOMOMSF_TITLE_MSF_FROZEN_ERROR);
    QApplication::alert(this);
}
