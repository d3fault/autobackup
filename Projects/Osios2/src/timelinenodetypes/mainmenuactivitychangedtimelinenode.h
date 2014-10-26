#ifndef MAINMENUACTIVITYCHANGEDTIMELINENODE_H
#define MAINMENUACTIVITYCHANGEDTIMELINENODE_H

#include "../itimelinenode.h"
#include "../iactivitytab_widget_formainmenutabwidget.h"

//#define MAIN_MENU_DEFAULT_ACTIVITY TimelineViewActivity

class MainMenuActivityChangedTimelineNodeData : public ITimelineNodeDataITimelineNodeData
{
public:
    MainMenuActivityChangedTimelineNodeData(MainMenuActivitiesEnum::MainMenuActivitiesEnumActual newMainMenuActivity)
        : ITimelineNodeDataITimelineNodeData(TimelineNodeTypeEnum::MainMenuActivityChangedTimelineNode)
        , NewMainMenuActivity(newMainMenuActivity)
    { }
    MainMenuActivityChangedTimelineNodeData(const MainMenuActivityChangedTimelineNodeData &other)
        : ITimelineNodeDataITimelineNodeData(other)
        , NewMainMenuActivity(other.NewMainMenuActivity)
    { }
    virtual ~MainMenuActivityChangedTimelineNodeData();

    MainMenuActivitiesEnum::MainMenuActivitiesEnumActual NewMainMenuActivity;

    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

class MainMenuActivityChangedTimelineNode : public ITimelineNode
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

#endif // MAINMENUACTIVITYCHANGEDTIMELINENODE_H
