#include "format2ui.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "iuigenerator.h"

format2ui::format2ui(QObject *parent)
    : QObject(parent)
{ }
void format2ui::beginFormat2ui(const QString &filePathOfJsonUiFormatInput, const QString &cliArgUiTypeString)
{
    QFile inputFile(filePathOfJsonUiFormatInput);
    if(!inputFile.open(QIODevice::ReadOnly))
    {
        emit e("failed to open input file: " + filePathOfJsonUiFormatInput);
        emit finishedFormat2ui(false);
        return;
    }
    const QByteArray &rawJsonUiFormatInput = inputFile.readAll();
    QJsonParseError jsonParseError;
    QJsonDocument jsonDocUiFormatInput = QJsonDocument::fromJson(rawJsonUiFormatInput, &jsonParseError);
    //TODOreq: handle jsonParseError
    const QJsonObject &jsonUiFormatInput = jsonDocUiFormatInput.object();

    IUIGenerator *uiGenerator = m_UIGeneratorFactory.uiGenerator(cliArgUiTypeString);
    if(!uiGenerator->generateUi(jsonUiFormatInput)) //kek
    {
        emit e("failed to generate ui: " + filePathOfJsonUiFormatInput);
        emit finishedFormat2ui(false);
        return;
    }
    emit finishedFormat2ui(true);
#if 0
    const QMetaObject metaObject = cliArgUiTypeString2metaObject("--qtwidgets");
    IUIGenerator *uiGenerator = qobject_cast<IUIGenerator*>(metaObject.newInstance(Q_ARG(QObject*, this)));
    if(uiGenerator)
        uiGenerator->generateUi(); //kek
#endif
}
#if 0
const QMetaObject format2ui::cliArgUiTypeString2metaObject(const QString &cliArgUiTypeString)
{
    if(cliArgUiTypeString == "--qtwidgets")
    {
        return QtWidgetsUiGenerator::staticMetaObject;
    }
    //TODOreq: other ui types
    qFatal("invalid to cli arg type string");
    return nullptr;
}
#endif
