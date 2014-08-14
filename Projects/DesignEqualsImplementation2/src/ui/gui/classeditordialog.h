#ifndef CLASSEDITORDIALOG_H
#define CLASSEDITORDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassProperty;

class ClassEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassEditorDialog(DesignEqualsImplementationClass *classToEdit, QWidget *parent = 0, Qt::WindowFlags f = 0);
private:
    DesignEqualsImplementationClass *m_ClassBeingEditted;

    //Add Property
    QLineEdit *m_AddPropertyTypeLineEdit;
    QLineEdit *m_AddPropertyNameLineEdit;
    QCheckBox *m_AddPropertyReadOnlyCheckbox;
    QCheckBox *m_AddPropertyNotifiesOnChangeCheckbox;

    bool addPropertyFieldsAreSane();
signals:
    void e(const QString &);
private slots:
    void handleQuickAddNewPropertyButtonClicked();
    void handleQuickAddNewSignalButtonClicked();
    void handleQuickAddNewSlotButtonClicked();

    void handleAddPropertyButtonClicked();

    //reactor pattern slots
    void handlePropertyAdded(DesignEqualsImplementationClassProperty *propertyAdded);
};

#endif // CLASSEDITORDIALOG_H
