#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H

#include <QWidget>

class QGraphicsScene;
class QDropEvent;

class DesignEqualsImplementationProject;
class DesignEqualsImplementationClass;

class DesignEqualsImplementationClassDiagramAsWidgetForTab : public QWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
protected:
    virtual void dropEvent(QDropEvent *event);
private:
    QGraphicsScene *m_ClassDiagramScene;
public slots:
    void handleClassAdded(DesignEqualsImplementationClass *classAdded);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
