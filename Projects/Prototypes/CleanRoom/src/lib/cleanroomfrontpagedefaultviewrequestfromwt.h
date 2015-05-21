#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H

#include "ifrontpagedefaultviewrequest.h"

#include <QStringList>

#include <boost/function.hpp>

class CleanRoom;

class CleanRoomFrontPageDefaultViewRequestFromWt : public IFrontPageDefaultViewRequest
{
public:
    CleanRoomFrontPageDefaultViewRequestFromWt(CleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (QStringList/*front page docs*/)> wApplicationCallback);
    //void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
    void respond(QStringList frontPageDocs);
private:
    std::string m_WtSessionId;
    boost::function<void (QStringList/*front page docs*/)> m_WApplicationCallback;
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMWT_H


#if 0
#include <QCoreApplication>

#include <QVariant>
#include <QDebug>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

class SomeClass
{
public:
    void someMethod(QVariantList list)
    {
        qDebug() << list;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVariantList someList;
    someList.append(QString("yolo"));

#if 1
    boost::shared_ptr<SomeClass> someClass(new SomeClass());
    boost::function<void (QVariantList)> f = boost::bind(&SomeClass::someMethod, someClass, _1);
    f(someList);
#else //compiles also, but doesn't use bind
    boost::function<void (SomeClass*,QVariantList)> someFunction;
    someFunction = &SomeClass::someMethod;
    SomeClass someClass;
    someFunction(&someClass, someList);
#endif

    return 0;
}
#endif
