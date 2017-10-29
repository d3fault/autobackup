#ifndef CODEGENERATORGENERATOR_H
#define CODEGENERATORGENERATOR_H

#include <QObject>

class QFileInfo;

class CodeGeneratorGenerator : public QObject
{
    Q_OBJECT
public:
    explicit CodeGeneratorGenerator(QObject *parent = 0);
private:
    static QString relativePathFromInputDir(const QString &inputDir, QString absoluteFilePath);
    static QString targetPath(QString outputDir, QString directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor, const QFileInfo *fileInfo);
    static QString appendSlashIfNeeded(QString x);
signals:
    void e(QString msg);
    void o(QString msg);
    void v(QString msg);
public slots:
    void generateCodeGenerator(const QString &directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor);
};

#endif // CODEGENERATORGENERATOR_H
