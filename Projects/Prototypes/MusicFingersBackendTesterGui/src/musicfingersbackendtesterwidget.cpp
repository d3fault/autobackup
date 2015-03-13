#include "musicfingersbackendtesterwidget.h"

#include <QHBoxLayout>
#include <QSlider>

#define NUM_FINGERS 10
//#define THE_BASE_OF_OUR_NUMBER_SYSTEM NUM_FINGERS


MusicFingersBackendTesterWidget::MusicFingersBackendTesterWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *myLayout = new QHBoxLayout();

    createSlider(myLayout, Finger::LeftPinky_A0);
    createSlider(myLayout, Finger::LeftRing_A1);
    createSlider(myLayout, Finger::LeftMiddle_A2);
    createSlider(myLayout, Finger::LeftIndex_A3);
    createSlider(myLayout, Finger::LeftThumb_A4);

    createSlider(myLayout, Finger::RightThumb_A5);
    createSlider(myLayout, Finger::RightIndex_A6);
    createSlider(myLayout, Finger::RightMiddle_A7);
    createSlider(myLayout, Finger::RightRing_A8);
    createSlider(myLayout, Finger::RightPinky_A9);

    setLayout(myLayout);
}
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
    slider->setValue(FINGERS_INITIAL_POSITION);
    m_Fingers.insert(slider, theFinger);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMoved(int)));
    layoutToAddTo->addWidget(slider);
}
