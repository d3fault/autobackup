#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <QObject>

class IMessage : public QObject
{
    Q_OBJECT
public:
  void doneWithMessage() { emit doneWithMessageSignal(); }
signals:
  void doneWithMessageSignal();
}

#endif // IMESSAGE_H
