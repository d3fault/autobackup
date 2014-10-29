#ifndef MAINMENUACTIVITYCHANGEDTIMELINENODE_H
#define MAINMENUACTIVITYCHANGEDTIMELINENODE_H

#include "../itimelinenode.h"
#include "../iactivitytab_widget_formainmenutabwidget.h"

#define MAIN_MENU_DEFAULT_ACTIVITY MainMenuActivitiesEnum::TimelineViewActivity

class MainMenuActivityChangedTimelineNode : public ITimelineNode
{
  public:
    MainMenuActivityChangedTimelineNode()
        : ITimelineNode(TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode, 0)
        , NewMainMenuActivity(MAIN_MENU_DEFAULT_ACTIVITY)
    { }
    MainMenuActivityChangedTimelineNode(MainMenuActivitiesEnum::MainMenuActivitiesEnumActual newMainMenuActivity, qint64 unixTimestamp_OrZeroToUseCurrentTime = 0)
        : ITimelineNode(TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode, unixTimestamp_OrZeroToUseCurrentTime)
        , NewMainMenuActivity(newMainMenuActivity)
    { }
    MainMenuActivityChangedTimelineNode(const MainMenuActivityChangedTimelineNode &other)
        : ITimelineNode(other)
        , NewMainMenuActivity(other.NewMainMenuActivity)
    { }
    virtual ~MainMenuActivityChangedTimelineNode();

    MainMenuActivitiesEnum::MainMenuActivitiesEnumActual NewMainMenuActivity;

    virtual QString humanReadableShortDescription() const;
protected:
    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

#if 0

class MainMenuActivityChangedTimelineNodeData : public ITimelineNodeData
{
public:
    MainMenuActivityChangedTimelineNodeData(MainMenuActivitiesEnum::MainMenuActivitiesEnumActual newMainMenuActivity)
        : ITimelineNodeData(TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode)
        , NewMainMenuActivity(newMainMenuActivity)
    { }
    MainMenuActivityChangedTimelineNodeData(const MainMenuActivityChangedTimelineNodeData &other)
        : ITimelineNodeData(other)
        , NewMainMenuActivity(other.NewMainMenuActivity)
    { }
    virtual ~MainMenuActivityChangedTimelineNodeData();

    MainMenuActivitiesEnum::MainMenuActivitiesEnumActual NewMainMenuActivity;

    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

class MainMenuActivityChangedTimelineNode : public ITimelineNode<MainMenuActivityChangedTimelineNodeData>
{
  public:
    MainMenuActivityChangedTimelineNode(MainMenuActivitiesEnum::MainMenuActivitiesEnumActual newMainMenuActivity)
        : ITimelineNode(TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode)
    {
        d = new MainMenuActivityChangedTimelineNodeData(newMainMenuActivity);
    }
    MainMenuActivityChangedTimelineNode(const MainMenuActivityChangedTimelineNode &other)
        : ITimelineNode(other)
        , d(other.d)
    { }
    virtual ~MainMenuActivityChangedTimelineNode();

    void setNewMainMenuActivity(MainMenuActivitiesEnum::MainMenuActivitiesEnumActual newMainMenuActivity) { d->NewMainMenuActivity = newMainMenuActivity; }
    int newMainMenuActivity() const { return d->NewMainMenuActivity; }
  private:
    QSharedDataPointer<MainMenuActivityChangedTimelineNodeData> d;
};

#endif

#endif // MAINMENUACTIVITYCHANGEDTIMELINENODE_H
