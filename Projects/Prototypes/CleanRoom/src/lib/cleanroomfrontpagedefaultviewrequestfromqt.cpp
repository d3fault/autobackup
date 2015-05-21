#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

#include <QVariant>

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const QString &slotCallback, QObject *parent)
    : IFrontPageDefaultViewRequest(cleanRoom, parent)
    //, m_ObjectCallback(objectCallback)
    //, m_SlotCallback(slotCallback.toStdString())
{
    std::string slotCallbackStdString = slotCallback.toStdString();
    QObject::connect(this, SIGNAL(frontPageDefaultViewResponseRequested(QStringList)), objectCallback, slotCallbackStdString.c_str());
}
#if 0
void CleanRoomFrontPageDefaultViewRequestFromQt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    QMetaObject::invokeMethod(m_ObjectCallback, m_SlotCallback.c_str(), Q_ARG(QVariantList, responseArgs));
}
#endif
void CleanRoomFrontPageDefaultViewRequestFromQt::respond(QStringList frontPageDocs)
{
    emit frontPageDefaultViewResponseRequested(frontPageDocs);
    //QMetaObject::invokeMethod(m_ObjectCallback, m_SlotCallback.c_str(), Q_ARG(QStringList, frontPageDocs)); //TODOreq: use signals/slots instead
}
