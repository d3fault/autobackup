#ifndef TIMELINEWITHFILESTORE_H
#define TIMELINEWITHFILESTORE_H

#include <QObject>

class TimelineWithFileStore : public QObject
{
    Q_OBJECT
public:
    explicit TimelineWithFileStore(QObject *parent = 0);
    void addNode(TimelineNode *nodeToAdd);
signals:

public slots:

};

#endif // TIMELINEWITHFILESTORE_H
