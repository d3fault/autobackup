#ifndef DINOSAURALPHABETGAMEWIDGET_H
#define DINOSAURALPHABETGAMEWIDGET_H

#include <QtGui/QWidget>
#include <QThread>
#include <QKeyEvent>

#include <dinosauralphabetgame.h>
#include <inputkeyfilter.h>


class dinosaurAlphabetGameWidget : public QWidget
{
    Q_OBJECT

public:
    dinosaurAlphabetGameWidget(QWidget *parent = 0);
    DinosaurAlphabetGame *m_Game;
    QThread *m_Thread;
    ~dinosaurAlphabetGameWidget();
protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // DINOSAURALPHABETGAMEWIDGET_H
