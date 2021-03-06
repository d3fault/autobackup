#ifndef TIMELINEWTWIDGET_H
#define TIMELINEWTWIDGET_H

#include <Wt/WContainerWidget>

#include <QString>
#include <QAtomicPointer>

using namespace Wt;
using namespace std;

#include "lastmodifiedtimestampsandpaths.h"

namespace Wt
{
    class WAnchor;
}

class TimeLineWtWidget : public WContainerWidget
{
public:
    static void setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer);
    static void setHvbsWebBaseDirActual(const QString &hvbsWebBaseDirActual);

    TimeLineWtWidget(WContainerWidget *parent = 0);
    void redirectToRandomPointInTimeline();
    void presentFile(const QString &relativePath_aka_internalPathQString, const QString &absolutePath, const std::string &myBrainItemFilenameOnlyStdString);
private:
    static QAtomicPointer<LastModifiedTimestampsAndPaths> *m_LastModifiedTimestampsAndPaths;
    static std::string m_HvbsWebBaseDirActual_NOT_CLEAN_URL;

    WContainerWidget *m_ContentsContainer;
    LastModifiedTimestampsAndPaths *m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory;

    //chronological timestamps
    WText *m_EarliestTimestamp;
    WText *m_PreviousTimestamp;
    WText *m_CurrentTimestamp;
    WText *m_NextTimestamp;
    WText *m_LatestTimestamp;

    //chronological anchors
    WAnchor *m_EarliestAnchor;
    WAnchor *m_PreviousAnchor;
    WAnchor *m_CurrentAnchor;
    WAnchor *m_NextAnchor;
    WAnchor *m_LatestAnchor;

    void embedBasedOnMimeType(const std::string &mimeType);
    void embedPicture(const std::string &mimeType, const QString &filename);
    void embedVideoFile(const std::string &mimeType, const QString &filename);
    void embedAudioFile(const std::string &mimeType, const QString &filename);

    string embedBasedOnFileExtensionAndReturnMimeType(const QString &filename);

    void setMainContent(WWidget *mainContent);

    WString longLongTimestampToWString(long long timestamp);

    inline LastModifiedTimestampsAndPaths* myQt4and5AtomicPointerGet()
    {
#if !(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        return m_LastModifiedTimestampsAndPaths->loadAcquire();
#else
        return *m_LastModifiedTimestampsAndPaths;
#endif
    }
};

#endif // TIMELINEWTWIDGET_H
