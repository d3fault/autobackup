#include "format2ui.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include "iuigenerator.h"
#include "uigeneratorformat.h"

format2ui::format2ui(QObject *parent)
    : QObject(parent)
{ }
void format2ui::beginFormat2ui(const QString &filePathOfJsonUiFormatInput, const QString &cliArgUiTypeString)
{
    QFile inputFile(filePathOfJsonUiFormatInput);
    if(!inputFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "failed to open input file:" << filePathOfJsonUiFormatInput;
        emit finishedFormat2ui(false);
        return;
    }
    const QByteArray &rawJsonUiFormatInput = inputFile.readAll();
    QJsonParseError jsonParseError;
    QJsonDocument jsonDocUiFormatInput = QJsonDocument::fromJson(rawJsonUiFormatInput, &jsonParseError);
    if(jsonParseError.error != QJsonParseError::NoError)
    {
        qWarning() << "json parse error:" << jsonParseError.errorString();
    }
    const QJsonObject &jsonUiFormatInput = jsonDocUiFormatInput.object();

    IUIGenerator *uiGenerator = m_UIGeneratorFactory.uiGenerator(cliArgUiTypeString);
    if(uiGenerator == nullptr)
    {
        qCritical() << "couldn't find ui generator for flag:" << cliArgUiTypeString;
        emit finishedFormat2ui(false);
        return;
    }

    //UIGeneratorFormat format;
    //parseJsonUiFormatInput(jsonUiFormatInput, &format);
    //so now uiVariables is populated, time to iterate it and output shit (I guess this could be done during input _reading_, but eh stfu I can't think that pro. can probably refactor to that later ezily anyways -- hmm actually I think the above 'input reading' might be generic enough to even put in our interface/abstract-base-class for re-use in other sibling ui generator implementations (such as cli,qml,etc))
    UICollector rootUiCollector(jsonUiFormatInput);

    if(!uiGenerator->generateUi(rootUiCollector))
    {
        qDebug() << "failed to generate ui";
        emit finishedFormat2ui(false);
        return;
    }
    qDebug() << "Generated UI";
    emit finishedFormat2ui(true);
#if 0
    const QMetaObject metaObject = cliArgUiTypeString2metaObject("--qtwidgets");
    IUIGenerator *uiGenerator = qobject_cast<IUIGenerator*>(metaObject.newInstance(Q_ARG(QObject*, this)));
    if(uiGenerator)
        uiGenerator->generateUi(); //kek
#endif
}
#if 0
//recursion head: void format2ui::parseJsonUiFormatInput
void format2ui::parseJsonUiFormatInput(const QJsonObject &inputJsonObject, UIGeneratorFormat *out_Format)
{
    UICollector thisUiCollector(/*typeString*/); //, humanReadableNameForShowingFinalEndUser, variableName, variableValue);
    switch(thisUiCollector.Type)
    {
        case UICollectorType::Widget:
            handleWidgetAkaContainerType(thisUiCollector, inputJsonObject, out_Format); //recursion path
            break;
        default:
            handleDerivedFromWidgetAkaActualType(thisUiCollector, inputJsonObject, out_Format); //non-recursion path
            break;
    }
    out_Format->UIVariables.append(thisUiCollector);
}
//everything inherits from widgets, yes, but only a widget by itself is considered a container
void format2ui::handleWidgetAkaContainerType(const UICollector &uiVariable, const QJsonObject &inputJsonObject, UIGeneratorFormat *out_Format)
{
    //out_Format->Name = inputJsonObject.value("name").toString(); //TODOreq: only want the TOP-LEVEL name, not each sub-widget to overwrite this value
    //uiVariable.setName(value("name")) wot

    //a ui format input is a series of layouts (containing "widgets" therein)
    //so as not to digress too fucking much, I'll start with a QVBoxLayout as my "root" layout. I use QVBoxLayout the most
    const QJsonObject &rootLayout = inputJsonObject.value("rootLayout").toObject();
    //I could have returned an array, but dealing with objects is ezier imo -- (ed note: we return an array but later actually)
    //maybe rootLayout _should_ be an array, however eg QGridLayout doesn't make sense as an array!!
    const QJsonArray &verticalUiVariables = rootLayout.value("verticalUiVariables").toArray();
    for(QJsonArray::const_iterator it = verticalUiVariables.constBegin(); it != verticalUiVariables.constEnd(); ++it)
    {
        const QJsonObject &currentUiVariableJsonObject = (*it).toObject();
        parseJsonUiFormatInput(currentUiVariableJsonObject, out_Format); //recursion call to head: void format2ui::parseJsonUiFormatInput
#if 0
        QString typeString2 = currentUiVariableJsonObject.value("typeString").toString();
        QString humanReadableNameForShowingFinalEndUser = currentUiVariableJsonObject.value("humanReadableNameForShowingFinalEndUser").toString();
        QString variableName = currentUiVariableJsonObject.value("variableName").toString(); //probably gonna take this out. not using it like I thought I would be
        QString variableValue = currentUiVariableJsonObject.value("variableValue").toString();
#endif

        //UIVariable uiVariable(typeString2, humanReadableNameForShowingFinalEndUser, variableName, variableValue);
        //handleWidgetAkaContainerType(uiVariable, out_Format);
#if 0
        if(uiVariable.Type == UIVariableType::Widget)
        {
            //recursion call to head: void format2ui::parseJsonUiFormatInput
            parseJsonUiFormatInput(currentUiVariableJsonObject, out_Format);
        }
#endif

        //out_Format->UIVariables.append(uiVariable);
    }
}
void format2ui::handleDerivedFromWidgetAkaActualType(const UICollector &uiVariable, const QJsonObject &inputJsonObject, UIGeneratorFormat *out_Format)
{
    //uhh, hi? wat do here?
}
#endif
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
