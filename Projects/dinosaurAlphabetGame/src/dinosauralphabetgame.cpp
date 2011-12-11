#include "dinosauralphabetgame.h"

DinosaurAlphabetGame::DinosaurAlphabetGame(QObject *parent) :
    QObject(parent)
{
}
void DinosaurAlphabetGame::myConstructor()
{
    m_NumbersToShowAtATime = 5;
}
void DinosaurAlphabetGame::start()
{
    //maybe i'm just paranoid, but i don't trust anything created in my real constructor because at the time any object in there is created it is before we have called "moveToThread" from the widget (which is on the gui thread at the time)
    myConstructor();

    //instead of using a while loop, we use a slot that signals itself indefinitely. we call it here to get it going for the first time. this makes shutting down this object easier as we are running on our own thread. instead of any fancy thread safety mutex or volatile booleans being checked at the top of a while loop, the gui need only send one stop event and the stop handler is called on the correct thread and is guaranteed to not be in the middle of something because it is guaranteed to not be in loopSlot or any other slot because events are processed one at a time (per thread)
    QMetaObject::invokeMethod(this, "loopSlot", Qt::QueuedConnection);
}
void DinosaurAlphabetGame::loopSlot()
{



    if(thereAreStillLettersWeGotWrong())
    {
        QMetaObject::invokeMethod(this, "loopSlot", Qt::QueuedConnection);
    }
    else
    {
        getNewLetters(); //invokes loopSlot for us after it is done
    }
}
void DinosaurAlphabetGame::handleKeyPressed(Qt::Key key)
{
    if(isKeyWeWant(key))
    {
        //move onto the next key, mark that that one is done. move up an index or something

        if(keyWeAreExpectingIsLastKeyInSet())
        {
            //get new keys, OR if any are marked as being ones we got wrong at least once (didn't get it right on first try
        }
    }
    else
    {
        //mark that they got it wrong once so it shows up next time
    }
}
