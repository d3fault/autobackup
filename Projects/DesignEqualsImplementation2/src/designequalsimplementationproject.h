#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECT_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECT_H

#include <QObject>
#include <QList>

#include "designequalsimplementationclass.h" //TODOoptional: forward declare in this + tons of similar classes
#include "designequalsimplementationusecase.h"

class DesignEqualsImplementationProject : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProject(QObject *parent = 0);
    explicit DesignEqualsImplementationProject(const QString &existingProjectFilePath, QObject *parent = 0);
    ~DesignEqualsImplementationProject();

    //TODOoptional: private + getter/setter blah
    QString Name;
    QList<DesignEqualsImplementationClass*> Classes;
    QList<DesignEqualsImplementationUseCase*> UseCases;
private:
    bool savePrivate(const QString &projectFilePath);
    bool generateSourceCodePrivate(const QString &destinationDirectoryPath);
    bool tempGenerateHardcodedUiFiles(const QString &destinationDirectoryPath);
    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }
signals:
    void saved(bool);
    void sourceCodeGenerated(bool);
    void e(const QString &);
public slots:
    void save(const QString &projectFilePath);
    void generateSourceCode(const QString &destinationDirectoryPath);
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project);

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECT_H
