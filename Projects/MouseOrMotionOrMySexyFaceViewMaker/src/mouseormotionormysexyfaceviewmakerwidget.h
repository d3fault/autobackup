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
    ~MouseOrMotionOrMySexyFaceViewMakerWidget();
private:
    QLabel *m_ViewLabel;
signals:
    void startMakingMouseAndOrMotionViewsRequested();
public slots:
    void presentPixmapForViewing(const QPixmap &tehPixmap);
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKERWIDGET_H
