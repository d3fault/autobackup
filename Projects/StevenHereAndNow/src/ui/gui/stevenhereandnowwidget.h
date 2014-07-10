#ifndef STEVENHEREANDNOWWIDGET_H
#define STEVENHEREANDNOWWIDGET_H

#include <QWidget>

class StevenHereAndNowWidget : public QWidget
{
    Q_OBJECT
public:
    StevenHereAndNowWidget(QWidget *parent = 0);
    ~StevenHereAndNowWidget();
signals:
    void sayRequested(const QString &whatToSay);
};

#endif // STEVENHEREANDNOWWIDGET_H
