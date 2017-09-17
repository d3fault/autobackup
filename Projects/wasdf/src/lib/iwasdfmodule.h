#ifndef IWASDFMODULE_H
#define IWASDFMODULE_H

class QObject;
class Wasdf;

class IWasdfModule
{
public:
    IWasdfModule(Wasdf *wasdf)
        : m_Wasdf(wasdf)
    { }
    IWasdfModule(const IWasdfModule &other) = delete;
    virtual ~IWasdfModule() = default;

    virtual QObject *asQObject() const = 0;
private:
    Wasdf *m_Wasdf;

//TODOoptimization: benchmark first, then use virtual function call and not signals/slots, IF the benchmark shows it would help. do I ever want a module to be on a separate thread?
public: //slots:
    virtual void handleFingerMoved(Finger fingerThatMoved, int newPos) = 0;
};

#endif // IWASDFMODULE_H
