#ifndef TIMELINENODE_H
#define TIMELINENODE_H

#include <QObject>

class TimelineNode : public QObject
{
    Q_OBJECT
public:
    explicit TimelineNode(TimelineNodeType, QString newNodeTitle, QString newNodeContent); //TODO: qvariant instead of qstring?
    enum TimelineNodeType
    {
        Text,
        Image,
        Video //TODO: vs. VideoFrame?????????????
    };
private:
    TimelineNodeType m_Type;
    QString m_Title;
    QString m_Content;
signals:

public slots:

};

#endif // TIMELINENODE_H
