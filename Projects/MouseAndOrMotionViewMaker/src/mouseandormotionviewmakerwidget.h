#ifndef MOUSEANDORMOTIONVIEWMAKERWIDGET_H
#define MOUSEANDORMOTIONVIEWMAKERWIDGET_H

#include <QWidget>
#include <QSize>
#include <QLabel>
#include <QPixmap>

class MouseAndOrMotionViewMakerWidget : public QWidget
{
    Q_OBJECT
public:
    MouseAndOrMotionViewMakerWidget(const QSize &viewSize, QWidget *parent = 0);
    ~MouseAndOrMotionViewMakerWidget();
private:
    QLabel *m_ViewLabel;
signals:
    void startMakingMouseAndOrMotionViewsRequested();
public slots:
    void presentPixmapForViewing(const QPixmap &tehPixmap);
};

#endif // MOUSEANDORMOTIONVIEWMAKERWIDGET_H
