#ifndef DESIGNEQUALSIMPLEMENTATION_H
#define DESIGNEQUALSIMPLEMENTATION_H

#include <QObject>
#include <QList>
#include <QMutex>

//#define DesignEqualsImplementation_TEST_MODE 1
#define DesignEqualsImplementation_TEST_GUI_MODE 1

class DesignEqualsImplementationProject;

class DesignEqualsImplementation : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementation(QObject *parent = 0);
    ~DesignEqualsImplementation();
    static QMutex BackendMutex;
private:
    QList<DesignEqualsImplementationProject*> m_CurrentlyOpenedDesignEqualsImplementationProjects;
signals:
    void projectOpened(DesignEqualsImplementationProject *project);
    void e(const QString &);
public slots:
    void initializeDesignEqualsImplementationInGuiMode();
    void newProject();
    void openExistingProject(const QString &existingProjectFilePath);
#ifdef DesignEqualsImplementation_TEST_MODE
private slots:
    void handlesourceCodeGenerated(bool success);
    void handleE(const QString &msg);
#endif
};

#endif // DESIGNEQUALSIMPLEMENTATION_H
