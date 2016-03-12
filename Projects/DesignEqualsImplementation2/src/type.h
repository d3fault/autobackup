#ifndef TYPE_H
#define TYPE_H

#include <QObject>
#include <QScopedPointer>
#include <QList>

struct Visibility
{
    enum VisibilityEnum { Public, Protected, Private };
};
class Type;
struct TypeAncestor
{
    Type *type;
    Visibility::VisibilityEnum visibility;
};
class TypeInstance : public QObject
{
    Q_OBJECT
public:
    explicit TypeInstance(QObject *parent) : QObject(parent) { }

    Type *type;
    QString Qualifiers_LHS;
    QString Qualifiers_RHS;

    QString VariableName;
};
struct NonFunctionMemberOwnershipOfPointedToDataIfPointer
{
    enum NonFunctionMemberOwnershipOfPointedToDataIfPointerEnum
    {
          NotPointer
        , OwnsPointedToData
        , DoesNotOwnPointedToData
    };
};
struct NonFunctionMember
{
    NonFunctionMember(const NonFunctionMember &other)
        : typeInstance(other.typeInstance)
        , ParentTypeThatIamMemberOf(other.ParentTypeThatIamMemberOf)
        , visibility(other.visibility)
        , OwnershipOfPointedTodataIfPointer(other.OwnershipOfPointedTodataIfPointer)
    { }

    TypeInstance *typeInstance;
    Type *ParentTypeThatIamMemberOf;
    Visibility::VisibilityEnum visibility;
    NonFunctionMemberOwnershipOfPointedToDataIfPointer::NonFunctionMemberOwnershipOfPointedToDataIfPointerEnum OwnershipOfPointedTodataIfPointer; //tells us whether or not to do a '[typeInstance.variableName] = new [typeInstance.type.Name](this)' in constructor initialization. OT'ish: if it's not a QObject derived object, we should throw that bitch in a scoped pointer
};
class Type : public QObject
{
    Q_OBJECT
public:
    explicit Type(QObject *parent = 0) : QObject(parent) { }
    QList<TypeAncestor> DirectAncestors; //those ancestors can have ancestors too, just like good ole inheritence

    QString Name;
    QList<NonFunctionMember> NonFunctionMembers; //they ARE non-function members, but the resulting code might still yield functions (getters & setters (d->pimpl for shared data and change checking+notification for Q_PROPERTY), change notifier signals in the case of Q_PROPERTIES)

    virtual bool addNonFunctionMember(NonFunctionMember nonFunctionMember)=0;
};
class DefinedElsewhereType : public Type
{
    Q_OBJECT
public:
    explicit DefinedElsewhereType(QObject *parent = 0) : Type(parent) { }
    bool addNonFunctionMember(NonFunctionMember nonFunctionMember)
    {
        qFatal("ERROR: tried to addNonFunctionMember() to a DefinedElsewhereType. The caller should have checked this");
        return false;
    }
};

#endif // TYPE_H
