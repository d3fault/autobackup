#ifndef IOSIOSGUIFACTORY_H
#define IOSIOSGUIFACTORY_H

#include "../iosiosuifactory.h"

class IOsiosGuiFactory : public IOsiosUiFactory
{
public:
    IOsiosGuiFactory() : IOsiosUiFactory() { }
    virtual ~IOsiosGuiFactory() { }
    bool uiNeedsToBeOnDifferentThreadThanOsios();
    IOsiosUi *newIOsiosUi(QObject *parent = 0);
};

#endif // IOSIOSGUIFACTORY_H
