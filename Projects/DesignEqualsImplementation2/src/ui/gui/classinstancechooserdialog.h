#ifndef CLASSINSTANCECHOOSERDIALOG_H
#define CLASSINSTANCECHOOSERDIALOG_H

#include <QDialog>
#include <QList>
#include <QStack>

class QRadioButton;
class QComboBox;
class QLineEdit;
class QListWidget;

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassLifeLine;
class DesignEqualsImplementationUseCase;
class HasA_Private_Classes_Member;

class ClassInstanceChooserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassInstanceChooserDialog(DesignEqualsImplementationClassLifeLine *classLifelineForWhichAnInstanceIsBeingChosen, QWidget *parent = 0, Qt::WindowFlags f = 0);
    bool newInstanceChosen();
    DesignEqualsImplementationClass *parentClassChosenToGetNewHasAprivateMember() const;
    QString nameOfNewPrivateHasAMember() const;

    HasA_Private_Classes_Member *chosenExistingHasA_Private_Classes_Member() const;

#if 0 //TODOinstancing
    bool newTopLevelInstanceChosen();
    DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe() const;
#endif
private:
    QRadioButton *m_NewInstanceRadioButton;
    bool m_NewInstanceChosen;
    QComboBox *m_ClassesInUseCaseAvailableForUseAsParentOfNewInstance;
    QLineEdit *m_NewInstanceNameLineEdit;
    QListWidget *m_ExistingInstancesListWidget;
    DesignEqualsImplementationClassLifeLine *m_ClassLifelineForWhichAnInstanceIsBeingChosen;
    DesignEqualsImplementationClass *m_ExistingClassToUseAsParentForNewInstance;
    QString m_NameOfNewPrivateHasAMember;

    HasA_Private_Classes_Member *m_ExistingInstanceHasAPrivateClassesMember;

    void addAllPrivateHasAMembersThatAreOfAcertainTypeToExistingInstancesListWidget(DesignEqualsImplementationClass *classToIterate, DesignEqualsImplementationClass *typeOfClassWeAreInterestedInInstancesOf);

    bool m_UserIsTypingInCustomVariableNameSoDontSuggestAutoName;

#if 0 //TODOinstancing
    bool m_NewTopLevelInstanceChosen;
    DesignEqualsImplementationClassInstance *m_myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline;
    //QList<QPair<QString /*absolute variable specifier */, DesignEqualsImplementationClassInstance* /*classInstance*/> > m_
    QStack<QString> m_AbsoluteVariableNameSpecifierCurrentDepthStack;

    void addInstanceToListWidget(const QString &absoluteVariableName, DesignEqualsImplementationClassInstance *currentInstance);
    void addAllChildrensChildrenClassInstancesToPassedInList(DesignEqualsImplementationClassInstance *currentChildInstance/*, QList<DesignEqualsImplementationClassInstance*> *runningListOfExistingInstances*/);
    QString makeAbsoluteVariableNameSpecifierForInstance(DesignEqualsImplementationClassInstance *instanceToFinalizeTheVariableName);
#endif
signals:
    void createNewHasAPrivateMemberAndAssignItAsClassLifelineInstanceRequested(DesignEqualsImplementationClass *parentClassChosenToGetNewHasAprivateMember, DesignEqualsImplementationClass *typeOfNewPrivateHasAMember, const QString &nameOfNewPrivateHasAMember);
    void assignPrivateMemberAsClassLifelineInstanceRequested(HasA_Private_Classes_Member *chosenExistingHasA_Private_Classes_Member);
private slots:
    void handleNewInstancesRadioButtonToggled(bool checked);
    void handleClassesInUseCaseAvailableForUseAsParentOfNewInstanceCurrentIndexChanged(int newIndex);
    void handleNewInstanceNameLineEditTextChanged(const QString &newText);
    void handleExistingInstancesCurrentRowChanged(int newRow);
    void handleDialogAccepted();
};

#endif // CLASSINSTANCECHOOSERDIALOG_H
