#include "ourserverforwtfrontends.h"

OurServerForWtFrontEnds::OurServerForWtFrontEnds(QObject *parent) :
    QObject(parent)
{
}
void OurServerForWtFrontEnds::startListeningForWtFrontEnds()
{
    //TODO: start the SSL server (contained. as in, it's logic should not mix with this class's logic........ which considering i have to implement QSslSocket anyways... shouldn't be TOO hard. but idk if i should have yet another class encapsulating even that)
}
void OurServerForWtFrontEnds::handleResponseFromAppLogic(AppLogicRequestResponse *response)
{
}
