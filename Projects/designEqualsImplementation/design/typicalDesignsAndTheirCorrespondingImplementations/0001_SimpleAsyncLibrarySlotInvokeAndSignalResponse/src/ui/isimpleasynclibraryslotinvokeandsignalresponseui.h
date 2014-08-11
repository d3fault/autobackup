#ifndef ISIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEUI_H
#define ISIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEUI_H

//should there be an interface for each use case instead?
class ISimpleAsyncLibrarySlotInvokeAndSignalResponseUi
{

//public:
    //virtual void handleFoo

//protected:
    //virtual void fooSignal(bool success)=0;

    //api/defined slots need signals to trigger them. the ui is responsible for emitting the signal at the proper time
protected:
    virtual void fooSlotRequested(const QString &cunt)=0;

    //any api/defined signal may be connected to N slots in the ui
public:
    //virtual void handleFooSignal(bool success) { }; //override optional
    //or
    virtual void handleFooSignal(bool success)=0;

    //...or.... anti-pattern?
};

#endif // ISIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEUI_H
