#ifndef NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H
#define NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H

#include <QDialog>

#include <QPair>

#include "libclangfunctiondeclarationparser.h"

class QButtonGroup;

class DesignEqualsImplementationType;
class DesignEqualsImplementationProject;

typedef QPair<QString, QButtonGroup*> TypesAndTheirDecisionRadioGroups;

class NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog : public QDialog
{
    Q_OBJECT
public:
    enum TypesCanBeQObjectDerivedEnum { TypesCanBeQObjectDerived, TypesCannotBeQObjectDerived };
    //static bool atLeastOneParsedTypeNeedsDecidingOn(const QList<ParsedTypeInstance> &parsedTypesPossiblyNeedingDecidingOn);
    explicit NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(const QList<QString> listOfUnknownParsedTypes, DesignEqualsImplementationProject *designEqualsImplementationProject, TypesCanBeQObjectDerivedEnum typesCanBeQObjectDerived, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QList<DesignEqualsImplementationType*> newTypesSeen() const;
private:
    static const int QObjectDerivedRadioButtonId;
    static const int ImplicitlySharedRadioButtonId;
    static const int DefinedElsewhereRadioButtonId;

    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
    TypesCanBeQObjectDerivedEnum m_TypesCanBeQObjectDerived;
    QList<TypesAndTheirDecisionRadioGroups> m_TypesAndTheirDecisionRadioGroups;

    QList<DesignEqualsImplementationType*> m_NewTypesSeen;
private slots:
    void selectAllByButtonId(int buttonId);
    void handleDialogAccepted();
};

#endif // NEWTYPESEEN_CREATEDESIGNEQUALSCLASSFROMIT_ORNOTEASDEFINEDELSEWHERETYPE_DIALOG_H
