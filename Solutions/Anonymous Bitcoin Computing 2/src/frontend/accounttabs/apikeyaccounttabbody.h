#ifndef APIKEYACCOUNTTABBODY_H
#define APIKEYACCOUNTTABBODY_H

#include "iaccounttabwidgettabbody.h"

class ApiKeyAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    static void setApiHttpsPort(const std::string &apiHttpsPort);

    ApiKeyAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    static std::string m_ApiHttpsPort;

    friend class AnonymousBitcoinComputingWtGUI;
    void populateAndInitialize();
    void displayApiKey(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
};

#endif // APIKEYACCOUNTTABBODY_H
