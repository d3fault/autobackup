#include "qtwidgetsuigenerator.h"

#include <QFile>
#include <QSet>
#include <QJsonArray>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

#define QtWidgetsUiGenerator_SOURCE_FILEPATH "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.cpp"
#define QtWidgetsUiGenerator_HEADER_FILEPATH "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"

//TODOreq: can callers handle relative subdirs? aka, will the paths be mkdir'd in dest output folder?
QString QtWidgetsUiGenerator::projectSrcDirWithSlashAppended() const
{
    return "/home/user/text/Projects/format2ui/design/src/QtWidgetsUiGeneratorOutputCompilingTemplateExample/src/";
}
QStringList QtWidgetsUiGenerator::filesToGenerate() const
{
    //return QStringList { QtWidgetsUiGenerator_SOURCE_FILEPATH, QtWidgetsUiGenerator_HEADER_FILEPATH };
    QStringList ret;
    QDir dir(projectSrcDirWithSlashAppended());
    QDirIterator dirIterator(dir, QDirIterator::Subdirectories); //TODOoptimization: cache the dir iteration results
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &fileInfo = dirIterator.fileInfo();
        if(fileInfo.isFile())
        {
            if(fileInfo.completeSuffix().toLower().endsWith("pro.user"))
                continue;
            QString filePath = fileInfo.absoluteFilePath();
            //QString relativeFilePath = filePath.right(filePath.length() - absolutePathOfCompilingTemplateExample_DirWithSlashAppended.length());
            QString relativeFilePath = dir.relativeFilePath(filePath);
            ret << relativeFilePath;
        }
    }
    return ret;
}
bool QtWidgetsUiGenerator::generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)
{
    QString theAbsoluteFilePathInWhichToGenerate = projectSrcDirWithSlashAppended() + theRelativeFilePathInWhichToGenerate;
    if(theRelativeFilePathInWhichToGenerate == QtWidgetsUiGenerator_SOURCE_FILEPATH)
    {
        if(!generateSource(theAbsoluteFilePathInWhichToGenerate, currentFileTextStream, rootUiCollector))
            return false;
    }
    else if(theRelativeFilePathInWhichToGenerate == QtWidgetsUiGenerator_HEADER_FILEPATH)
    {
        if(!generateHeader(currentFileTextStream, rootUiCollector))
            return false;
    }
    else
    {
        qDebug() << "Copying file to 'generated' verbatim:" << theRelativeFilePathInWhichToGenerate;
        QString sourceContents;
        if(!readAllFile(theAbsoluteFilePathInWhichToGenerate, &sourceContents))
            return false;
        currentFileTextStream << sourceContents;
    }
    //etc

    return true;
}
void QtWidgetsUiGenerator::addSpecialFilesContentMarkers(SpecialFilesContentsType *out_SpecialFilesContents)
{
    out_SpecialFilesContents->insert("top5movieslistwidget.h", QString());
    out_SpecialFilesContents->insert("top5movieslistwidget.cpp", QString());
}
bool QtWidgetsUiGenerator::generateSource(const QString &absoluteFilePathOfSourceFileToGenerate, QTextStream &currentFileTextStream, const UICollector &rootUiCollector)
{
    //read in source from compiling template example
    QString compilingTemplateExampleSource;
    if(!readAllFile(absoluteFilePathOfSourceFileToGenerate, &compilingTemplateExampleSource))
        return false;

    //strReplace shiz on the file contents

    m_WhatToReplaceItWith0_liiueri93jrkjieruj.clear();
    m_WhatToReplaceItWith1_kldsfoiure8098347824.clear();
    m_WhatToReplaceItWith2_lksdjoirueo230480894.clear();
    m_WhatToReplaceItWith3_lksdfjoiduf08340983409.clear();
    m_WhatToReplaceItWith4_ldkjsflj238423084.clear();
    m_WhatToReplaceItWith5_lksdfjodusodsfudsflkjdskl983402824.clear();

    QString whatToSearchFor6("#include \"top5movieslistwidget.h\"");
    m_WhatToReplaceItWith6.clear();

    m_FirstNonWidget = true;
    m_FirstWidget = true;
    recursivelyProcessUiCollectorForSource(rootUiCollector);

    replaceSpecialCommentSection(&compilingTemplateExampleSource, "liiueri93jrkjieruj", m_WhatToReplaceItWith0_liiueri93jrkjieruj);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "kldsfoiure8098347824", m_WhatToReplaceItWith1_kldsfoiure8098347824);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "lksdjoirueo230480894", m_WhatToReplaceItWith2_lksdjoirueo230480894);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "lksdfjoiduf08340983409", m_WhatToReplaceItWith3_lksdfjoiduf08340983409);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "ldkjsflj238423084" , m_WhatToReplaceItWith4_ldkjsflj238423084);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "lksdfjodusodsfudsflkjdskl983402824", m_WhatToReplaceItWith5_lksdfjodusodsfudsflkjdskl983402824);
    compilingTemplateExampleSource.replace(whatToSearchFor6, m_WhatToReplaceItWith6);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleSource;

    return true;
}
void QtWidgetsUiGenerator::recursivelyProcessUiCollectorForSource(const UICollector &uiCollector)
{
    if(uiCollector.Type != UICollectorType::Widget)
    {
        m_WhatToReplaceItWith5_lksdfjodusodsfudsflkjdskl983402824 += QString(m_FirstNonWidget ? "" : ", ") + uiCollector.variableName();
        m_FirstNonWidget = false;
    }
    switch(uiCollector.Type)
    {
        case UICollectorType::LineEdit_String:
            {
                QString lineEditMemberName = lineEditMemberVariableName(uiCollector.variableName());
                m_WhatToReplaceItWith0_liiueri93jrkjieruj += "    , " + lineEditMemberName + "(new QLineEdit())\n";
                m_WhatToReplaceItWith1_kldsfoiure8098347824 += "    " + lineEditMemberName + "->setPlaceholderText(\"" + uiCollector.humanReadableNameForShowingFinalEndUser() + ":\");\n    " + currentParentLayoutName() + "->addWidget(" + lineEditMemberName + ");\n";
                m_WhatToReplaceItWith2_lksdjoirueo230480894 += "    connect(" + lineEditMemberName + ", &QLineEdit::returnPressed, okButton, &QPushButton::click);\n";
                m_WhatToReplaceItWith3_lksdfjoiduf08340983409 += "        if(!m_ArgParser." + uiCollector.variableName() + "DefaultValueParsedFromProcessArg().isEmpty())\n            " + lineEditMemberName + "->setText(m_ArgParser." + uiCollector.variableName() + "DefaultValueParsedFromProcessArg());\n";
                m_WhatToReplaceItWith4_ldkjsflj238423084 += "    QString " + uiCollector.variableName() + " = " + lineEditMemberName + "->text();\n    if(" + uiCollector.variableName() + ".isEmpty())\n    {\n        QMessageBox::critical(this, \"Error\", \"" + uiCollector.humanReadableNameForShowingFinalEndUser() + " cannot be empty\");\n        return;\n    }\n";
            }
        break;
        case UICollectorType::PlainTextEdit_StringList:
            {
                QString listWidgetMemberName = listWidgetMemberVariableName(uiCollector.variableName());
                QString listWidgetTypeString = listWidgetTypeName(uiCollector.variableName());
                m_WhatToReplaceItWith0_liiueri93jrkjieruj += "    , " + listWidgetMemberName + "(new " + listWidgetTypeString + "())\n";
                m_WhatToReplaceItWith1_kldsfoiure8098347824 += "    " + currentParentLayoutName() + "->addWidget(new QLabel(\"" + uiCollector.humanReadableNameForShowingFinalEndUser() + "\"));\n    " + currentParentLayoutName() + "->addWidget(" + listWidgetMemberName + ");\n";

                m_WhatToReplaceItWith4_ldkjsflj238423084 += "    QStringList " + uiCollector.variableName() + " = " + listWidgetMemberName + "->" + uiCollector.variableName() + "();\n";
                m_WhatToReplaceItWith6 += "#include \"" + listWidgetTypeString.toLower() + ".h\"\n";
                addInstanceOfSpecialFile("top5movieslistwidget.h", listWidgetTypeString);
                addInstanceOfSpecialFile("top5movieslistwidget.cpp", listWidgetTypeString);
            }
        break;
            //etc
            //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh
        case UICollectorType::Widget:
            {
                QString layoutName;
                if(m_FirstWidget)
                    layoutName = "rootLayout";
                else
                    layoutName = "layout" + QString::number(getNextUnusedLayoutInt());

                QString parentLayoutName = currentParentLayoutName();
                m_WhatToReplaceItWith1_kldsfoiure8098347824 += "    QVBoxLayout *" + layoutName + " = new QVBoxLayout(" + (m_FirstWidget ? "this" : "") + ");\n";
                setCurrentParentLayoutName(layoutName); //become the parent layout for future widgets
                //iterate uiCollector's rootLayout, calling generateSource for each IWidget (Widget or DerivedFromWidget) therein
                const QJsonObject &rootLayout = uiCollector.rootLayout();
                const QJsonArray &verticalUiVariables = rootLayout.value("verticalUiVariables").toArray();
                QString codeGenToAppendAfterIteratingOurChildren = "";
                if(!m_FirstWidget)
                {
                    //rootLayout->addLayout(nestedLayout0);
                    //rootLayout doesn't need to do this, took care of this in it's constructor (besides it'd be QWidget::setLayout instead!~)
                    codeGenToAppendAfterIteratingOurChildren += "    " + parentLayoutName + "->addLayout(" + layoutName + ");\n";

                }
                m_FirstWidget = false;
                qDebug() << "Widget has" << verticalUiVariables.size() << " sub-widgets";
                for(QJsonArray::const_iterator it = verticalUiVariables.constBegin(); it != verticalUiVariables.constEnd(); ++it)
                {
                    const QJsonObject &currentUiCollectorJsonObject = (*it).toObject();
                    UICollector currentUiCollector(currentUiCollectorJsonObject);
                    recursivelyProcessUiCollectorForSource(currentUiCollector);
                }
                m_WhatToReplaceItWith1_kldsfoiure8098347824 += codeGenToAppendAfterIteratingOurChildren; //we needed to set m_FirstWidget to false before doing the recursive call
            }
        break;
        default:
            qWarning() << "unknown UiCollector type";
        break;
    }
}
bool QtWidgetsUiGenerator::generateHeader(QTextStream &currentFileTextStream, const UICollector &rootUiCollector)
{
    //read in header from compiling template example
    QFile compilingTemplateExampleHeaderFile("/home/user/text/Projects/format2ui/design/src/QtWidgetsUiGeneratorOutputCompilingTemplateExample/src/qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h");
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
    QString m_WhatToReplaceItWith0("../uishared/" + commandLineParserObjectName.toLower() + ".h");

    QString whatToLookFor1("FirstNameLastNameQObjectCommandLineOptionParser");
    QString m_WhatToReplaceItWith1(commandLineParserObjectName);

    //QString whatToLookFor2 = "    QLineEdit *m_FirstNameLineEdit;\n    QLineEdit *m_LastNameLineEdit;";
    QString m_WhatToReplaceItWith2_lskjdfouewr08084097342098;

    QString whatToLookFor3("const QString &firstName, const QString &lastName, const QStringList &top5Movies");
    QString m_WhatToReplaceItWith3;

    //QString whatToLookFor4("class QLineEdit;");
    QSet<QString> m_WhatToReplaceItWith4_forwardDeclareClasses_dsfsflsjdflkjowe0834082934;

#if 0
    bool first = true;
    for(QList<UICollector>::const_iterator it = uiCollector.UIVariables.constBegin(); it != uiCollector.UIVariables.constEnd(); ++it)
    {
        const UICollector &uiVariable = *it;
        switch(uiVariable.Type)
        {
            case UICollectorType::LineEdit_String:
                {
                    m_WhatToReplaceItWith4_forwardDeclareClasses_dsfsflsjdflkjowe0834082934.insert("QLineEdit");
                    QString lineEditMemberName = lineEditMemberVariableName(uiVariable.VariableName);
                    m_WhatToReplaceItWith2_lskjdfouewr08084097342098 += "    QLineEdit *" + lineEditMemberName + ";\n";
                    m_WhatToReplaceItWith3 += QString(first ? "" : ", ") + QString("const QString &") + uiVariable.VariableName;
                }
            break;
            case UICollectorType::PlainTextEdit_StringList:
                {
                    m_WhatToReplaceItWith4_forwardDeclareClasses_dsfsflsjdflkjowe0834082934.insert("QPlainTextEdit");
                    m_WhatToReplaceItWith2_lskjdfouewr08084097342098 += "    QPlainTextEdit *" + plainTextEditMemberVariableName(uiVariable.VariableName) + ";\n";
                    m_WhatToReplaceItWith3 += QString(first ? "" : ", ") + QString("const QStringList &") + uiVariable.VariableName;
                }
            break;
                //etc
            default:
                qWarning("unknown uiVariable type");
            break;
        }

        first = false;
    }
#endif

    QString m_WhatToReplaceItWith4_dsfsflsjdflkjowe0834082934;
    for(QSet<QString>::const_iterator it = m_WhatToReplaceItWith4_forwardDeclareClasses_dsfsflsjdflkjowe0834082934.constBegin(); it != m_WhatToReplaceItWith4_forwardDeclareClasses_dsfsflsjdflkjowe0834082934.constEnd(); ++it)
    {
        m_WhatToReplaceItWith4_dsfsflsjdflkjowe0834082934 += "class " + (*it) + ";\n";
    }

    compilingTemplateExampleHeader.replace(whatToLookFor0, m_WhatToReplaceItWith0);
    compilingTemplateExampleHeader.replace(whatToLookFor1, m_WhatToReplaceItWith1);
    //compilingTemplateExampleHeader.replace(whatToLookFor2, m_WhatToReplaceItWith2_lskjdfouewr08084097342098);
    replaceSpecialCommentSection(&compilingTemplateExampleHeader, "lskjdfouewr08084097342098", m_WhatToReplaceItWith2_lskjdfouewr08084097342098);
    compilingTemplateExampleHeader.replace(whatToLookFor3, m_WhatToReplaceItWith3);
    //compilingTemplateExampleHeader.replace(whatToLookFor4, m_WhatToReplaceItWith4_dsfsflsjdflkjowe0834082934);
    replaceSpecialCommentSection(&compilingTemplateExampleHeader, "dsfsflsjdflkjowe0834082934", m_WhatToReplaceItWith4_dsfsflsjdflkjowe0834082934);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleHeader;

    return true;
}
int QtWidgetsUiGenerator::getNextUnusedLayoutInt()
{
    int ret = m_CurrentLayoutInt;
    ++m_CurrentLayoutInt;
    return ret;
}
QString QtWidgetsUiGenerator::currentParentLayoutName() const
{
    if(m_LayoutParentStack.isEmpty())
        return "rootLayout";
    return m_LayoutParentStack.top();
}
void QtWidgetsUiGenerator::setCurrentParentLayoutName(const QString &parentLayoutName)
{
    m_LayoutParentStack.push(parentLayoutName);
}
QString QtWidgetsUiGenerator::lineEditMemberVariableName(const QString &variableName)
{
    QString ret = "m_" + firstLetterToUpper(variableName) + "LineEdit";
    return ret;
}
QString QtWidgetsUiGenerator::listWidgetTypeName(const QString &variableName)
{
    QString ret = firstLetterToUpper(variableName) + "ListWidget";
    return ret;
}
QString QtWidgetsUiGenerator::listWidgetMemberVariableName(const QString &variableName)
{
    QString ret = "m_" + listWidgetTypeName(variableName);
    return ret;
}
QString QtWidgetsUiGenerator::strReplaceSpecialFile(const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile)
{
    QString ret = specialFilesContents().value(relativeFilePathOfSpecialFile);// = TO DOnereq; put file contents into ret for initial state

#if 0
    if(relativeFilePathOfSpecialFile == "top5movieslistwidget.h")
    {
#endif
        if(relativeFilePathOfSpecialFile == "top5movieslistwidget.cpp" || relativeFilePathOfSpecialFile == "top5movieslistwidget.h")
        {
            QString Top5Movies("Top5Movies");
            //TODOreq:
            ret.replace(Top5Movies, classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile);
            ret.replace(Top5Movies.toUpper(), classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile.toUpper());
            ret.replace(firstLetterToLower(Top5Movies), firstLetterToLower(classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile));
#if 0
    }
    else if(relativeFilePathOfSpecialFile == "top5movieslistwidget.cpp")
    {
#endif
            ret.replace(Top5Movies.toLower(), classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile.toLower());
        }
#if 0
    }
#endif

    return ret;
}
QString QtWidgetsUiGenerator::getOutputFilePathFromRelativeFilePath(const QString &outputPathWithSlashAppended, const QString &relativeFilePathOfSpecialFile, const QString &classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile)
{
    QString ret = outputPathWithSlashAppended;
    ret.append(classNameToBeSubstitutedInDuringStrReplaceHacksInSpecialFile.toLower());
    if(relativeFilePathOfSpecialFile == "top5movieslistwidget.cpp")
    {
        ret.append("listwidget.cpp");
    }
    else if(relativeFilePathOfSpecialFile == "top5movieslistwidget.h")
    {
        ret.append("listwidget.h");
    }
    else
    {
        qFatal("unknown blah this shouldn't have happened dlksjfldjsou08340293");
    }
    return ret;
}
