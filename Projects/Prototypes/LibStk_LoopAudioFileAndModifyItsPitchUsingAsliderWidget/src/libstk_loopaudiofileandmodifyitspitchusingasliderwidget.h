#ifndef LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
#define LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H

#include <QWidget>

#include <QScopedPointer>

#include "stk/FileLoop.h"

class QSlider;

class StkFileLoopIoDevice;

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
    QScopedPointer<stk::FileLoop> input;
    unsigned int m_NumBufferFrames;
    StkFileLoopIoDevice *m_FileLoopIoDevice;
private slots:
    void handlePitchShiftSliderValueChanged(int newValue);
    void handlePitchShiftMixAmountSliderValueChanged(int newValue);
};

#endif // LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDERWIDGET_H
