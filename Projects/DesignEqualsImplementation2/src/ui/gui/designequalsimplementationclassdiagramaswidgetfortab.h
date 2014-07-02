#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H

#include <QWidget>

class QGraphicsScene;

class DesignEqualsImplementationProject;
class DesignEqualsImplementationClass;

class DesignEqualsImplementationClassDiagramAsWidgetForTab : public QWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
private:
    QGraphicsScene *m_ClassDiagramScene;
public slots:
    void handleClassAdded(DesignEqualsImplementationClass *classAdded);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
