#ifndef ICEPICKWIDGET_H
#define ICEPICKWIDGET_H

#include <QWidget>

#include "../lib/icepick.h"

class QCheckBox;
class QTextEdit;

class IcePickWidget : public QWidget
{
    Q_OBJECT
public:
    IcePickWidget(QWidget *parent = 0);
private:
    YearRange m_YearRange;
    QTextEdit *m_Output;
    QCheckBox *m_ForceRefreshCacheCheckbox;
signals:
    void pickIceRequested(YearRange yearRange, bool forceRefreshCache = false);
public slots:
    void handleMsg(const QString &msg);
    void handleFinishedPickingIce(bool success, IceEntry iceEntry);
private slots:
    void handleMinYearChanged(int newMin);
    void handleMaxYearChanged(int newMax);
    void handlePickIceButtonClicked();
};

#endif // ICEPICKWIDGET_H
