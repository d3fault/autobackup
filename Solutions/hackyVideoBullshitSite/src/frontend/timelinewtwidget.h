#ifndef TIMELINEWTWIDGET_H
#define TIMELINEWTWIDGET_H

#include <Wt/WContainerWidget>
using namespace Wt;

#include <QString>

class TimeLineWtWidget : public WContainerWidget
{
public:
    TimeLineWtWidget(const QString &myOwnInternalPath, WContainerWidget *parent = 0);
    void handleInternalPathChanged(const QString &newInternalPath);
private:
    enum TimeLineDirection { Horizontal = 0, Vertical = 1 };

    const QString &m_MyOwnInternalPath;
    TimeLineDirection m_CurrentDirection;

    void handleDirectionRadiosCheckChanged(WRadioButton *newlyCheckedRadioButton);
};

#endif // TIMELINEWTWIDGET_H
