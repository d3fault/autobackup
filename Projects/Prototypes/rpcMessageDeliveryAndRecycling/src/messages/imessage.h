#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <QObject>

class IMessage : public QObject
{
    Q_OBJECT
public:
  void doneWithMessage() { emit doneWithMessageSignal(); }
  //void deliver() { privateDeliver(); }
protected:
  //virtual void privateDeliver()=0; //each specific message must override this type. all it does is emit it's message-specific signal
signals:
  void doneWithMessageSignal();
};

#endif // IMESSAGE_H
