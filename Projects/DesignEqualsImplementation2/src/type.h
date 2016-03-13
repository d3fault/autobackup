#ifndef TYPE_H
#define TYPE_H

#include <QObject>
#include <QScopedPointer>
#include <QList>

struct Visibility
{
    enum VisibilityEnum { Public = 0, Protected = 1, Private = 2 };
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
    explicit TypeInstance(Type *type, const QString &variableName, QObject *parent = 0)
        : QObject(parent)
        , type(type)
        , VariableName(variableName)
    { }
    QString preferredTextualRepresentationOfTypeAndVariableTogether() const;

    Type *type;
    QString Qualifiers_LHS;
    QString Qualifiers_RHS;

    QString VariableName;
};
struct NonFunctionMemberOwnershipOfPointedToDataIfPointer
{
    enum NonFunctionMemberOwnershipOfPointedToDataIfPointerEnum
    {
          NotPointer = 0
        , OwnsPointedToData = 1
        , DoesNotOwnPointedToData = 2
    };
};
class NonFunctionMember : public QObject
{
    Q_OBJECT
public:
    explicit NonFunctionMember(Type *type, const QString &variableName, Type *parentClassThatIamMemberOf, QObject *parent, bool hasInit = false, const QString &optionalInit = QString())
        : QObject(parent)
        , typeInstance(new TypeInstance(type, variableName, this))
        , visibility(Visibility::Private)
        , HasInit(hasInit)
        , OptionalInit(optionalInit)
        , OwnershipOfPointedTodataIfPointer(NonFunctionMemberOwnershipOfPointedToDataIfPointer::NotPointer)
        , m_ParentTypeThatIamMemberOf(parentClassThatIamMemberOf)
    { }
#if 0
    NonFunctionMember(const NonFunctionMember &other)
        : typeInstance(new TypeInstance(other.typeInstance))
        , ParentTypeThatIamMemberOf(other.ParentTypeThatIamMemberOf)
        , visibility(other.visibility)
        , OwnershipOfPointedTodataIfPointer(other.OwnershipOfPointedTodataIfPointer)
    { }
#endif

    TypeInstance *typeInstance;
    Type *parentClass() const { return m_ParentTypeThatIamMemberOf; }
    Visibility::VisibilityEnum visibility;
    bool HasInit;
    QString OptionalInit;
    NonFunctionMemberOwnershipOfPointedToDataIfPointer::NonFunctionMemberOwnershipOfPointedToDataIfPointerEnum OwnershipOfPointedTodataIfPointer; //tells us whether or not to do a '[typeInstance.variableName] = new [typeInstance.type.Name](this)' in constructor initialization. OT'ish: if it's not a QObject derived object, we should throw that bitch in a scoped pointer
    //NOTE: for now, owning the pointer and having an init are mutually exclusive, since their functionality overlaps
private:
    Type *m_ParentTypeThatIamMemberOf;
};
Q_DECLARE_METATYPE(NonFunctionMember*)
class Type : public QObject
{
    Q_OBJECT
public:
    explicit Type(QObject *parent = 0) : QObject(parent) { }
    QList<TypeAncestor> DirectAncestors; //those ancestors can have ancestors too, just like good ole inheritence

    QString Name;

    QList<NonFunctionMember*> nonFunctionMembers() const { return m_NonFunctionMembers; }
    virtual bool addNonFunctionMember(NonFunctionMember* nonFunctionMember)=0;
    NonFunctionMember *createNewNonFunctionMember(Type *typeOfNewNonFunctionMember, const QString &nameOfNewNonFunctionMember, Visibility::VisibilityEnum visibility = Visibility::Private, NonFunctionMemberOwnershipOfPointedToDataIfPointer::NonFunctionMemberOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer = NonFunctionMemberOwnershipOfPointedToDataIfPointer::NotPointer, bool hasInit = false, const QString &optionalInit = QString());
    int serializationNonFunctionMemberIdForNonFunctionMember(NonFunctionMember *nonFunctionMember) const;
    NonFunctionMember *nonFunctionMemberFromNonFunctionMemberId(int nonFunctionMemberId) const;
protected:
    QList<NonFunctionMember*> m_NonFunctionMembers; //they ARE non-function members, but the resulting code might still yield functions (getters & setters (d->pimpl for shared data and change checking+notification for Q_PROPERTY), change notifier signals in the case of Q_PROPERTIES)
};
class DefinedElsewhereType : public Type
{
    Q_OBJECT
public:
    explicit DefinedElsewhereType(QObject *parent = 0) : Type(parent) { }
    bool addNonFunctionMember(NonFunctionMember* nonFunctionMember)
    {
        Q_UNUSED(nonFunctionMember)
        qFatal("ERROR: tried to addNonFunctionMember() to a DefinedElsewhereType. The caller should have checked this");
        return false;
    }
};

#endif // TYPE_H
