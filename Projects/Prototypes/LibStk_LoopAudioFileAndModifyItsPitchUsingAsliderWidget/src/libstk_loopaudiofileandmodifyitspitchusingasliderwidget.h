#ifndef LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
#define LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H

#include <QWidget>

#include "stk/Stk.h"

class QSlider;

class LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget : public QWidget
{
    Q_OBJECT
public:
    LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent = 0);
private:
    static qreal map(qreal valueToMap, qreal sourceRangeMin, qreal sourceRangeMax, qreal destRangeMin, qreal destRangeMax);
    void showStdStringError(const std::string &stkError);
    void setupGui();

    QSlider *m_PitchShiftSlider;
    QSlider *m_PitchShiftMixAmountSlider;
signals:
    void changePitchShiftRequested(stk::StkFloat newPitchShiftValue);
    void changePitchShiftMixAmountRequested(stk::StkFloat newPitchShiftMixAmount);
private slots:
    void handlePitchShiftSliderValueChanged(int newValue);
    void handlePitchShiftMixAmountSliderValueChanged(int newValue);
};

#endif // LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
