#ifndef SHIFTCLICKABLEPUSHBUTTON_H
#define SHIFTCLICKABLEPUSHBUTTON_H

#include <QPushButton>

class ShiftClickablePushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ShiftClickablePushButton(int keyboardKey, QWidget *parent = 0);
private:
    int m_KeyboardKey;
protected:
    virtual bool eventFilter(QObject *object, QEvent *event);
signals:
    void alphabeticalKeyPressDetected(int keyboardKey, bool shiftKeyPressed);
};

#endif // SHIFTCLICKABLEPUSHBUTTON_H
