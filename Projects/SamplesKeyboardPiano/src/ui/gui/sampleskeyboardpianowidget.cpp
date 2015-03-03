#include "sampleskeyboardpianowidget.h"

#include <QKeyEvent>
#include <QGridLayout>

#include "shiftclickablepushbutton.h"

#define SamplesKeyboardPianoWidget_NUM_COLUMNS_PER_ROW 5

//TODOoptional: pressing the key on the keyboard makes the corresponding button depress
SamplesKeyboardPianoWidget::SamplesKeyboardPianoWidget(QWidget *parent)
    : QWidget(parent)
{
    //setFocusPolicy(Qt::ClickFocus);
    //setFocus();
    //grabKeyboard();

    setWindowTitle(tr("Samples Keyboard Piano"));

    QGridLayout *gridLayout = new QGridLayout();
    int currentRow = 0;
    int currentColumn = 0;
    for(int i = Qt::Key_A; i <= Qt::Key_Z; ++i)
    {
        //QPushButton *keyboardKeyButton = new QPushButton(QKeySequence(i).toString());
        ShiftClickablePushButton *keyboardKeyButton = new ShiftClickablePushButton(i);
        connect(keyboardKeyButton, SIGNAL(alphabeticalKeyPressDetected(int,bool)), this, SIGNAL(alphabeticalKeyPressDetected(int,bool)));
        gridLayout->addWidget(keyboardKeyButton, currentRow, currentColumn++);
        keyboardKeyButton->installEventFilter(keyboardKeyButton);

        if(currentColumn > SamplesKeyboardPianoWidget_NUM_COLUMNS_PER_ROW)
        {
            currentColumn = 0;
            ++currentRow;
        }
    }

    setLayout(gridLayout);
}
void SamplesKeyboardPianoWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z)
    {
        bool shiftKeyPressed = (keyEvent->modifiers() & Qt::ShiftModifier);
        emit alphabeticalKeyPressDetected(keyEvent->key(), shiftKeyPressed);
        return;
    }
    QWidget::keyPressEvent(keyEvent);
}
