#include "iosiosguifactory.h"

#include "osiosgui.h"

bool IOsiosGuiFactory::uiNeedsToBeOnDifferentThreadThanOsios()
{
    return true;
}
IOsiosUi *IOsiosGuiFactory::newIOsiosUi(QObject *parent)
{
    return new OsiosGui(parent);
}
