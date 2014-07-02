#include "bar.h"

Bar::Bar(QObject *parent)
    : QObject(parent)
{ }
void Bar::barSlot(const QString &cunt)
{
    if(cunt == "test")
    {
        emit barSignal(true); //TODOreq: see note about mixing C++ code with these in-design signal emissions in fooSlot, which deals with similar stuff. HOWEVER this brings up a yet-unsolved problem of mixing custom C++ code to _DETERMINE_ the args used in signal emission and slot invocation. One solution off the top of my head: if they put a privateMethod in front of a signal emission or slot invocation, they can REQUEST an object (given to their privateMethod as an argument) that will be used for the signal emission or slot invocation. Multiple privateMethods back to back (or something?) can(should?) use the SAME argument object btw (but this sentence is confusing me more than anything). So for example: Bar::someBarPrivateMethod(BarSignalArgumentObject *barSignalArgumentObject) { if(customCppCalls()) barSignalArgumentObject->success = true; else barSignalArgumentObject->success = false; } -- BarSignalArgumentObject is obviously auto-generated. When "sucking in" their C++ code, we could maybe using libClang or IDFK check+verify somehow that they specified all the signal/slot parameters and yell at them if they forgot any (to start off and to KISS initially, though, we could just use "default values" for the arguments (regardless of whether or not the slot normally accepts default values (signals can't so are irrelevant to that statement))). Since here "cunt" is an argument we want to act upon, that needs to be accounted for and given to their privateMethod as well (I think? Maybe they have to request that too actually idfk)
    }
    else
    {
        emit barSignal(false);
    }
}
