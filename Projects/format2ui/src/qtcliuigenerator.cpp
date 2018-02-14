#include "qtcliuigenerator.h"

#include <QFile>
#include <QJsonArray>
#include <QDebug>

#define QtCliUiGenerator_SOURCE_FILEPATH "qtcliuigeneratoroutputcompilingtemplateexample.cpp"
#define QtCliUiGenerator_HEADER_FILEPATH "qtcliuigeneratoroutputcompilingtemplateexample.h"

bool QtCliUiGenerator::generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)
{
    QString theAbsoluteFilePathInWhichToGenerate = absolutePathOfCompilingTemplateExampleProjectSrcDir_WithSlashAppended() + theRelativeFilePathInWhichToGenerate;
    if(theRelativeFilePathInWhichToGenerate == QtCliUiGenerator_SOURCE_FILEPATH)
    {
        if(!generateSource(theAbsoluteFilePathInWhichToGenerate, currentFileTextStream, rootUiCollector))
            return false;
    }
    else if(theRelativeFilePathInWhichToGenerate == QtCliUiGenerator_HEADER_FILEPATH)
    {
        if(!generateHeader(/*theAbsoluteFilePathInWhichToGenerate, */currentFileTextStream, rootUiCollector))
            return false;
    }
    else
    {
        if(!copyFileVerbatimWithMessage(theRelativeFilePathInWhichToGenerate, theAbsoluteFilePathInWhichToGenerate, currentFileTextStream))
            return false;
    }
    //etc

    return true;
}
void QtCliUiGenerator::addTriggeredFilesContentMarkers(TriggeredFilesContentsType *out_TriggeredFilesContents)
{
    Q_UNUSED(out_TriggeredFilesContents); //QtCliUiGenerator doesn't have any triggered files at the moment
}
QString QtCliUiGenerator::strReplaceTriggeredFile(const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile, const UICollector &uiCollector)
{
    //QtCliUiGenerator doesn't have any triggered files at the moment
    Q_UNUSED(relativeFilePathOfTriggeredFile);
    Q_UNUSED(classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile);
    Q_UNUSED(uiCollector);
    return QString();
}
QString QtCliUiGenerator::getOutputFilePathForTriggeredFileFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfTriggeredFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile)
{
    Q_UNUSED(outputPathWithSlashAppended);
    Q_UNUSED(relativeFilePathOfTriggeredFile);
    Q_UNUSED(classNameToBeSubstitutedInDuringStrReplaceHacksInTriggeredFile);
    qFatal("QtCliUiGenerator doesn't have any triggered files at the moment");
    return QString();
}
bool QtCliUiGenerator::generateSource(const QString &absoluteFilePathOfSourceFileToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)
{
    //isn't it better (faster, ezier, smarter) to generate code by copy/pasting the "compiling template example" and then strReplacing the fuck out of THAT? I mean it genuinely sounds like it'd be LESS work (and changes to the compiling template example needn't NECESSARILY require any changes to format2ui (although often times, changes _are_ necessary (more strReplace ez shit)). in any case, I'm surely going to experiment with the file-copy->strReplace strategy :-D. I might learn it sucks, but I'll learn something in any case!
    //TODOreq: the compiling template example(s) would then become project resources (.qrc) to be shipped/deployed inside the binary! neat

    //read in source from compiling template example
    QString compilingTemplateExampleSource;
    if(!readAllFile(absoluteFilePathOfSourceFileToGenerate, &compilingTemplateExampleSource))
        return false;

    //strReplace shiz on the file contents

    //QString whatToLookFor0 = "    QString firstName = queryString(\"First Name\", m_ArgParser.firstNameDefaultValueParsedFromProcessArg());\n    QString lastName = queryString(\"Last Name\", m_ArgParser.lastNameDefaultValueParsedFromProcessArg());";
    m_WhatToReplaceItWith0_sdlkfjouedsflkjsdlf0983048324.clear();

    //QString whatToLookFor1 = "firstName, lastName, top5Movies";
    m_WhatToReplaceItWith1_lsdkjflkdjsfouerooiuwerlkjou89234098234.clear();

    m_FirstNonWidget = true;
    recursivelyProcessUiCollectorForSource(rootUiCollector);

    //compilingTemplateExampleSource.replace(whatToLookFor0, whatToReplaceItWith0_sdlkfjouedsflkjsdlf0983048324);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "sdlkfjouedsflkjsdlf0983048324", m_WhatToReplaceItWith0_sdlkfjouedsflkjsdlf0983048324);
    //compilingTemplateExampleSource.replace(whatToLookFor1, m_WhatToReplaceItWith1_lsdkjflkdjsfouerooiuwerlkjou89234098234);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "lsdkjflkdjsfouerooiuwerlkjou89234098234", m_WhatToReplaceItWith1_lsdkjflkdjsfouerooiuwerlkjou89234098234);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleSource;

    return true;
}
void QtCliUiGenerator::recursivelyProcessUiCollectorForSource(const UICollector &uiCollector)
{
    switch(uiCollector.Type)
    {
        case UICollectorType::LineEdit_String:
            {
                m_WhatToReplaceItWith0_sdlkfjouedsflkjsdlf0983048324 += "    QString " + uiCollector.variableName() + " = query(\"" + uiCollector.humanReadableNameForShowingFinalEndUser() + "\", m_ArgParser." + uiCollector.variableName() + "DefaultValueParsedFromProcessArg());\n";
            }
        break;
        case UICollectorType::PlainTextEdit_StringList:
            {
                m_WhatToReplaceItWith0_sdlkfjouedsflkjsdlf0983048324 += "    QStringList top5Movies = queryStringList(\"" + uiCollector.humanReadableNameForShowingFinalEndUser() + "\", m_ArgParser." + uiCollector.variableName() + "DefaultValueParsedFromProcessArg());";
            }
        break;
            //etc
            //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh
        case UICollectorType::Widget:
            {
                //iterate uiCollector's rootLayout, calling generateSource for each IWidget (Widget or DerivedFromWidget) therein
                const QJsonObject &rootLayout = uiCollector.rootLayout();
                const QJsonArray &verticalUiVariables = rootLayout.value("verticalUiVariables").toArray();
                qDebug() << "Widget has" << verticalUiVariables.size() << " sub-widgets";
                for(auto &&i : verticalUiVariables)
                {
                    const QJsonObject &currentUiCollectorJsonObject = i.toObject();
                    UICollector currentUiCollector(currentUiCollectorJsonObject);
                    recursivelyProcessUiCollectorForSource(currentUiCollector);
                }
            }
        break;
        default:
            qWarning() << "unknown UiCollector type";
        break;
    }
    if(uiCollector.Type != UICollectorType::Widget)
    {
        m_WhatToReplaceItWith1_lsdkjflkdjsfouerooiuwerlkjou89234098234 += (m_FirstNonWidget ? "" : ", ") + uiCollector.variableName();
        m_FirstNonWidget = false;
    }
}
bool QtCliUiGenerator::generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector)
{
    //read in header from compiling template example
    QFile compilingTemplateExampleHeaderFile("/home/user/text/Projects/format2ui/design/src/QtCliUiGeneratorOutputCompilingTemplateExample/src/qtcliuigeneratoroutputcompilingtemplateexample.h");
    if(!compilingTemplateExampleHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "failed to open file for reading:" << compilingTemplateExampleHeaderFile.fileName();
        return false;
    }
    QTextStream compilingTemplateExampleHeaderTextStream(&compilingTemplateExampleHeaderFile);
    QString compilingTemplateExampleHeader = compilingTemplateExampleHeaderTextStream.readAll();

    //strReplace shiz on the file contents

    QString commandLineParserObjectName = rootUiCollector.name() + "CommandLineOptionParser";

    QString whatToLookFor0 = "../../uishared/firstnamelastnameqobjectcommandlineoptionparser.h";
    m_WhatToReplaceItWith0 = "../uishared/" + commandLineParserObjectName.toLower() + ".h";

    QString whatToLookFor1("FirstNameLastNameQObjectCommandLineOptionParser");
    m_WhatToReplaceItWith1 = commandLineParserObjectName;

    QString whatToLookFor2("const QString &firstName, const QString &lastName, const QStringList &top5Movies");
    m_WhatToReplaceItWith2.clear();

    m_FirstNonWidget = true;
    recursivelyProcessUiCollectorForHeader(rootUiCollector);

    compilingTemplateExampleHeader.replace(whatToLookFor0, m_WhatToReplaceItWith0);
    compilingTemplateExampleHeader.replace(whatToLookFor1, m_WhatToReplaceItWith1);
    compilingTemplateExampleHeader.replace(whatToLookFor2, m_WhatToReplaceItWith2);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleHeader;

    return true;
}
void QtCliUiGenerator::recursivelyProcessUiCollectorForHeader(const UICollector &uiCollector)
{
    if(uiCollector.Type != UICollectorType::Widget)
    {
        m_WhatToReplaceItWith2 += QString(m_FirstNonWidget ? "" : ", ") + QString("const ");
    }
    switch(uiCollector.Type)
    {
        case UICollectorType::LineEdit_String:
            {
                m_WhatToReplaceItWith2 += "QString";
            }
        break;
        case UICollectorType::PlainTextEdit_StringList:
            {
                m_WhatToReplaceItWith2 += "QStringList";
            }
        break;
            //etc
            //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh
        case UICollectorType::Widget:
            {
                //iterate uiCollector's rootLayout, calling generateSource for each IWidget (Widget or DerivedFromWidget) therein
                const QJsonObject &rootLayout = uiCollector.rootLayout();
                const QJsonArray &verticalUiVariables = rootLayout.value("verticalUiVariables").toArray();
                qDebug() << "Widget has" << verticalUiVariables.size() << " sub-widgets";
                for(auto &&i : verticalUiVariables)
                {
                    const QJsonObject &currentUiCollectorJsonObject = i.toObject();
                    UICollector currentUiCollector(currentUiCollectorJsonObject);
                    recursivelyProcessUiCollectorForHeader(currentUiCollector);
                }
            }
        break;
        default:
            qWarning() << "unknown UiCollector type";
        break;
    }
    if(uiCollector.Type != UICollectorType::Widget)
    {
        m_WhatToReplaceItWith2 += " &" + uiCollector.variableName();
        m_FirstNonWidget = false;
    }
}
QString QtCliUiGenerator::absolutePathOfCompilingTemplateExampleProjectSrcDir_WithSlashAppended() const
{
    return "/home/user/text/Projects/format2ui/design/src/QtCliUiGeneratorOutputCompilingTemplateExample/src/";
}
