#include "musicfingersbackendtesterwidget.h"

#include <QHBoxLayout>
#include <QSlider>

#define NUM_FINGERS 10
//#define THE_BASE_OF_OUR_NUMBER_SYSTEM NUM_FINGERS


MusicFingersBackendTesterWidget::MusicFingersBackendTesterWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *myLayout = new QHBoxLayout();

    createSlider(myLayout, Finger::LeftPinky0);
    createSlider(myLayout, Finger::LeftRing1);
    createSlider(myLayout, Finger::LeftMiddl2);
    createSlider(myLayout, Finger::LeftIndex3);
    createSlider(myLayout, Finger::LeftThumb4);

    createSlider(myLayout, Finger::RightThumb5);
    createSlider(myLayout, Finger::RightIndex6);
    createSlider(myLayout, Finger::RightMiddle);
    createSlider(myLayout, Finger::RightRing7);
    createSlider(myLayout, Finger::RightPinky8);

    setLayout(myLayout);
}
MusicFingersBackendTesterWidget::~MusicFingersBackendTesterWidget()
{ }
void MusicFingersBackendTesterWidget::handleSliderMoved(int newPosition)
{
    QSlider *slider = qobject_cast<QSlider*>(sender());
    if(!slider)
        return;
    emit fingerMoved(m_Fingers.value(slider), newPosition);
}
void MusicFingersBackendTesterWidget::createSlider(QLayout *layoutToAddTo, Finger::FingerEnum theFinger)
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setMinimum(MUSIC_FINGERS_MIN_POSITION);
    slider->setMaximum(MUSIC_FINGERS_MAX_POSITION);
    m_Fingers.insert(slider, theFinger);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMoved(int)));
    layoutToAddTo->addWidget(slider);
}
