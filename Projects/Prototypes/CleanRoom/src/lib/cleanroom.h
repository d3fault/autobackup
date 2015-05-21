#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>

class ICleanRoomRequest;

class CleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
public slots:
    void getFrontPageDefaultViewBegin(/*TODOreq: args? like 'request' or 'response path' etc...*/);
    void finishedGettingFrontPageDefaultView(ICleanRoomRequest *request, QStringList frontPageDocs);
};

#endif // CLEANROOM_H
