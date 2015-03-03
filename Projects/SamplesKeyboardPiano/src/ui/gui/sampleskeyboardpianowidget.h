#ifndef SAMPLESKEYBOARDPIANOWIDGET_H
#define SAMPLESKEYBOARDPIANOWIDGET_H

#include <QWidget>

class QKeyEvent;

class SamplesKeyboardPianoWidget : public QWidget
{
    Q_OBJECT
public:
    SamplesKeyboardPianoWidget(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *keyEvent, bool shiftKeyPressed);
signals:
    void alphabeticalKeyPressDetected(int keyboardKey, bool shiftKeyPressed);
};

#endif // SAMPLESKEYBOARDPIANOWIDGET_H
