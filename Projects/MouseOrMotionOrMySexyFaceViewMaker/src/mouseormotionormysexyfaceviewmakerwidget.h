#ifndef MOUSEORMOTIONORMYSEXYFACEVIEWMAKERWIDGET_H
#define MOUSEORMOTIONORMYSEXYFACEVIEWMAKERWIDGET_H

#include <QWidget>
#include <QSize>
#include <QLabel>
#include <QPixmap>

class MouseOrMotionOrMySexyFaceViewMakerWidget : public QWidget
{
    Q_OBJECT
public:
    MouseOrMotionOrMySexyFaceViewMakerWidget(const QSize &viewSize, QWidget *parent = 0);
protected:
    void mouseDoubleClickEvent(QMouseEvent *e);
private:
    QLabel *m_ViewLabel;
signals:
    void startMakingMouseAndOrMotionViewsRequested();
    void setMouseOrMotionOrMySexyFaceViewModeRequested();
    void setMySexyFaceViewModeRequested();
public slots:
    void presentPixmapForViewing(const QPixmap &tehPixmap);
    void handleMySexyFaceStreamIsFrozen();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKERWIDGET_H
