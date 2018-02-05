#include "iuigenerator.h"

#include <QTemporaryDir>
#include <QDebug>

bool IUIGenerator::generateUi(const UIGeneratorFormat &format)
{
    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QString outputPathWithSlashAppended = appendSlashIfNeeded(outputDir.path());
    QStringList filesToGenerate2 = filesToGenerate();
    for(QStringList::const_iterator it = filesToGenerate2.constBegin(); it != filesToGenerate2.constEnd(); ++it)
    {
        const QString &currentFilePathToGenerate = *it;
        QFile currentFileToGenerate(outputPathWithSlashAppended + currentFilePathToGenerate); //TODOreq: parse the "ui name" from the inputFormat json shiz
        if(!currentFileToGenerate.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "failed to open file:" << currentFileToGenerate.fileName();
            return false;
        }
        QTextStream currentFileTextStream(&currentFileToGenerate);
        if(generateUiForFile(currentFilePathToGenerate, currentFileTextStream, format)) //kek
        {
            qDebug() << "Generated:" << currentFileToGenerate.fileName();
        }
        else
        {
            qCritical() << "failed to generate ui: " << currentFileToGenerate.fileName();
            return false;
        }
    }
    return true;
}
