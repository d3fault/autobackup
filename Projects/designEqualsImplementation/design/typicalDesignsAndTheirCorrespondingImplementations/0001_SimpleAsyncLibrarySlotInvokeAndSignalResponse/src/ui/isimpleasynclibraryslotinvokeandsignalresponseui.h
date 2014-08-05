#ifndef ISIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEUI_H
#define ISIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEUI_H

//should there be an interface for each use case instead?
class ISimpleAsyncLibrarySlotInvokeAndSignalResponseUi
{
    //signals
protected:
    virtual void fooSignal(bool success)=0;
    //slots
public:
    virtual void fooSlot(const QString &cunt)=0;
};

#endif // ISIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEUI_H
