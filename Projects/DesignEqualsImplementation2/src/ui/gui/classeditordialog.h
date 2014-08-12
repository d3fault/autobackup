#ifndef CLASSEDITORDIALOG_H
#define CLASSEDITORDIALOG_H

#include <QDialog>

class DesignEqualsImplementationClass;

class ClassEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassEditorDialog(DesignEqualsImplementationClass *classToEdit, QWidget *parent = 0, Qt::WindowFlags f = 0);
private slots:
    void handleQuickAddNewPropertyButtonClicked();
    void handleQuickAddNewSignalButtonClicked();
    void handleQuickAddNewSlotButtonClicked();
};

#endif // CLASSEDITORDIALOG_H
