#ifndef DINOSAURALPHABETGAMEWIDGET_H
#define DINOSAURALPHABETGAMEWIDGET_H

#include <QtGui/QWidget>
#include <QThread>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

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

    //gui
    QVBoxLayout *m_Layout;
    QLabel *m_KeySetLabel;
private:
    QMap<int,Qt::Key> *m_CurrentKeySet;
    //gui
    void setLabelFromIndexToEnd(int index);
protected:
    void keyPressEvent(QKeyEvent *);
signals:
    void keyPressed(Qt::Key key);
private slots:
    void handleKeySetChanged(QMap<int,Qt::Key> *newKeySet);
    void handleCurrentIndexChanged(int newIndex);
    void handleGuessedWrong();
public slots:
    void buildGui();
};

#endif // DINOSAURALPHABETGAMEWIDGET_H
