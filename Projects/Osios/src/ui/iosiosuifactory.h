#ifndef IOSIOSUIFACTORY_H
#define IOSIOSUIFACTORY_H

class IOsiosUi;
class QObject;

class IOsiosUiFactory
{
public:
    IOsiosUiFactory() { }
    virtual ~IOsiosUiFactory() { }
    virtual bool uiNeedsToBeOnDifferentThreadThanOsios()=0;
    virtual IOsiosUi *newIOsiosUi(QObject *parent = 0)=0;
};

#endif // IOSIOSUIFACTORY_H
