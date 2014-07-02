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
    QList<DesignEqualsImplementationUseCase*> UseCases;

    void addClass(DesignEqualsImplementationClass* newClass);
    QList<DesignEqualsImplementationClass*> classes();

    //TODOreq: m_Classes should be private, but I had issues getting the getters/setters to play nicely with QDataStream. Maybe a simple "friend QDataStream;" would fix it (or similar), but I can't be fucked to even play around with it right now. STILL, after coding for a while you should check that all usages of m_Classes are only from within the getter/setters (more important is the setter, but still in principle the getter too)
    QList<DesignEqualsImplementationClass*> m_Classes;
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
