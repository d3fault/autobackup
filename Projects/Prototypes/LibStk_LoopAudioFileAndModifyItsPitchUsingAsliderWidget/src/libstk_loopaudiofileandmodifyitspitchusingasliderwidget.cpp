#include "libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h"

#include <QSlider>
#include <QHBoxLayout>
#include <QMessageBox>

#include <QDebug>

//TODOmb: a slider for volume and other shiz stk provides
LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent)
    : QWidget(parent)
{
    setupGui();
}
qreal LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::map(qreal valueToMap, qreal sourceRangeMin, qreal sourceRangeMax, qreal destRangeMin, qreal destRangeMax)
{
    //map 'valueToMap' from [sourceRangeMin-sourceRangeMax] to [destRangeMin-destRangeMax]
    qreal mappedValue = (valueToMap - sourceRangeMin) * (destRangeMax - destRangeMin) / (sourceRangeMax - sourceRangeMin) + destRangeMin; //jacked from Arduino/WMath.cpp , which in turn was jacked from Wiring. it's just math anyways (not copyrightable)
    return mappedValue;
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::showStdStringError(const std::string &stkError)
{
    QString stkErrorQString = QString::fromStdString(stkError);
    QMessageBox::critical(this, "StkError:", stkErrorQString);
    close();
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::setupGui()
{
    QHBoxLayout *myLayout = new QHBoxLayout(this);

    myLayout->addWidget(m_PitchShiftSlider = new QSlider());
    m_PitchShiftSlider->setRange(0, 1023);
    m_PitchShiftSlider->setValue((m_PitchShiftSlider->minimum() + m_PitchShiftSlider->maximum()) / 2);
    connect(m_PitchShiftSlider, &QSlider::valueChanged, this, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftSliderValueChanged);

    myLayout->addWidget(m_PitchShiftMixAmountSlider = new QSlider());
    m_PitchShiftMixAmountSlider->setRange(0, 100);
    m_PitchShiftMixAmountSlider->setValue((m_PitchShiftMixAmountSlider->minimum() + m_PitchShiftMixAmountSlider->maximum()) / 2);
    connect(m_PitchShiftMixAmountSlider, &QSlider::valueChanged, this, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftMixAmountSliderValueChanged);
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftSliderValueChanged(int newValue)
{
    //bleh all the PitShift docs say is that "1.0" has no pitch shift effect, doesn't tell me which direction to go or what range min/max I should map my 0-1023 values to. bah I guess trial and error (but if I don't change it ENOUGH, I won't hear shit!)
    qreal newValueAsFloat = static_cast<qreal>(newValue);

    //map 0-1023 -> 0.1-2
    qreal newValueMapped = map(newValueAsFloat, 0, 1023, 0.1, 2);

    emit changePitchShiftRequested(static_cast<stk::StkFloat>(newValueMapped));
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handlePitchShiftMixAmountSliderValueChanged(int newValue)
{
    qreal newValueAsFloat = static_cast<qreal>(newValue);

    //map 0-100 -> 0-1
    qreal newValueMapped = map(newValueAsFloat, 0, 100, 0, 1);

    emit changePitchShiftMixAmountRequested(static_cast<stk::StkFloat>(newValueMapped));
}
