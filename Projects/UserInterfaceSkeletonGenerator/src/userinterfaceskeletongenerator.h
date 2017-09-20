#ifndef USERINTERFACESKELETONGENERATOR_H
#define USERINTERFACESKELETONGENERATOR_H

#include <QObject>

class QTextStream;

class UserInterfaceSkeletonGenerator : public QObject
{
    Q_OBJECT
public:
    static QString TAB;
    struct UserInterfaceSkeletonGeneratorData
    {
        struct SingleArg
        {
            QString ArgType;
            QString ArgName;
        };
        typedef QList<SingleArg> ArgsList;

        struct SlotData
        {
            QString SlotReturnType;
            QString SlotName;
            ArgsList SlotArgs;

            QString argsWithoutParenthesis() const;
            QString argsWithParenthesis() const;

            QString correspondingRequestSignalName() const;
        };

        void createAndAddSlot(QString slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType/*TODOreq*/ /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsList slotArgs = ArgsList());

        void generateCpp(QTextStream &textStream) const;

        QString targetUserInterfaceClassName() const;

        QString BusinessLogiClassName;
        QList<SlotData> Slots;
    };

    explicit UserInterfaceSkeletonGenerator(QObject *parent = 0);
private:
    void displayFrontendBackendConnectStatements(const UserInterfaceSkeletonGeneratorData &data);
signals:
    void e(QString msg);
    void o(QString msg);
    void finishedGeneratingUserInterfaceSkeleton(bool success);
public slots:
    void generateUserInterfaceSkeletonFromClassDeclarationString(/*TODOreq: QString &classDeclarationCpp_ForParsing*/);
    void generateUserInterfaceSkeletonFromData(const UserInterfaceSkeletonGeneratorData &data);
private slots:
    void handleDbg(QString msg);
};

#endif // USERINTERFACESKELETONGENERATOR_H
