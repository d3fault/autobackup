#ifndef IACTIONMESSAGE_H
#define IACTIONMESSAGE_H

#include <QDataStream>

#include "../imessage.h"

class IActionMessage : public IMessage
{
    Q_OBJECT
public:
    explicit IActionMessage(QObject *parent = 0);

    inline void setSuccessful() { m_Successful = true; }
    inline bool isSuccessful() { return m_Successful; }
    inline quint8 errorCode() { return m_ErrorCode; }
    inline void resetSuccessAndErrorCode() { m_Successful = false; m_ErrorCode = 0; }
private:
    //Both sides have the members. It's just a matter of whether or not you stream them in or out
    bool m_Successful;
    quint8 m_ErrorCode; //TODOreq: decide if you think 256 failed reasons per action is enough. probably is (famous last words)
protected:
    inline void setErrorCode(quint8 errorCode) { m_ErrorCode = errorCode; } //action messages call this from within setFailed<reason>();

#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
    inline void streamSuccessAndErrorCodeIn(QDataStream &in) { in >> Success; in >> ErrorCode; }
#endif

//we have to be one or the other

#ifdef WE_ARE_RPC_BANK_SERVER_CODE
    inline void streamSuccessAndErrorCodeOut(QDataStream &out) { out << Success; out << ErrorCode; }
#endif
};

#endif // IACTIONMESSAGE_H
