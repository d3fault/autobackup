#include "dinosauralphabetgamewidget.h"

dinosaurAlphabetGameWidget::dinosaurAlphabetGameWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Thread = new QThread();
    m_Game = new DinosaurAlphabetGame();
    m_Game->moveToThread(m_Thread);
    m_Thread->start();

    //connect the gui events that we want to use to the game object
    connect(this, SIGNAL(keyPressed(Qt::Key)), m_Game, SLOT(processKey(Qt::Key)));

    //connect to m_Game's signals to receive gui update events
    connect(m_Game, SIGNAL(keySetChanged(QMap<int,Qt::Key>*)), this, SLOT(handleKeySetChanged(QMap<int,Qt::Key>*)));
    connect(m_Game, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCurrentIndexChanged(int)));
    connect(m_Game, SIGNAL(guessedWrong()), this, SLOT(handleGuessedWrong()));

    //schedule that start() be called on the game thread
    QMetaObject::invokeMethod(m_Game, "start", Qt::QueuedConnection);

    //schedule us to build the gui. we don't do it now so that we get our widget to show up asap. i guess i could schedule us to also set up the game object and thread in a future even too, making the constructor only queue 2 events (the "start" event is relocated to the hypothetical event) and then be finished
    QMetaObject::invokeMethod(this, "buildGui", Qt::QueuedConnection);
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
void dinosaurAlphabetGameWidget::buildGui()
{
    m_Layout = new QVBoxLayout();
    m_KeySetLabel = new QLabel();
    QFont labelFont;
    labelFont.setPointSize(30);
    m_KeySetLabel->setFont(labelFont);
    m_Layout->addWidget(m_KeySetLabel,0, Qt::AlignCenter);
    this->setLayout(m_Layout);
}
void dinosaurAlphabetGameWidget::handleKeySetChanged(QMap<int,Qt::Key> *newKeySet)
{
    m_CurrentKeySet = newKeySet;
    setLabelFromIndexToEnd(0);
}
void dinosaurAlphabetGameWidget::handleCurrentIndexChanged(int newIndex)
{
    setLabelFromIndexToEnd(newIndex);
}
void dinosaurAlphabetGameWidget::setLabelFromIndexToEnd(int index)
{
    QString keys;
    int keySetCount = m_CurrentKeySet->count();
    //we don't need an iterator here because we KNOW it's 0-N ordered
    for(int i = index; i < keySetCount; ++i)
    {
        //keys.append(QString::number(i));
        //keys.append(QString::fromAscii((const char*)(int)newKeySet.at(i)));
        keys.append(QString((char)(int)m_CurrentKeySet->value(i)));
    }
    m_KeySetLabel->setText(keys);
}
void dinosaurAlphabetGameWidget::handleGuessedWrong()
{
    QMessageBox *mb = new QMessageBox();
    mb->setText("Wrong");
    mb->show();
}
