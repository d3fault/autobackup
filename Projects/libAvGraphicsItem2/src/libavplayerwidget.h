#ifndef LIBAVPLAYERWIDGET_H
#define LIBAVPLAYERWIDGET_H

#include <QtGui/QWidget>

class libAvPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    libAvPlayerWidget(QWidget *parent = 0);
    ~libAvPlayerWidget();
};

#endif // LIBAVPLAYERWIDGET_H
