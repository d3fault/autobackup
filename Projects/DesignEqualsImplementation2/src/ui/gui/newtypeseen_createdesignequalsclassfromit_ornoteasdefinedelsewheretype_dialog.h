#ifndef NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H
#define NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H

#include <QDialog>

class QGridLayout;

class NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(const QList<QString> listOfTypesToDecideOn, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QList<QString> typesDecidedToBeDesignEqualsImplementationClass() const;
    QList<QString> typesDecidedToBeDefinedElsewhere() const;
private:
    QGridLayout *m_NewTypesGridLayout;

    QList<QString> buildTypesListDependingOnWhetherCreateClassIsChecked(bool checked) const;
};

#endif // NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H
