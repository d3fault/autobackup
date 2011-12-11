#include "dinosauralphabetgamewidget.h"

dinosaurAlphabetGameWidget::dinosaurAlphabetGameWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Game = new DinosaurAlphabetGame();
    m_Game->start();
}
dinosaurAlphabetGameWidget::~dinosaurAlphabetGameWidget()
{

}
