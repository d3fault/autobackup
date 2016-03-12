enum Visibility { Public, Protected, Private };
class TypeAncestor
{
    Type type;
    Visibility visibility;
};
class NonFunctionMember
{
    Type *ParentTypeThatIamMemberOf;
    TypeInstance *typeInstance;
    Visibility visibility;
    QScopedPointer<bool> IsOwnerOfPointer_orNullIfNotPointer; //hacky tri-bool :). tells us whether or not to do a '[typeInstance.variableName] = new [typeInstance.type.Name](this)' in constructor initialization. OT'ish: if it's not a QObject derived object, we should throw that bitch in a scoped pointer
};
class Type
{
    QList<TypeAncestor> DirectAncestors; //those ancestors can have ancestors too, just like good ole inheritence

    QString Name;
    QList<NonFunctionMember> NonFunctionMembers; //they ARE non-function members, but the resulting code might still yield functions (getters & setters (d->pimpl for shared data and change checking+notification for Q_PROPERTY), change notifier signals in the case of Q_PROPERTIES)

    virtual bool addNonFunctionMember(NonFunctionMember nonFunctionMember)=0;
};
class DesignEqualsImplementationClass_aka_BusinessQObject : public Type
{
    bool addNonFunctionMember(NonFunctionMember nonFunctionMember)
    {
        //QObjects can have qobjects, impliticly shared types, defined elsewhere types, and Q_PROPERTIES, as children
        NonFunctionMembers << nonFunctionMember;
        return true;
    }
};
class DataType_aka_ImplicitlyShared : public Type
{
    bool addNonFunctionMember(NonFunctionMember nonFunctionMember)
    {
        if(qobject_cast<DesignEqualsImplementationClass_aka_BusinessQObject*>(nonFunctionMember.typeInstance->type))
            return false; //only DesignEqualsImplementationClass_aka_BusinessQObjects can have themselves as children

        if(qobject_cast<QPropertyTypeInstance*>(nonFunctionMember.typeInstance))
            return false; //only DesignEqualsImplementationClass_aka_BusinessQObjects can have Q_PROPERTY 'members'

        NonFunctionMembers << nonFunctionMember;
        return true;
    }
};
class DefinedElsewhereType : public Type
{
    //has no reason to have a list of NonFunctionMembers, but it simplifies the design fuggit
    bool addNonFunctionMember(NonFunctionMember nonFunctionMember)
    {
        return false;
    }
};

class TypeInstance
{
    Type type;
    QString Qualifiers_LHS;
    QString Qualifiers_RHS;

    QString VariableName;
};
class QPropertyTypeInstance : public TypeInstance
{
    //I'm unsure if Q_PROPERTY has any use for the LHS/RHS qualifiers, but since they migth and since leaving them empty is a NOOP, I'll inherit them

    //What is the meaning of a 'private' Q_PROPERTY? Does that even make sense? We do at all times want the underlying data of a Q_PROPERTY to be 'private', but it's getters/setters are another story. Perhaps Visibility is disregarded for Q_PROPERTY? Perhaps Q_PROPERTY is [special] a type of Visibility? It doesn't make any sense to have a Q_PROPERTY that isn't publically visible in at least SOME aspect (why expose yourself to meta object system if meta object system can't use you?!?). I'm thinking Visibiity should be disregarded and a qobject_cast to this QPropertyTypeInstance should shortcut the regular Visibility logic (and I should set Visibility to private for Q_PROPERTIES to minimize the damage that a software bug in d=i woul do) TODOreq

    bool HasChangeNotifierSignal;
    bool IsReadOnly;
    //ETC
};

void /*UseCaseClassLifelineInstanceChooserDialog::*/populateComboBoxWithEligibleInstances()
{
    Q_FOREACH(ClassLifeline aClasslifeLine, m_ClassLifeLinesInUseCaseScene)
    {

        Q_FOREACH(NonFunctionMember aNonFunctionMember, aClassLifeLine.ParentClass.NonFunctionMembers)
        {
            if(!qobject_cast<DesignEqualsImplementationClass_aka_BusinessQObject>(aNonFunctionMember.typeInstance->type))
                continue; //only DesignEqualsImplementationClass_aka_BusinessQObjects can be used as class lifeline instances. the other types don't have signals/slots!
            comboBox->addItem(aNonFunctionMember);
        }

    }
}
