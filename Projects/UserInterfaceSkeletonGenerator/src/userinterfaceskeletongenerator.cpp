#include "userinterfaceskeletongenerator.h"

#include <QTextStream>
#include <QFile>
#include <QTemporaryDir>
#include <QMetaObject>
#include <QSet>
#include <QDebug>

#include "cliimplstubgenerator.h"

QString UserInterfaceSkeletonGenerator::TAB = "    ";

UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGenerator(QObject *parent)
    : QObject(parent)
{
    connect(this, SIGNAL(e(QString)), this, SLOT(handleDbg(QString)));
    connect(this, SIGNAL(o(QString)), this, SLOT(handleDbg(QString)));
}
UserInterfaceSkeletonGenerator::~UserInterfaceSkeletonGenerator()
{
    qDeleteAll(ImplStubGenerators);
}
void UserInterfaceSkeletonGenerator::displayFrontendBackendConnectStatements(const UserInterfaceSkeletonGeneratorData &data)
{
    //TODOmb: we could inject this shit directly into the class declaration (input to generateUserInterfaceSkeletonFromClassDeclarationString), but for now it's good enough to just print to screen and tell the user to copy/paste it :). much easier than using libclang xD
    if(data.Slots.isEmpty() /*&& data.Signals.isEmpty()*/)
        return; //TODOreq: we will have signals at some point too, so we need at least one of one or the other

    QString temp("copy/paste this method into the public area of your business logic class:\n\n");

    temp.append(TAB);
    temp.append("inline void connectToUi(");
    temp.append(data.targetUserInterfaceClassName() + "*ui)\n");
    temp.append(TAB + "{\n");
    Q_FOREACH(UserInterfaceSkeletonGeneratorData::SlotData slotData, data.Slots)
    {
        //Qt5 fail: temp.append(TAB + TAB + "connect(ui->asQObject(), &" + data.targetUserInterfaceClassName() + "::" + slotData.correspondingRequestSignalName() + ", this, &" + data.BusinessLogiClassName + "::" + slotData.SlotName + ");\n"); //TODOreq: use Qt4 string-based syntax, Qt5 style doesn't work here!
        temp.append(TAB + TAB + "connect(ui->asQObject(), SIGNAL(" + slotData.correspondingRequestSignalName() + slotData.argTypesNormalizedAndWithParenthesis() + "), this, SLOT(" + slotData.SlotName + slotData.argTypesNormalizedAndWithParenthesis() + "));\n");
    }
    temp.append(TAB + "}\n");

    temp.append("\n\n");

    emit o(temp);
}
void UserInterfaceSkeletonGenerator::generatePureVirtualUserInterfaceHeaderFile(const UserInterfaceSkeletonGeneratorData &data)
{
    QFile file(m_OutputDirWithTrailingSlash + data.targetUserInterfaceClassName().toLower() + ".h");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open file: " + file.fileName());
        emit finishedGeneratingUserInterfaceSkeleton(false); //oh right, "finished" comes before corresponding slot name, not after ;-P
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);
    data.generatePureVirtualUserInterfaceHeaderFile(textStream);
}
void UserInterfaceSkeletonGenerator::generateUserInterfaceImplStubsMaybe(const UserInterfaceSkeletonGeneratorData &data, QList<QString> implStubShortNames)
{
    QSet<QString> deDuped = QSet<QString>::fromList(implStubShortNames);
    Q_FOREACH(QString currentImplStubShortName, deDuped)
    {
        QString stubType = currentImplStubShortName.toLower();
        if(stubType == "cli")
        {
            ImplStubGenerators.append(new CliImplStubGenerator());
        }
        else if(stubType == "gui")
        {
            //TODOreq: //ImplStubGenerators.append(new GuiImplStubGenerator());
        }
        //etc for web and shiz
    }
    generateAnyAndAllUserInterfaceImplStubs(data);
}
void UserInterfaceSkeletonGenerator::generateAnyAndAllUserInterfaceImplStubs(const UserInterfaceSkeletonGeneratorData &data)
{
    Q_FOREACH(IUserInterfaceImplStubGenerator *currentImplStubGenerator, ImplStubGenerators)
    {
        currentImplStubGenerator->generateImplHeaderAndSourceStubFiles(data, m_OutputDirWithTrailingSlash);

        //tell, don't ask? go fuck yourself!
        //data.generateImplHeaderAndSourceStubFiles(currentImplStubGenerator);
        //^do I "tell" the generator, or do I "tell" the data? fuck you and your short fancy sounding sentences "tell, don't ask". shit don't fuckin apply cunt. mb I should actually read an article explaining it [better] (again) before I talk shit
    }
}
void UserInterfaceSkeletonGenerator::generateUserInterfaceSkeletonFromClassDeclarationString(const QString &classDeclarationCpp_ForParsing, QList<QString> implStubShortNames)
{
    UserInterfaceSkeletonGeneratorData data; //TODOreq: data gets populated, with the help of libclang, from classDeclarationCpp_ForParsing
    data.BusinessLogiClassName = "LibFfmpeg";

    data.createAndAddSlot("bool", "encodeVideo", UserInterfaceSkeletonGeneratorData::ArgsList() << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","input"} << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","output"} << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","fourCC"} ); //TODOoptimization: don't require those huge prefixes. since I'm going to be MODIFYING this code in order to USE the app [initially], it's a huge optimization xD. use a namespace or something (and do using namespace blah; at top of this file)

    data.createAndAddSlot("void", "fuck");

    generateUserInterfaceSkeletonFromData(data, implStubShortNames);
}
void UserInterfaceSkeletonGenerator::generateUserInterfaceSkeletonFromData(const UserInterfaceSkeletonGeneratorData &data, QList<QString> implStubShortNames)
{
    QTemporaryDir outputDir;
    if(!outputDir.isValid())
    {
        emit e("failed to create output dir:" + outputDir.path());
        emit finishedGeneratingUserInterfaceSkeleton(false);
        return;
    }
    outputDir.setAutoRemove(false);
    m_OutputDirWithTrailingSlash = appendSlashIfNeeded(outputDir.path());

    displayFrontendBackendConnectStatements(data);
    generatePureVirtualUserInterfaceHeaderFile(data);
    generateUserInterfaceImplStubsMaybe(data, implStubShortNames); //OT'ish: doesn't the "tell, don't ask" principle violate "keep logic and data separate"? maybe I'm doin it wrong xD. wait yes I think it does! wtf. I "tell" my "data" to "do" something, rather than asking (querying) the data repeatedly. ehh fuck it who cares for now <3

    emit o("the output you desire is here: " + m_OutputDirWithTrailingSlash);
    emit finishedGeneratingUserInterfaceSkeleton(true);
}
void UserInterfaceSkeletonGenerator::handleDbg(QString msg)
{
    qDebug() << msg; //TODOreq: proper
}
