#ifndef SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
#define SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H

#include <QString>
#include <QList>

//#include "designequalsimplementationclasslifeline.h"

//TODOreq: should probably just make this a list of TypeInstances. sure I don't need the type, but eh why lose information if I don't need to? but actually on second thought I need to point to the TypeInstance in order for VariableRenames to be easy...
typedef QList<QString> SignalEmissionOrSlotInvocationContextVariables; //TODOreq(WIP): fuck, if a signal emit 'arg' is a member of the class, and we rename that member later, finding it in all use cases' signal slot connection activations for a given class will be a bitch. I should have had all classes/properties/defined-elsewhere-types/ETC all inherit from the same type: Type... or some such

#endif // SIGNALEMISSIONORSLOTINVOCATIONCONTEXTVARIABLES_H
