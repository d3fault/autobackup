#ifndef ACTIONMESSAGEDISPENSER_H
#define ACTIONMESSAGEDISPENSER_H

#include <QObject>
#include <QList>


//gah maybe templates aren't the answer. how do a signal off of T (messageDone) to a slot of our own? doesn't T need to be a more specified type (interface?) in order for me to connect to it's signal?
//i think i need to make a barebones prototype that:
//first: WORKS
//second: i trim/minimize it, committing to git in each step
//third: take the finalized product/prototype and use what i learn (or the code itself, copy paste) in this actual project... shit's just getting too confusing
//i also think a prototype would duplicate needlessly a lot of the code i already have here
//i mean sure it'd make me more experimental / less scared of breaking shit... but this code is already pretty damn simple/empty. i've yet to really do anything hah. god damnit i need to fix my git commit logs... that's what i need to have a reference point to safely refer to / roll back to

//but still, if i use interfaces i I i I i i iI Ii I i i i i i i i like i better fuck yea laziness

//subtle typos can give you away
//same with caps
//so type all lower caps and use perfect spelling
//is the only *chance* of hiding
//if you choose to hide
//naw mean?

//shit what was i on about?
//if i use interfaces then i have to cast :(
//or i could just not use interfaces and have duplicate code (auto-generated anyways)
//the downside is a larger binary
//is casting a slow operation? is it even a run-time operation? i think it is, but what is it's overhead? should i give a shit?
//i already know that since each dispenser is used in a specific/selected area, we'll already implicitly know what to cast into
//so that's not the problem. the problem is only that i do not wish to cast...
//maybe i'm just being a crybaby and over-optimizing / over-engineering
//i'm definitely over-engineering this ABC project~
//err, solution :-P
//solution to money problems

//but also, this rpc thing is going to be sooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo fucking useful. I can see myself using it for hundreds of products
//it is a product. i could sell it
//it is also a product in that someone else might buy it. err the above line is referring to licenses. this one is referring to the "IP" itself. not that RPC generators are rare... but i don't know of any that are Qt (cross platform as FUCK) and Ssl / thread / maybe-IPC enabled. this message recycling shit is dumb by comparison to the overall rpc project... which is a sub-project / tangent / simultaneous effort to the ABC proj-solution
//i could also expose it in an AWS fashion
//"easy service maker" web app
//auto-dht, auto-rpc
//you provide business logic, we provide storage + networking + maybe even web front end (wt shit auto-gen'd?)


//to interface/cast, or not to interface/cast... that is the question

template <class T>
class ActionMessageDispenser : public QObject
{
    Q_OBJECT
public:
    explicit ActionMessageDispenser(QObject *parent = 0)
        : QObject(parent)
    {
        m_List = new QList<T>();
    }
    T getNewOrRecycled()
    {
        if(m_List->isEmpty())
        {
            T newItem = new T();
            //connect it to be used with messageDone?
            return newItem;
        }
        return m_List->takeLast();
        //associate with network message? i think it needs to be passed in as a parameter
    }
    void recycleItem(T itemToBeRecycled)
    {
        m_List->append(itemToBeRecycled);
    }
private:
    QList<T> *m_List;
};

#endif // ACTIONMESSAGEDISPENSER_H
