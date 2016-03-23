#ifndef USECASEUMLITEMSWIDGET_H
#define USECASEUMLITEMSWIDGET_H

#include <QListWidget>

class DesignEqualsImplementationProject;
class DesignEqualsImplementationClass;

class UseCaseUmlItemsWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit UseCaseUmlItemsWidget(QWidget *parent = 0);
protected:
    virtual void startDrag(Qt::DropActions supportedActions);
private:
    void addActor();
    void addClass(DesignEqualsImplementationClass *designEqualsImplementationClass);
public slots:
    void handleNowShowingProject(DesignEqualsImplementationProject *project);
    void handleTypeAdded(Type *typeAdded);
};

#endif // USECASEUMLITEMSWIDGET_H
