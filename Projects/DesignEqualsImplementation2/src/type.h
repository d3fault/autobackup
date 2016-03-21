#ifndef TYPE_H
#define TYPE_H

#include <QObject>
#include <QScopedPointer>
#include <QList>

//TODOreq: namespace this shiz
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
struct TypeInstanceOwnershipOfPointedToDataIfPointer
{
    enum TypeInstanceOwnershipOfPointedToDataIfPointerEnum
    {
          NotPointer = 0
        , OwnsPointedToData = 1
        , DoesNotOwnPointedToData = 2
    };
};
class TypeInstance : public QObject
{
    Q_OBJECT
public:
    explicit TypeInstance(Type *type, const QString &qualifiedTypeString, const QString &variableName, QObject *parent = 0)
        : QObject(parent)
        , type(type)
        , VariableName(variableName)
        , OwnershipOfPointedTodataIfPointer(TypeInstanceOwnershipOfPointedToDataIfPointer::NotPointer)
    {
        parseQualifiedTypeString(qualifiedTypeString);
    }
    static QString preferredTextualRepresentationOfTypeAndVariableTogether(const QString &qualifiedType, const QString &variableName);
    void parseQualifiedTypeString(const QString &qualifiedTypeString);
    QString nonQualifiedType() const;
    QString qualifiedType() const;
    QString preferredTextualRepresentationOfTypeAndVariableTogether() const;
    bool isPointer() const;

    Type *type;
    QString Qualifiers_LHS;
    QString Qualifiers_RHS;

    QString VariableName;

    TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum OwnershipOfPointedTodataIfPointer; //tells us whether or not to do a '[typeInstance.variableName] = new [typeInstance.type.Name](this)' in constructor initialization. OT'ish: if it's not a QObject derived object, we should throw that bitch in a scoped pointer
};
class NonFunctionMember : public TypeInstance
{
    Q_OBJECT
public:
    explicit NonFunctionMember(Type *type, const QString &qualifiedTypeString, const QString &variableName, Type *parentClassThatIamMemberOf, QObject *parent, bool hasInit = false, const QString &optionalInit = QString())
        : TypeInstance(type, qualifiedTypeString, variableName, parent)
        , visibility(Visibility::Private)
        , HasInit(hasInit)
        , OptionalInit(optionalInit)
        , m_ParentTypeThatIamMemberOf(parentClassThatIamMemberOf)
    { }

    Type *parentClass() const { return m_ParentTypeThatIamMemberOf; }
    Visibility::VisibilityEnum visibility;
    bool HasInit;
    QString OptionalInit;
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
    virtual void addNonFunctionMember(NonFunctionMember* nonFunctionMember)=0;
    NonFunctionMember *createNewNonFunctionMember(Type *typeOfNewNonFunctionMember, const QString &qualifiedTypeString, const QString &nameOfNewNonFunctionMember = QString(), Visibility::VisibilityEnum visibility = Visibility::Private, TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer = TypeInstanceOwnershipOfPointedToDataIfPointer::NotPointer, bool hasInit = false, const QString &optionalInit = QString());
    bool memberWithNameExists(const QString &memberNameToCheckForCollisions) const;
    QString autoNameForNewChildMemberOfType(Type *childMemberClassType) const;
    int serializationNonFunctionMemberIdForNonFunctionMember(NonFunctionMember *nonFunctionMember) const;
    NonFunctionMember *nonFunctionMemberFromNonFunctionMemberId(int nonFunctionMemberId) const;

    QString headerFilenameOnly() const;
    QString sourceFilenameOnly() const;

    virtual int typeCategory() const=0;
protected:
    void addNonFunctionMemberPrivate(NonFunctionMember *nonFunctionMember);
private:
    QList<NonFunctionMember*> m_NonFunctionMembers; //they ARE non-function members, but the resulting code might still yield functions (getters & setters (d->pimpl for shared data and change checking+notification for Q_PROPERTY), change notifier signals in the case of Q_PROPERTIES)
protected: //signals:
    virtual void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember)=0;
};
class DefinedElsewhereType : public Type
{
    Q_OBJECT
public:
    explicit DefinedElsewhereType(QObject *parent = 0) : Type(parent) { }
    void addNonFunctionMember(NonFunctionMember* nonFunctionMember)
    {
        Q_UNUSED(nonFunctionMember)
        qFatal("ERROR: tried to addNonFunctionMember() to a DefinedElsewhereType. The caller should have checked this");
    }
    int typeCategory() const { return 2; }
signals:
    void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember);
};

#endif // TYPE_H
