#ifndef SOCIALNETWORKTIMELINE_H
#define SOCIALNETWORKTIMELINE_H

#include <QObject>

class SocialNetworkTimeline : public QObject
{
    Q_OBJECT
public:
    explicit SocialNetworkTimeline(QObject *parent = 0);
};

#endif // SOCIALNETWORKTIMELINE_H
