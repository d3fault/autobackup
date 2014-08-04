#ifndef CLASSINSTANCECHOOSERDIALOG_H
#define CLASSINSTANCECHOOSERDIALOG_H

#include <QDialog>
#include <QList>
#include <QStack>

class QListWidget;

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassInstance;
class DesignEqualsImplementationUseCase;

class ClassInstanceChooserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassInstanceChooserDialog(DesignEqualsImplementationClass *classBeingAdded, DesignEqualsImplementationUseCase *useCaseClassIsBeingAddedTo, QWidget * parent = 0, Qt::WindowFlags f = 0);
    bool newTopLevelInstanceChosen();
    DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe() const;
private:
    QListWidget *m_PotentialInstancesListWidget;
    DesignEqualsImplementationClass *m_ClassBeingAdded;
    bool m_NewTopLevelInstanceChosen;
    DesignEqualsImplementationClassInstance *m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline;
    //QList<QPair<QString /*absolute variable specifier */, DesignEqualsImplementationClassInstance* /*classInstance*/> > m_
    QStack<QString> m_AbsoluteVariableNameSpecifierCurrentDepthStack;

    void addInstanceToListWidget(const QString &absoluteVariableName, DesignEqualsImplementationClassInstance *currentInstance);
    void addAllChildrensChildrenClassInstancesToPassedInList(DesignEqualsImplementationClassInstance *currentChildInstance/*, QList<DesignEqualsImplementationClassInstance*> *runningListOfExistingInstances*/);
    QString makeAbsoluteVariableNameSpecifierForInstance(DesignEqualsImplementationClassInstance *instanceToFinalizeTheVariableName);
private slots:
    void handleCurrentRowChanged(int newRow);
};

#endif // CLASSINSTANCECHOOSERDIALOG_H
