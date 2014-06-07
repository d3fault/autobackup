#include "simplenotepadautosavewithoutoverwritinggui.h"

#include <QCoreApplication>
#include <QStringList>

#include "simplenotepadautosavewithoutoverwritingwidget.h"

SimpleNotepadAutosaveWithoutOverwritingGui::SimpleNotepadAutosaveWithoutOverwritingGui(QObject *parent)
    : QObject(parent)
{
    QStringList argz = QCoreApplication::arguments();

    QString autoSaveTemplate; //TODOreq in backend: "prefix-%timestamp%-%contenthash%.txt" <-- there now it is sound theory not based o cn courrent day dtcechnology
    if(argz.size() > 1)
    {
        autoSaveTemplate = argz.at(1);
    }
    //QWidget is on same thread as us always at this point, so we don't use regular QMetaObject::invokeMethod design
    //instead we'll just instatitate it into existence
    m_Gui = new SimpleNotepadAutosaveWithoutOverwritingWidget(autoSaveTemplate, this);
}
