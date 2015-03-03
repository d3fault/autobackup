#ifndef SAMPLESKEYBOARDPIANOWIDGET_H
#define SAMPLESKEYBOARDPIANOWIDGET_H

#include <QWidget>

class QKeyEvent;

class SamplesKeyboardPianoWidget : public QWidget
{
    Q_OBJECT
public:
    SamplesKeyboardPianoWidget(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *keyEvent);
signals:
    void alphabeticalKeyPressDetected(int keyboardKey, bool shiftKeyPressed);
};

#endif // SAMPLESKEYBOARDPIANOWIDGET_H
