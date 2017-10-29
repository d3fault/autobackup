#ifndef CODEGENERATORGENERATOR_H
#define CODEGENERATORGENERATOR_H

#include <QObject>

#include "codegeneratorgeneratorrequestresponsecontracts.h"

class QFileInfo;

class CodeGeneratorGenerator : public QObject
{
    Q_OBJECT
public:
    template<class T>
    static void establishConnectionsToAndFromBackendAndUi(CodeGeneratorGenerator *backend, T *ui)
    {
        connect(ui, &T::generateCodeGeneratorRequested, backend, &CodeGeneratorGenerator::generateCodeGenerator);
        connect(backend, &CodeGeneratorGenerator::e, ui, &T::handleE);
        connect(backend, &CodeGeneratorGenerator::o, ui, &T::handleO);
        connect(backend, &CodeGeneratorGenerator::v, ui, &T::handleV);
        connect(backend, &CodeGeneratorGenerator::generateCodeGeneratorFinished, ui, &T::handleGenerateCodeGeneratorFinished);
    }

    explicit CodeGeneratorGenerator(QObject *parent = 0);
private:
    static QString relativePathFromInputDir(const QString &inputDir, QString absoluteFilePath);
    static QString targetPath(QString outputDir, QString directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor, const QFileInfo *fileInfo);
    static QString appendSlashIfNeeded(QString x);

    CodeGeneratorGeneratorRequestResponseContracts::Contracts m_Contracts;
signals:
    void e(QString msg);
    void o(QString msg);
    void v(QString msg);
    void generateCodeGeneratorFinished(bool success, QString outputDir);
public slots:
    void generateCodeGenerator(const QString &directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor);
};

#endif // CODEGENERATORGENERATOR_H
