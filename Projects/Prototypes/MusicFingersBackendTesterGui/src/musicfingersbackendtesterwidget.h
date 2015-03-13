#ifndef MUSICFINGERSBACKENDTESTERWIDGET_H
#define MUSICFINGERSBACKENDTESTERWIDGET_H

#include <QWidget>
#include <QHash>

#include "musicfingers.h"

class QSlider;

class MusicFingersBackendTesterWidget : public QWidget
{
    Q_OBJECT
public:
    MusicFingersBackendTesterWidget(QWidget *parent = 0);
private:
    QHash<QSlider*, Finger::FingerEnum> m_Fingers;
    void createSlider(QLayout *layoutToAddTo, Finger::FingerEnum theFinger);
signals:
    void fingerMoved(Finger::FingerEnum finger, int newPosition);
private slots:
    void handleSliderMoved(int newPosition);
};

#endif // MUSICFINGERSBACKENDTESTERWIDGET_H
