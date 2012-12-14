#ifndef ENDRESULTTARGETTEST_H
#define ENDRESULTTARGETTEST_H

#include <QObject>

//i need a flag that my moc-hack can see and know to generate the thread helper
//the flag SHOULD IDEALLY only need to mention the class
//but i am too much of a noob so i think i'm going to need to mention it's constructor + args manually as like a string that the user types (or copies :-P). there is also the filepath to deal with TODOreq, but i can deal with that after getting constructor args working

Q_DECLARE_OBJECT_FOR_THREAD(EndResultTargetBusiness(int simpleArg1, QString implicitlySharedQString, QObject *parent = 0))
//^^must be only on one line, because that's how it easily/hackily finds the last parenthesis


//this include is generated (ANSWER: NOPE)? or i guess it shouldn't be so that we can "DECLARE/FLAG" once and then use the qthread helper all around the project by simply including it (this gives us multiple thread-objects (the object on threads))
#include "endresulttargetbusinessonthreadhelper.h"

class EndResultTargetTest : public QObject
{
    Q_OBJECT
public:
    explicit EndResultTargetTest(int simpleArg1, QString implicitlySharedQString, QObject *parent = 0);
private:
    EndResultTargetBusinessOnThreadHelper m_EndResultTargetBusiness;
signals:
    
public slots:
    void handleBusinessInstantiated();
};

#endif // ENDRESULTTARGETTEST_H
