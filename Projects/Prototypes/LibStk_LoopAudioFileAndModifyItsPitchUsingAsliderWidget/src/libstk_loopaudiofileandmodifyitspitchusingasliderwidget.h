#ifndef LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
#define LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H

#include <QWidget>

#include <QScopedPointer>

#include "stk/RtAudio.h"
#include "stk/FileLoop.h"
#include "stk/PitShift.h"
#include "stk/Envelope.h"

#define LIBSTK_TICK_METHOD_SIGNATURE (void *outputBuffer, void *unusedInputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)

class QSlider;

class LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget : public QWidget
{
    Q_OBJECT
public:
    LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent = 0);
    static int staticStkTick LIBSTK_TICK_METHOD_SIGNATURE;
    int stkTick LIBSTK_TICK_METHOD_SIGNATURE;
    ~LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget();
private:
    static qreal map(qreal valueToMap, qreal sourceRangeMin, qreal sourceRangeMax, qreal destRangeMin, qreal destRangeMax);
    void showStdStringError(const std::string &stkError);
    void setupGui();

    QSlider *m_PitchShiftSlider;
    QSlider *m_PitchShiftMixAmountSlider;
    unsigned int m_NumBufferFrames;
    QScopedPointer<RtAudio> dac;
    QScopedPointer<stk::FileLoop> input;
    QScopedPointer<stk::PitShift> shifter;
    QScopedPointer<stk::Envelope> envelope;
    stk::StkFrames frames;
    RtAudio::StreamParameters parameters;
private slots:
    void handlePitchShiftSliderValueChanged(int newValue);
    void handlePitchShiftMixAmountSliderValueChanged(int newValue);
};

#endif // LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
