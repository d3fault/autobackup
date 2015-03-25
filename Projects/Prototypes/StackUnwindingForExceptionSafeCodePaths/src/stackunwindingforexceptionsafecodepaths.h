#ifndef STACKUNWINDINGFOREXCEPTIONSAFECODEPATHS_H
#define STACKUNWINDINGFOREXCEPTIONSAFECODEPATHS_H

#include <QObject>

#include <QScopedPointer>
#include <QSharedPointer>

class StackUnwindingForExceptionSafeCodePaths;

class SomeTask
{
public:
    SomeTask(StackUnwindingForExceptionSafeCodePaths *someTaskProcessor);
    void setSuccess();
    ~SomeTask();
private:
    StackUnwindingForExceptionSafeCodePaths *m_SomeTaskProcessor;
    bool m_Success;
};

typedef QSharedPointer<SomeTask> StackUndwinder;

//this class better named "SomeTaskProcessor"?
class StackUnwindingForExceptionSafeCodePaths : public QObject
{
    Q_OBJECT
public:
    explicit StackUnwindingForExceptionSafeCodePaths(QObject *parent = 0);
private:
    friend class SomeTask;
    void emitFinished(bool success);
signals:
    void finished(bool success);
public slots:
    void doSomeTask(int x);
private slots:
    void bkaskfj(SomeTask theTask);
};

#endif // STACKUNWINDINGFOREXCEPTIONSAFECODEPATHS_H
