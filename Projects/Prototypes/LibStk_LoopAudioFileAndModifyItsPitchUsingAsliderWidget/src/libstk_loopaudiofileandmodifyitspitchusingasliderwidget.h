#ifndef LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
#define LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H

#include <QSlider>

#include <QScopedPointer>

#include "stk/FileLoop.h"
#include "stk/RtAudio.h"

#define LIBSTKTICK_METHOD_SIGNATURE (void *outputBuffer, void *unusedInputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)

class LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget : public QSlider
{
    Q_OBJECT
public:
    LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent = 0);
    static int staticStkTick LIBSTKTICK_METHOD_SIGNATURE;
    int stkTick LIBSTKTICK_METHOD_SIGNATURE;
    ~LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget();
private:
    void showStdStringError(const std::string &stkError);

    QScopedPointer<RtAudio> dac;
    QScopedPointer<stk::FileLoop> input;
    stk::StkFrames frames;
    RtAudio::StreamParameters parameters;
private slots:
    void handleValueChanged(int newValue);
};

#endif // LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
