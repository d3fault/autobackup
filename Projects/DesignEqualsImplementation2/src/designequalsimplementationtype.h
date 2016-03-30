#ifndef DESIGNEQUALSIMPLEMENTATIONTYPE_H
#define DESIGNEQUALSIMPLEMENTATIONTYPE_H

#include <QObject>
#include <QScopedPointer>
#include <QList>
#include <QPointF>
#include <QStringList>

class DesignEqualsImplementationProject;

struct Visibility
{
    enum VisibilityEnum { Public = 0, Protected = 1, Private = 2 };
};
class DesignEqualsImplementationType;
struct TypeAncestor
{
    DesignEqualsImplementationType *type;
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
    explicit TypeInstance(DesignEqualsImplementationType *type, const QString &qualifiedTypeString, const QString &variableName, QObject *parent = 0)
        : QObject(parent)
        , type(type)
        , VariableName(variableName)
        , OwnershipOfPointedTodataIfPointer(TypeInstanceOwnershipOfPointedToDataIfPointer::NotPointer)
    {
        parseQualifiedTypeString(qualifiedTypeString);
    }
    static QString preferredTextualRepresentationOfTypeAndVariableTogether(const QString &qualifiedType, const QString &variableName);
    static bool isPointer(const QString &qualifiers_RHS_or_BothSidesIsFineToo);
    void parseQualifiedTypeString(const QString &qualifiedTypeString);
    QString nonQualifiedType() const;
    QString qualifiedType() const;
    QString preferredTextualRepresentationOfTypeAndVariableTogether() const;
    bool isPointer() const;

    DesignEqualsImplementationType *type;
    QString Qualifiers_LHS;
    QString Qualifiers_RHS;

    QString VariableName;

    TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum OwnershipOfPointedTodataIfPointer; //tells us whether or not to do a '[typeInstance.variableName] = new [typeInstance.type.Name](this)' in constructor initialization. OT'ish: if it's not a QObject derived object, we should throw that bitch in a scoped pointer
};
class NonFunctionMember : public TypeInstance
{
    Q_OBJECT
public:
    explicit NonFunctionMember(DesignEqualsImplementationType *type, const QString &qualifiedTypeString, const QString &variableName, DesignEqualsImplementationType *parentClassThatIamMemberOf, QObject *parent, bool hasInit = false, const QString &optionalInit = QString())
        : TypeInstance(type, qualifiedTypeString, variableName, parent)
        , visibility(Visibility::Private)
        , HasInit(hasInit)
        , OptionalInit(optionalInit)
        , m_ParentTypeThatIamMemberOf(parentClassThatIamMemberOf)
    { }

    DesignEqualsImplementationType *parentClass() const { return m_ParentTypeThatIamMemberOf; }
    Visibility::VisibilityEnum visibility;
    bool HasInit;
    QString OptionalInit;
    //NOTE: for now, owning the pointer and having an init are mutually exclusive, since their functionality overlaps
private:
    DesignEqualsImplementationType *m_ParentTypeThatIamMemberOf;
};
Q_DECLARE_METATYPE(NonFunctionMember*)
class DesignEqualsImplementationType : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationType(QObject *parent, DesignEqualsImplementationProject *parentProject)
        : QObject(parent)
        , m_ParentProject(parentProject)
    { }
    QList<TypeAncestor> DirectAncestors; //those ancestors can have ancestors too, just like good ole inheritence

    QString Name;
    QPointF Position; //DefinedElsewhereType does not use this, since it is not in/on the class diagram scene (but I might change my mind about that later)

    DesignEqualsImplementationProject *m_ParentProject;

    QList<NonFunctionMember*> nonFunctionMembers() const { return m_NonFunctionMembers; }
    QList<NonFunctionMember*> nonFunctionMembers_OrderedCorrectlyAsMuchAsPossibleButWithMembersThatHaveOptionalInitAtTheEnd() const;
    virtual void addNonFunctionMember(NonFunctionMember* nonFunctionMember)=0; //TODOmb: protected?
    NonFunctionMember *createNewNonFunctionMember(DesignEqualsImplementationType *typeOfNewNonFunctionMember, const QString &qualifiedTypeString, const QString &nameOfNewNonFunctionMember = QString(), Visibility::VisibilityEnum visibility = Visibility::Private, TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum ownershipOfPointedToDataIfPointer = TypeInstanceOwnershipOfPointedToDataIfPointer::NotPointer, bool hasInit = false, const QString &optionalInit = QString());
    bool memberWithNameExists(const QString &memberNameToCheckForCollisions) const;
    QString autoNameForNewChildMemberOfType(DesignEqualsImplementationType *childMemberClassType) const;
    int serializationNonFunctionMemberIdForNonFunctionMember(NonFunctionMember *nonFunctionMember) const;
    NonFunctionMember *nonFunctionMemberFromNonFunctionMemberId(int nonFunctionMemberId) const;

    QString headerFilenameOnly() const;
    QString sourceFilenameOnly() const;
    QStringList includes() const;

    virtual int typeCategory() const=0;
protected:
    void addNonFunctionMemberPrivate(NonFunctionMember *nonFunctionMember);
private:
    QList<NonFunctionMember*> m_NonFunctionMembers; //they ARE non-function members, but the resulting code might still yield functions (getters & setters (d->pimpl for shared data and change checking+notification for Q_PROPERTY), change notifier signals in the case of Q_PROPERTIES)
protected: //signals: //TODOreq: i AM a qobject, so this isn't necessary
    virtual void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember)=0;
signals:
    void e(const QString &msg);
public slots:
    void setClassName(const QString &newClassName);
};
class DefinedElsewhereType : public DesignEqualsImplementationType
{
    Q_OBJECT
public:
    explicit DefinedElsewhereType(QObject *parent, DesignEqualsImplementationProject *parentProject) : DesignEqualsImplementationType(parent, parentProject) { }
    QStringList definedElsewhereIncludes() const { return QStringList(); } //note: does not contain (had:include) the "#include " part or even the double quotes, just the file path of the include
    void addNonFunctionMember(NonFunctionMember* nonFunctionMember)
    {
        Q_UNUSED(nonFunctionMember)
        qFatal("ERROR: tried to addNonFunctionMember() to a DefinedElsewhereType. The caller should have checked this");
    }
    virtual int typeCategory() const { return 2; }
signals:
    void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember);
};

#endif // DESIGNEQUALSIMPLEMENTATIONTYPE_H
