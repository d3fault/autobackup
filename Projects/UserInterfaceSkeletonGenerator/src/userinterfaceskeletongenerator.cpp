#include "userinterfaceskeletongenerator.h"

#include <QTextStream>
#include <QFile>
#include <QTemporaryDir>
#include <QMetaObject>
#include <QSet>

#include "cliimplstubgenerator.h"
#include "widgetimplstubgenerator.h"

QString UserInterfaceSkeletonGenerator::TAB = "    ";

//TODOreq: to complement my list<signals> and list<slots>, I want a list<RequestResponse_aka_SlotWithFinishedSignal>; "slot with slotFinished(bool success) signal emission contract". I want to use auto-generated scoped-destruction response-signal-emitters, which can request deferrals to wait for async operations to complete. at the end of all async operations, the coder calls requestResponse.setSuccess(true) and then simply lets the requestResponse go out of scope. deferall would naturally look like requestResponse.deferResponding() and probably a corresponding requestResponse.resumeResponsibilityForResponding() whenever the next async callback is called (right at the beginning of it, before any failures are handled of any kind (because error handling could throw!))
UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGenerator(QObject *parent)
    : QObject(parent)
{ }
UserInterfaceSkeletonGenerator::~UserInterfaceSkeletonGenerator()
{
    qDeleteAll(ImplStubGenerators);
}
void UserInterfaceSkeletonGenerator::displayFrontendBackendConnectStatements(const UserInterfaceSkeletonGeneratorData &data)
{
    //TODOmb: we could inject this shit directly into the class declaration (input to generateUserInterfaceSkeletonFromClassDeclarationString), but for now it's good enough to just print to screen and tell the user to copy/paste it :). much easier than using libclang xD
    if(data.Slots.isEmpty() && data.Signals.isEmpty())
        return; //TODOreq: we will have signals at some point too, so we need at least one of one or the other. I should probably perform this sanitization much earlier and yea just yell at the user or silently do nothing but at the very least I shouldn't get this far in the code

    QString temp("copy/paste this method into the public area of your business logic class:\n\n");

    temp.append(TAB);
    temp.append("template<class T>\n");
    temp.append(TAB);
    temp.append("static void establishConnectionsToAndFromBackendAndUi("); //this used to be non-static, backend was instead 'this', but I changed it for the following reason: a public method is generally not considered thread safe [unless it's marked a slot and used correctly]. A static public method will cue the coder that it is much more likely to be thread safe. they will investigate and see ONLY a series of connect statements, and they'll then know this is thread safe. If it was a non-static public method, they would not have investigated in the first place, they would/might have assumed [CORRECTLY, USUALLY] that it is not thread safe. I guess I could make connectToUi a slot (so it implies some thread safety if used a certain way), but..... fuck that shit I like this static. and besides, "how am I supposed to invoke that slot (without using QMetaObject::invokeMethod) if it's _IN_ the slot that the connection I would use to invoke it gets established!? dependency error."
    //TODOmb: ^I could put the BusinessClass's actual NAME in that connectBusinessClassNameGoesHereToUi method, if I want. I kinda like the stable standardized name tho...
    QString backendVarName("backend"), frontendVarName("ui");
    temp.append(data.BusinessLogiClassName + " *" + backendVarName + ", ");
    //temp.append(data.targetUserInterfaceClassName() + " *" + frontendVarName + ")\n");
    temp.append("T *" + frontendVarName + ")\n");
    temp.append(TAB + "{\n");
    Q_FOREACH(UserInterfaceSkeletonGeneratorData::SlotData slotData, data.Slots)
    {
        temp.append(TAB + TAB + "connect(" + frontendVarName + ", &T::" + slotData.correspondingSlotInvokeRequestSignalName() + ", " + backendVarName + ", &" + data.BusinessLogiClassName + "::" + slotData.slotName() + ");\n");
        //temp.append(TAB + TAB + "connect(ui->asQObject(), SIGNAL(" + slotData.correspondingRequestSignalName() + slotData.argTypesNormalizedAndWithParenthesis() + "), this, SLOT(" + slotData.SlotName + slotData.argTypesNormalizedAndWithParenthesis() + "));\n");
    }
    Q_FOREACH(UserInterfaceSkeletonGeneratorData::SignalData signalData, data.Signals)
    {
        temp.append(TAB + TAB + "connect(" + backendVarName + ", &" + data.BusinessLogiClassName + "::" + signalData.signalName() + ", " + frontendVarName + ", &T::" + signalData.correspondingSignalHandlerSlotName() + ");\n");
    }
    temp.append(TAB + "}\n");

    temp.append("\n\n");

    emit o(temp);
}
#if 0
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
#endif
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
        else if(stubType == "widget")
        {
            ImplStubGenerators.append(new WidgetImplStubGenerator());
        }
        //etc for web and shiz
    }
    generateAnyAndAllUserInterfaceImplStubs(data);
}
void UserInterfaceSkeletonGenerator::generateAnyAndAllUserInterfaceImplStubs(const UserInterfaceSkeletonGeneratorData &data)
{
    Q_FOREACH(IUserInterfaceImplStubGenerator *currentImplStubGenerator, ImplStubGenerators)
    {
        currentImplStubGenerator->generateImplStubFiles(data, m_OutputDirWithTrailingSlash);

        //tell, don't ask? go fuck yourself!
        //data.generateImplStubFiles(currentImplStubGenerator);
        //^do I "tell" the generator, or do I "tell" the data? fuck you and your short fancy sounding sentences "tell, don't ask". shit don't fuckin apply cunt. mb I should actually read an article explaining it [better] (again) before I talk shit
    }
}
//OT'ish (actually kinda on topic): I tend to code ALL of my backend before coding _any_ of my frontend (well that's true for the 'initial target' 0.1 release at least), so that means I won't have to run this ui impl skeleton generator until my api is already stabilized. also, to manually keep this hardcoded method below me in sync with the business class API is ezpz COMPARED TO changing all the ui impls and connect statements every fucking time. but yea shit once I have business class parsing (and ui impl parsing, in order to RE-gen later (tricky tricky but doable <3)), THEN this app will be fuckin sechz
void UserInterfaceSkeletonGenerator::populateDataUsingHardCodedCppXD(UserInterfaceSkeletonGeneratorData &data)
{
    data.BusinessLogiClassName = "LibFfmpeg";

    data.createAndAddSlot("void", "encodeVideo", UserInterfaceSkeletonGeneratorData::ArgsList() << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","input"} << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","output"} << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","fourCC"} ); //TODOoptimization: don't require those huge prefixes. since I'm going to be MODIFYING this code in order to USE the app [initially], it's a huge optimization xD. use a namespace or something (and do using namespace blah; at top of this file)

    data.createAndAddSignal("error", UserInterfaceSkeletonGeneratorData::ArgsList() << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","errorMessage"}); //TO DOnvm: there will be an encodeVideoFinished(bool success) signal generated on business? wait not we PARSE the business, not generate it! so nvm actually, if you want an encodeVideoFinished signal, you have to specify it (you have to "check" it to choose it when we are parsing the class decl string [for the first time]
}
void UserInterfaceSkeletonGenerator::generateUserInterfaceSkeletonFromClassDeclarationString(const QString &classDeclarationCpp_ForParsing, QList<QString> implStubShortNames)
{
    UserInterfaceSkeletonGeneratorData data;

    //TODOreq: populateDataUsingLibClang(data, classDeclarationCpp_ForParsing);
    populateDataUsingHardCodedCppXD(data);

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

    //nope: virtual inheritance interfaces suck, templated connectBackendToUi methods using a bunch of Qt5-style connect statements is way better:
    //generatePureVirtualUserInterfaceHeaderFile(data);

    generateUserInterfaceImplStubsMaybe(data, implStubShortNames); //OT'ish: doesn't the "tell, don't ask" principle violate "keep logic and data separate"? maybe I'm doin it wrong xD. wait yes I think it does! wtf. I "tell" my "data" to "do" something, rather than asking (querying) the data repeatedly. ehh fuck it who cares for now <3

    emit o("the output you desire is here: " + m_OutputDirWithTrailingSlash);
    emit finishedGeneratingUserInterfaceSkeleton(true);
}
