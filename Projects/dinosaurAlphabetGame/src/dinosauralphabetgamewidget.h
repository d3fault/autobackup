#ifndef DINOSAURALPHABETGAMEWIDGET_H
#define DINOSAURALPHABETGAMEWIDGET_H

#include <QtGui/QWidget>

#include <dinosauralphabetgame.h>

class dinosaurAlphabetGameWidget : public QWidget
{
    Q_OBJECT

public:
    dinosaurAlphabetGameWidget(QWidget *parent = 0);
    DinosaurAlphabetGame *m_Game;
    ~dinosaurAlphabetGameWidget();
};

#endif // DINOSAURALPHABETGAMEWIDGET_H
