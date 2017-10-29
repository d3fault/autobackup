#include "codegeneratorgenerator.h"

#include <QDirIterator>
#include <QTemporaryDir>
#include <QDir>

CodeGeneratorGenerator::CodeGeneratorGenerator(QObject *parent)
    : QObject(parent)
{ }
QString CodeGeneratorGenerator::relativePathFromInputDir(const QString &inputDir, QString /*life is to short for const correctness, unless benchmarks show it makes a significant difference in a given use case*/ absoluteFilePath)
{
    QString inputDirWithTrailingSlash = appendSlashIfNeeded(inputDir);
    return absoluteFilePath.mid(inputDirWithTrailingSlash.size());
}
QString CodeGeneratorGenerator::targetPath(QString outputDir, QString directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor, const QFileInfo *fileInfo)
{
    return outputDir + relativePathFromInputDir(directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor, fileInfo->canonicalFilePath());
}
QString CodeGeneratorGenerator::appendSlashIfNeeded(QString x)
{
    if(!x.endsWith("/"))
    {
        x.append("/");
    }
    return x;
}
void CodeGeneratorGenerator::generateCodeGenerator(const QString &directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor)
{
    GenerateCodeGeneratorScopedResponder scopedResponder(m_Contracts.generateCodeGenerator());

    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QDirIterator dirIterator(directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor, QDirIterator::Subdirectories);
    while(dirIterator.hasNext())
    {
        dirIterator.next();
        const QFileInfo &fileInfo = dirIterator.fileInfo();
        QString myTargetPath = targetPath(outputDir.path(), directoryToCompilingExampleTemplateToGenerateCodeGeneratorFor, &fileInfo);
        if(fileInfo.isFile())
        {
            //copy file to outputDir
            QFile inputFile(fileInfo.canonicalFilePath());
            if(!inputFile.copy(myTargetPath))
            {
                emit e("couldn't copy file: " + myTargetPath);
                return/* emit*/; //let scopedResponder go out of scope, emitting generateCodeGeneratorFinished(falseWhichIsTheDefault)
            }
            emit v("copied file: " + myTargetPath);
        }
        else if(fileInfo.isDir())
        {
            //mkpath on outputDir
            QDir dir(myTargetPath);
            if(!dir.mkpath(myTargetPath))
            {
                emit e("couldn't make path: " + dir.path());
                return/* emit*/;
            }
            emit v("copied dir: " + dir.path());

#if 0 //just make it, but handle errors ofc!
            //ensure dir is made in outputDir
            if(!dir.exists())
            {
                dir.mkpath(dir.path()); //xD
            }
#endif
        }
        else
        {
            emit e("warning: unprocessed dir iterator entry: \"" + fileInfo.canonicalFilePath() + "\". would have copied to: \"" + myTargetPath + "\"");
        }
    }

    emit o("your output dir is here: " + outputDir.path());
    scopedResponder.setSuccess(true);
    return /*emit*/;
}
