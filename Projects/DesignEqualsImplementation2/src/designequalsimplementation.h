#ifndef DESIGNEQUALSIMPLEMENTATION_H
#define DESIGNEQUALSIMPLEMENTATION_H

#include <QObject>
#include <QList>

#include "designequalsimplementationproject.h"

class DesignEqualsImplementation : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementation(QObject *parent = 0);
    ~DesignEqualsImplementation();
private:
    QList<DesignEqualsImplementationProject*> m_CurrentlyOpenedDesignEqualsImplementationProjects;
public slots:
    void newEmptyProject();
    void loadProjectFromFilePath(const QString &existingProjectFilePath);
};

#endif // DESIGNEQUALSIMPLEMENTATION_H
