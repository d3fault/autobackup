#ifndef CLASSEDITORDIALOG_H
#define CLASSEDITORDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;
class QCheckBox;
class QVBoxLayout;

class DesignEqualsImplementationProject;
class MethodSingleArgumentWidget;
class Type;
class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassProperty;
class IDesignEqualsImplementationMethod;
class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassSignal;

class ClassEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassEditorDialog(Type *typeToEdit, DesignEqualsImplementationProject *currentProject, QWidget *parent = 0, Qt::WindowFlags f = 0);
private:
    Type *m_TypeBeingEditted;
    bool m_TypeIsQObjectDerived;
    DesignEqualsImplementationProject *m_CurrentProject;

    QLineEdit *m_QuickMemberAddLineEdit;

    QLabel *m_ClassOverviewLabel;

    //Add Property
    QLineEdit *m_AddPropertyTypeLineEdit;
    QLineEdit *m_AddPropertyNameLineEdit;
    QCheckBox *m_AddPropertyReadOnlyCheckbox;
    QCheckBox *m_AddPropertyNotifiesOnChangeCheckbox;

    //Add Slot
    QLineEdit *m_AddSlotNameLineEdit;
    QVBoxLayout *m_AddSlotArgumentsVLayout;

    void addArgToSlotBeingCreated();
    //static void addAllArgsInLayoutToMethod(IDesignEqualsImplementationMethod* methodToAddArgumentsTo, QLayout *layoutContainingOneArgPerChild);
    bool addPropertyFieldsAreSane();
    bool addSlotFieldsAreSane();

    QWidget *createMainClassEditorTab();
    QWidget *createQObjectDerivedMainClassEditorBodyWidget();
    QWidget *createNonQObjectDerivedMainClassEditorBodyWidget();

    QWidget *createPropertiesClassEditorTab();
    QWidget *createSignalsClassEditorTab();
    QWidget *createSlotsClassEditorTab();

    QString classDetailsAsHtmlString();
signals:
    void e(const QString &);
    void editCppModeRequested(Type *designEqualsImplementationClass);
private slots:
    void handleEditCppButtonClicked();

    void addNewNonFunctionMember();
    void handleQuickAddNewSignalButtonClicked();
    void handleQuickAddNewSlotButtonClicked();

    void handleAddPropertyButtonClicked();

    void handleAddSlotAddArgButtonClicked();
    void handleTabPressedInArgNameLineEdit_SoMakeNewArgIfThatWasTheLastOne_OtherwiseManuallyChangeFocusSinceTheTabWasEaten();
    void handleMoveArgumentUpRequested(MethodSingleArgumentWidget *argumentRequestingToBeMovedUp);
    void handleMoveArgumentDownRequested(MethodSingleArgumentWidget *argumentRequestingToBeMovedDown);
    void handleDeleteArgumentRequested(MethodSingleArgumentWidget *argumentRequestingToBeDeleted);
    void handleAddSlotButtonClicked();

    void handleDoneButtonClicked();

    //reactor pattern slots
    void updateClassOverviewLabel();
#if 0
    void handleNonFunctionMemberAdded(NonFunctionMember *propertyAdded);
    void handleSignalAdded(DesignEqualsImplementationClassSignal *signalAdded);
    void handleSlotAdded(DesignEqualsImplementationClassSlot *slotAdded);
#endif
};

#endif // CLASSEDITORDIALOG_H
