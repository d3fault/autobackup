#ifndef DESIGNPROJECTVIEW_H
#define DESIGNPROJECTVIEW_H

#include <QObject>

class DesignProjectView : public QObject
{
    Q_OBJECT
public:
    enum ProjectViewType { UseCaseType, ClassDiagramType };
    explicit DesignProjectView(QString useCaseName);
    explicit DesignProjectView();
    QString getProjectViewName();
private:
    void privConstructor(QString projectViewName, ProjectViewType projectViewType);
    QString m_ProjectViewName;
    ProjectViewType m_ProjectViewType;
signals:

public slots:

};

#endif // DESIGNPROJECTVIEW_H
