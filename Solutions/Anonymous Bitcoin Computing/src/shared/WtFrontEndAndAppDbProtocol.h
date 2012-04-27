#ifndef WTFRONTENDANDAPPDBPROTOCOL_H
#define WTFRONTENDANDAPPDBPROTOCOL_H

#include <QDataStream>

struct WtFrontEndAndAppDbMessage
{
    enum WtFrontEndAndAppDbMessageType
    {
        InvalidWtFrontEndAndAppDbMessageType = 0,
        WtFrontEndToAppDbMessageType,
        AppDbToWtFrontEndMessageType
    };
    inline WtFrontEndAndAppDbMessage(WtFrontEndAndAppDbMessageType wtFrontEndAndAppDbMessageType = InvalidWtFrontEndAndAppDbMessageType, quint16 theMessage = 0, QString extraString0)
        : m_WtFrontEndAndAppDbMessageType(wtFrontEndAndAppDbMessageType), m_TheMessage(theMessage), m_ExtraString0(extraString0)
    { }
    quint16 m_WtFrontEndAndAppDbMessageType;
    quint16 m_TheMessage;
    QString m_ExtraString0;
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
};

#endif // WTFRONTENDANDAPPDBPROTOCOL_H
