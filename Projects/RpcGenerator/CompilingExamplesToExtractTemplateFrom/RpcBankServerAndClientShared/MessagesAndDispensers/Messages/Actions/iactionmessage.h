#ifndef IACTIONMESSAGE_H
#define IACTIONMESSAGE_H

#include <QDataStream>

#include "../imessage.h"

class IActionMessage : public IMessage
{
    Q_OBJECT
public:
    explicit IActionMessage(QObject *parent);

    inline void setSuccessful() { m_Successful = true; }
    inline bool isSuccessful() { return m_Successful; }
    inline quint8 errorCode() { return m_ErrorCode; }
    inline void resetSuccessAndErrorCode() { m_Successful = false; m_ErrorCode = 0; }
    inline bool toggleBit() { return m_ToggleBit; }
    inline void flipToggleBit() { m_ToggleBit = !m_ToggleBit; }
private:
    //Both sides have the members. It's just a matter of whether or not you stream them in or out
    bool m_Successful;
    quint8 m_ErrorCode; //TODOreq: decide if you think 256 failed reasons per action is enough. probably is (famous last words)
    bool m_ToggleBit; //TODOoptimization: since a bool takes a byte of memory, it might be worth it to keep all my toggle bits for messages in a QBitArray as a memory optimization. However if I have to store an index into that bit array, then it defeats the purpose rofl... soo.... idfk....
    //TODOreq: stream the toggle bit where appropriate. Off the top of my head, I think it's streaming cases are the opposite of when I stream success/error-code. There is no use for it in the response I don't think. Only in request. Could be wrong?
protected:
    inline void setErrorCode(quint8 errorCode) { m_ErrorCode = errorCode; } //action messages call this from within setFailed<reason>();


//TODOreq: I think perhaps I should only stream in/out m_ErrorCode depending on m_Successful (think before changing) for both of the below... but for now to KISS I'm going to just always stream both in/out. I've made too many changes already, I'll be lucky if it even works :-/. Dynamic/minimal protocols ftw :). I was about to write right here that I should put m_ErrorCode in a safe state just in case they accidentally call it... but there is no safe state. There is no "non-error" state. 0x0 is "GenericError"... so I _GUESS_ we could set it to that... but well-written user code (WHICH WE CANNOT RELY ON) should never access m_ErrorCode unless we're not m_Successful

#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
    inline void streamSuccessAndErrorCodeIn(QDataStream &in) { in >> m_Successful; in >> m_ErrorCode; }
#endif

//we have to be one or the other

#ifdef WE_ARE_RPC_BANK_SERVER_CODE
    inline void streamSuccessAndErrorCodeOut(QDataStream &out) { out << m_Successful; out << m_ErrorCode; }
#endif
};

#endif // IACTIONMESSAGE_H
