#ifndef CLASSEDITORDIALOG_H
#define CLASSEDITORDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;
class QCheckBox;
class QVBoxLayout;

class DesignEqualsImplementationProject;
class MethodSingleArgumentWidget;
class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassProperty;
class IDesignEqualsImplementationMethod;
class DesignEqualsImplementationClassSlot;
class DesignEqualsImplementationClassSignal;

class ClassEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassEditorDialog(DesignEqualsImplementationClass *classToEdit, DesignEqualsImplementationProject *currentProject, QWidget *parent = 0, Qt::WindowFlags f = 0);
private:
    DesignEqualsImplementationClass *m_ClassBeingEditted;
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
    QWidget *createPropertiesClassEditorTab();
    QWidget *createSignalsClassEditorTab();
    QWidget *createSlotsClassEditorTab();

    QString classDetailsAsHtmlString();
signals:
    void e(const QString &);
private slots:
    void handleQuickAddNewPropertyButtonClicked();
    void handleQuickAddNewSignalButtonClicked();
    void handleQuickAddNewSlotButtonClicked();

    void handleAddPropertyButtonClicked();

    void handleAddSlotAddArgButtonClicked();
    void handleTabPressedInArgNameLineEdit_SoMakeNewArgIfThatWasTheLastOne_OtherwiseManuallyChangeFocusSinceTheTabWasEaten();
    void handleMoveArgumentUpRequested(MethodSingleArgumentWidget *argumentRequestingToBeMovedUp);
    void handleMoveArgumentDownRequested(MethodSingleArgumentWidget *argumentRequestingToBeMovedDown);
    void handleDeleteArgumentRequested(MethodSingleArgumentWidget *argumentRequestingToBeDeleted);
    void handleAddSlotButtonClicked();

    //reactor pattern slots
    void updateClassOverviewLabel();
#if 0
    void handlePropertyAdded(DesignEqualsImplementationClassProperty *propertyAdded);
    void handleSignalAdded(DesignEqualsImplementationClassSignal *signalAdded);
    void handleSlotAdded(DesignEqualsImplementationClassSlot *slotAdded);
#endif
};

#endif // CLASSEDITORDIALOG_H
