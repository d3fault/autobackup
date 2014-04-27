#ifndef TIMELINEWTWIDGET_H
#define TIMELINEWTWIDGET_H

#include <Wt/WContainerWidget>
using namespace Wt;

#include <QString>
#include <QAtomicPointer>

#include "lastmodifiedtimestampsandpaths.h"

namespace Wt
{
    class WAnchor;
}

class TimeLineWtWidget : public WContainerWidget
{
public:
    static void setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer);
    static void setTimelineInternalPath(const QString &timelineInternalPath);

    TimeLineWtWidget(WContainerWidget *parent = 0);
    void handleInternalPathChanged(const QString &newInternalPath);
private:
    static QAtomicPointer<LastModifiedTimestampsAndPaths> *m_LastModifiedTimestampsAndPaths;
    static QString m_MyOwnInternalPath;

    LastModifiedTimestampsAndPaths *m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory;

    //chronological
    WAnchor *m_EarliestAnchor;
    WAnchor *m_PreviousAnchor;
    WAnchor *m_PermalinkToThisAnchor;
    WAnchor *m_RandomAnchor;
    WAnchor *m_NextAnchor;
    WAnchor *m_LatestAnchor;
};

#endif // TIMELINEWTWIDGET_H
