#ifndef APIKEYACCOUNTTABBODY_H
#define APIKEYACCOUNTTABBODY_H

#include "iaccounttabwidgettabbody.h"

class ApiKeyAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    ApiKeyAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    friend class AnonymousBitcoinComputingWtGUI;
    void populateAndInitialize();
    void displayApiKey(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
};

#endif // APIKEYACCOUNTTABBODY_H
