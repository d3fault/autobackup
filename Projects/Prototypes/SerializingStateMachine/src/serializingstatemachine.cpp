#include "serializingstatemachine.h"

#include <QStateMachine>
#include <QState>

//My context when writing this prototype is trying to serialize Osios2's application state (based on a series of timeline nodes (mutations to said application state)), and I want Osios2 to maybe be a state machine (if I can, I will)
SerializingStateMachine::SerializingStateMachine(QObject *parent)
    : QObject(parent)
{
    //initial state = "empty json doc"
    //first state = "profile name declared"
    //etc
    //but how to serialize from that?

    QStateMachine *stateMachine = new QStateMachine(this);

    QState *emptyJsonDocState = new QState();
    stateMachine->addState(emptyJsonDocState);
    stateMachine->setInitialState(emptyJsonDocState);

    //technically should be able to serialize even when profile name is not declared, to a 0-byte-len binary (or an empty json doc)
    //but how????

    //QState *profileDeclared;
    QState *stateToRepresentTheCurrentRollingHash; //? so in this case there is no empty json doc state, just a 'null current rolling hash' state

    //we need to dynamically retrieve the previous state and set up transitions from it to the new state?


    QState *oldState;
    oldState->addTransition(newState, SIGNAL(entered()), copyOldStateAndApplyMutationsOfNewState);
    QState *newState;



    //idk wtf im doing

    //I think I want assignProperty to be called with the app state as the first argument, and those "json fields" exposed as Q_PROPERTYs?
    //Do I gain anything from all this? Maybe I do and just don't see it yet, maybe I don't and am just over-organizing. I actually think I might, like constrained execution paths or something (eliminating certain use case flow errors implicitly rather than explicitly)

    /*
    WOWOW just stumbled upon:

    QEventTransition *enterTransition = new QEventTransition(button, QEvent::Enter);
    enterTransition->setTargetState(s2);
    s1->addTransition(enterTransition);

    I can/should(?) totally use QStateMachine to resynthesize received events in Osios2 xD (imagine the above 'hover' equivalent as 'key press'/etc), would save me lines I think (but getting it to actually work might prove to be a bitch)
    I want KISS, but that's what QStateMachine achieves with ease. I'm just rusty as fuck with state machine stuff is all
    */

    //Ok so I was kind of on track! my "states" are going to be identified by currentRollingHash!!! each state simply assignProperties the app state, which is more or less a POD (Q_PROPERTY) object (and serializing those is easy)
    //There is also a "mutateOldRollingHashIdentifiedStateIntoNewRollingHashIdentifiedState" event transition between every state. Note that it works on the full app state contents (that just so happen to be identified by the current rolling hash), not just the current rolling hash itself
    //I either want an interface to implement in a custom event transition, or an interface to implement in states. Idk which tbh. Err what I'm getting at is idk if a timeline node is an event transition or a state. I would argue that it's an event transition (mutates the state), but weirdly I think I have it misunderstood because isn't assignProperty (the mutation) associated with the state, not the state transition? wtf is a state transition then? Ok transitions just link states together, simple. But wait what, no, it's modifying the button by sending it a QEvent::Enter signal :-/...
    //Also worth noting that I can use dynamic properties instead of preset ones, but it makes little difference.
    //Hmm I do want to look into this more, but I'm getting ahead of myself and should focus on finishing the required code to support copycat testing. Don't need to serialize app state, will just always start from timeline node 0 when playing back (requirement for now). But that does mean I need to finish the hello code (right?)
}
