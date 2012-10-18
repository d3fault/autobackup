#ifndef IACTIONMESSAGE_H
#define IACTIONMESSAGE_H

#include <QDataStream>

#include "../imessage.h"

class IActionMessage : public IMessage
{
    Q_OBJECT
public:
    explicit IActionMessage(QObject *parent = 0);

    inline void setSuccessful();
    inline bool isSuccessful();
    inline quint8 errorCode();
    inline void resetSuccessAndErrorCode();
private:
    //Both sides have the members. It's just a matter of whether or not you stream them in or out
    bool m_Successful;
    quint8 m_ErrorCode; //TODOreq: decide if you think 256 failed reasons per action is enough. probably is (famous last words)
protected:
    inline void setErrorCode(quint8 errorCode); //action messages call this from within setFailed<reason>();

#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
    void streamSuccessAndErrorCodeIn(QDataStream &in);
#endif

//we have to be one or the other

#ifdef WE_ARE_RPC_BANK_SERVER_CODE
    void streamSuccessAndErrorCodeOut(QDataStream &out);
#endif
};

#endif // IACTIONMESSAGE_H
