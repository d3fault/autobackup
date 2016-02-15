#ifndef LINEEDITTHATIGNORESKEYPRESSEDEVENTS_H
#define LINEEDITTHATIGNORESKEYPRESSEDEVENTS_H

#include <QLineEdit>

class LineEditThatIgnoresKeyPressedEvents : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEditThatIgnoresKeyPressedEvents(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *keyEvent);
};

#endif // LINEEDITTHATIGNORESKEYPRESSEDEVENTS_H
