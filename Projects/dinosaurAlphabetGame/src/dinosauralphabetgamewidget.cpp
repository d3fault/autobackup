#include "dinosauralphabetgamewidget.h"

dinosaurAlphabetGameWidget::dinosaurAlphabetGameWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Thread = new QThread();
    m_Game = new DinosaurAlphabetGame();
    m_Game->moveToThread(m_Thread);
    m_Thread->start();

    //connect the gui events that we want to use to the game
    connect(this, SIGNAL(keyPressed(Qt::Key)), m_Game, SLOT(handleKeyPressed(Qt::Key)));

    //connect to m_Game's signals to receive gui update events

    //schedule that start() be called on the game thread
    QMetaObject::invokeMethod(m_Game, "start", Qt::QueuedConnection);
}
dinosaurAlphabetGameWidget::~dinosaurAlphabetGameWidget()
{

}
void dinosaurAlphabetGameWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    if(InputKeyFilter::isAtoZOnly((Qt::Key)keyEvent->key()))
    {
        emit keyPressed((Qt::Key)keyEvent->key());
    }
}
