#ifndef ENDRESULTTARGETBUSINESSONTHREADHELPER_H
#define ENDRESULTTARGETBUSINESSONTHREADHELPER_H

#include <QThread>

//also generated
#include "endresulttargetbusiness.h"

//generated class name
class EndResultTargetBusinessOnThreadHelper : public QThread
{
    Q_OBJECT
public:
    explicit EndResultTargetBusinessOnThreadHelper(QObject *parent = 0);
    EndResultTargetBusiness *getEndResultTargetBusiness()
    {
        return m_EndResultTargetBusiness;
    }
private:
    //generated list is easy (no additives) for simple args
    int m_SimpleArg1Saver;

    //implicitly shared args need to deref later otherwise our saver might be holding onto it when the app expects the reference count to be zero
    QString *m_ImplicitlySharedQStringWithPointerModifierAdded;
protected:
    virtual void run();
    EndResultTargetBusiness *m_EndResultTargetBusiness;
public slots:
    //generated constructor(s????) <-- hardest part
    void start(int simpleArg1, QString implicitlySharedQString, Priority priority = InheritPriority);
signals:
    void objectOnThreadEndResultTargetBusinessInstantiated();
};

#endif // ENDRESULTTARGETBUSINESSONTHREADHELPER_H
