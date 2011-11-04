#ifndef OPENPROJECTSMANAGER_H
#define OPENPROJECTSMANAGER_H

#include <QObject>
#include <QList>

#include <designproject.h>

class OpenProjectsManager : public QObject
{
    Q_OBJECT
public:
    explicit OpenProjectsManager(QObject *parent = 0);
    void add(DesignProject *designProject);
    DesignProject *getByTabIndex(int tabIndex);
private:
    QList<DesignProject*> *m_ListOfOpenProjects;
signals:
    void projectOpened(DesignProject*);

public slots:

};

#endif // OPENPROJECTSMANAGER_H
