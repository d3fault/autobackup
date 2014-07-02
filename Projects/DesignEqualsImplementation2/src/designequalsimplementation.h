#ifndef DESIGNEQUALSIMPLEMENTATION_H
#define DESIGNEQUALSIMPLEMENTATION_H

#include <QObject>
#include <QList>

#include "designequalsimplementationproject.h"

#define DesignEqualsImplementation_TEST_MODE 1

class DesignEqualsImplementation : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementation(QObject *parent = 0);
    ~DesignEqualsImplementation();
private:
    QList<DesignEqualsImplementationProject*> m_CurrentlyOpenedDesignEqualsImplementationProjects;
signals:
    void e(const QString &);
public slots:
    void newEmptyProject();
    void loadProjectFromFilePath(const QString &existingProjectFilePath);
#ifdef DesignEqualsImplementation_TEST_MODE
private slots:
    void handlesourceCodeGenerated(bool success);
    void handleE(const QString &msg);
#endif
};

#endif // DESIGNEQUALSIMPLEMENTATION_H
