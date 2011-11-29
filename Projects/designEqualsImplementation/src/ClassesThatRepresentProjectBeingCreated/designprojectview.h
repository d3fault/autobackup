#ifndef DESIGNPROJECTVIEW_H
#define DESIGNPROJECTVIEW_H

#include <QObject>

#include "designprojecttemplates.h"

class DesignProjectView : public QObject
{
    Q_OBJECT
public:
    explicit DesignProjectView(QString useCaseName);
    explicit DesignProjectView();
    QString getProjectViewName();
private:
    void privConstructor(QString projectViewName, DesignProjectTemplates::DesignProjectViewType projectViewType);
    QString m_ProjectViewName;
    DesignProjectTemplates::DesignProjectViewType m_ProjectViewType;
signals:

public slots:

};

#endif // DESIGNPROJECTVIEW_H
