#ifndef WTFRONTENDANDAPPDBPROTOCOL_H
#define WTFRONTENDANDAPPDBPROTOCOL_H

#include <QDataStream>
#include <QList>

struct WtFrontEndAndAppDbMessage
{
    enum WtFrontEndAndAppDbMessageType
    {
        InvalidWtFrontEndAndAppDbMessageType = 0,
        WtFrontEndToAppDbMessageType,
        AppDbToWtFrontEndMessageType
    };
    inline WtFrontEndAndAppDbMessage(WtFrontEndAndAppDbMessageType wtFrontEndAndAppDbMessageType = InvalidWtFrontEndAndAppDbMessageType, quint16 theMessage = 0, QString extraString0 = QString())
        : m_WtFrontEndAndAppDbMessageType(wtFrontEndAndAppDbMessageType), m_TheMessage(theMessage), m_ExtraString0(extraString0)
    { }
    quint16 m_WtFrontEndAndAppDbMessageType;
    quint16 m_TheMessage;
    QString m_ExtraString0;

    //don't-pass via DataStream
    uint m_RequestorId;

    //i could have the 'give' and 'recycle' messages be here and stored in a QHash<WtFrontEndAndAppDbMessageType,QList<WtFrontEndAndAppDbMessage*> > .... with give accepting a WtFrontEndAndAppDbMessageType as a parameter... but who gives a fuck.
};
struct WtFrontEndToAppDbMessage : public WtFrontEndAndAppDbMessage
{
    enum TheMessage
    {
        InvalidWtFrontEndToAppDbMessageType = 0,
        CreateBankAccountForUserX, /* TODO: withPublicCertY.. for SecureAsFuckBank */
        EtcEtcLoLOlThisIsMesasagserawrawer
    };
    inline WtFrontEndToAppDbMessage(TheMessage theMessage = InvalidWtFrontEndToAppDbMessageType, QString extraString0 = QString())
        : WtFrontEndAndAppDbMessage(WtFrontEndToAppDbMessageType, theMessage, extraString0)
    { }
    static QList<AppDbToWtFrontEndMessage*> m_RecycledWtFrontEndToAppDbMessages;
    static WtFrontEndToAppDbMessage *giveMeAWtFrontEndToAppDbMessage(uint requestorId)
    {
        WtFrontEndToAppDbMessage *wtFrontEndToAppDbMessage;
        if(!m_RecycledWtFrontEndToAppDbMessages.isEmpty())
        {
            wtFrontEndToAppDbMessage = m_RecycledWtFrontEndToAppDbMessages.takeLast();
        }
        else
        {
            wtFrontEndToAppDbMessage = new WtFrontEndToAppDbMessage();
        }
        wtFrontEndToAppDbMessage->m_RequestorId = requestorId;
        return wtFrontEndToAppDbMessage;
    }
    static void returnAWtFrontEndToAppDbMessage(WtFrontEndToAppDbMessage *message)
    {
        m_RecycledWtFrontEndToAppDbMessages.append(message);
    }
};
struct AppDbToWtFrontEndMessage : public WtFrontEndAndAppDbMessage
{
    enum TheMessage
    {
        InvalidAppDbToWtFrontEndMessageType = 0,
        BankAccountForUserXCreated,
        HereIsTheDataThatYouAndOnlyYouRequested,
        HereIsAnUpdateThatIAmPushingToEveryone,
        Etcl0asdlfkwerouLOLetcetcOrSomethingMaybeRewriteModifyAdaptChange
    };
    inline AppDbToWtFrontEndMessage(TheMessage theMessage = InvalidAppDbToWtFrontEndMessageType, QString extraString0 = QString())
        : WtFrontEndAndAppDbMessage(AppDbToWtFrontEndMessageType, theMessage, extraString0)
    { }
    static QList<AppDbToWtFrontEndMessage*> m_RecycledAppDbToWtFrontEndMessages;
    static AppDbToWtFrontEndMessage *giveMeAnAppDbToWtFrontEndMessage(uint requestorId)
    {
        AppDbToWtFrontEndMessage *appDbToWtFrontEndMessage;
        if(!m_RecycledAppDbToWtFrontEndMessages.isEmpty())
        {
            appDbToWtFrontEndMessage = m_RecycledAppDbToWtFrontEndMessages.takeLast();
        }
        else
        {
            appDbToWtFrontEndMessage = new AppDbToWtFrontEndMessage();
        }
        appDbToWtFrontEndMessage->m_RequestorId = requestorId;
        return appDbToWtFrontEndMessage;
    }
    static void returnAppDbToWtFrontEndMessage(AppDbToWtFrontEndMessage *message)
    {
        m_RecycledAppDbToWtFrontEndMessages.append(message);
    }
};

#endif // WTFRONTENDANDAPPDBPROTOCOL_H
