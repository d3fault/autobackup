#include <QCoreApplication>

#include <QElapsedTimer>
#include <QAtomicPointer>
#include <QSharedData>
#include <QDateTime>
#include <QCryptographicHash>
#include <QSharedPointer>
#include <QTextStream>

//#define NUM_CYCLES 10000 //ten thousand
#define NUM_CYCLES 100000 //hundred thousand

class TheData : public QSharedData
{
public:
    TheData() { }
    TheData(const TheData &other)
        : QSharedData(other), Data(other.Data) { }
    ~TheData() { }

    QByteArray Data;
};

QByteArray myHash(const QByteArray &input)
{
    return QCryptographicHash::hash(input, QCryptographicHash::Sha512);
}

class The
{
public:
    The() { d = new TheData; }
    The(const QByteArray &data) {
        d = new TheData;
        setData(data);
    }
    The(const The &other)
          : d (other.d)
    {
    }

    QByteArray data() const { return d->Data; }
    void setData(const QByteArray &newData)
    {
        d->Data = newData;
    }
private:
    QSharedDataPointer<TheData> d;
};

//This particular use case occurs on servers. You want to give one piece of information (in my case, a filesystem) to a lot of people on different threads, you want to use COW, you want it to be thread safe. I know my "wait 5 mins" + atomic ptr strategy is not infalliable (not sure that's the right word (but I know it sucks, let's just put it that way)). I know that there's probably some weird bug or glitch or use case that I hadn't considered that is going to trigger the 5 minutes. Just the fact that it's POSSIBLE means it's code I should reject. But at the time of writing [Hvbs] I didn't understand what I think I do now: that an atomic increment is maybe only marginally slower than an atomic read. But I could also imagine that an atomic increment could be up to ("near") twice as fast as an atomic read because you only go one length of the bus (increment does not need response, does it? idfk enough about my internals. *continues coding this test*) instead of two (second trip is response)
int main(int argc, char *argv[])
{
    QCoreApplication q(argc, argv);

    QString s(QDateTime::currentDateTime().toString());
    QByteArray a = s.toUtf8();

    The d_0;
    d_0.setData(a);

    TheData *d_1 = new TheData();
    d_1->Data = a;

    //QSharedDataPointer<TheData> sharedData;
    QAtomicPointer<TheData> atomicPointer(d_1);

    QByteArray rollingBufferA = myHash(a);
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    for(int i = 0; i < NUM_CYCLES; ++i)
    {
        TheData d;
        QString s(QDateTime::currentDateTime().toString());
        QByteArray a = s.toUtf8();
        d.Data = a;

        //atomic reference count and const reference
        //sharedData.ref();
        rollingBufferA = myHash(rollingBufferA + d_0.data());
    }
    //calc atomic reference count and const reference time
    qint64 implicitlySharedElapsedTime = elapsedTimer.elapsed();

    QByteArray rollingBufferB = myHash(a);
    elapsedTimer.restart();
    for(int i = 0; i < NUM_CYCLES; ++i)
    {
        TheData d;
        QString s(QDateTime::currentDateTime().toString());
        QByteArray a = s.toUtf8();
        d.Data = a;

        //atomic ptr read and deref
        rollingBufferB = myHash(rollingBufferB + atomicPointer.loadAcquire()->Data);
    }
    //calc atomic ptr elapsed time
    qint64 atomicPointerElapsedTIme = elapsedTimer.elapsed();

    QTextStream stdOut(stdout);
    stdOut << (implicitlySharedElapsedTime < atomicPointerElapsedTIme ? "reference counting a const reference" : "atomic read and derereference") << " was faster" << endl;
    stdOut << "reference counted elapsed: " << implicitlySharedElapsedTime << endl;
    stdOut << "atomic read and dereference elapsed: " << atomicPointerElapsedTIme << endl;
    stdOut << endl << endl;
    stdOut << rollingBufferA.toHex();
    stdOut << endl;
    stdOut << rollingBufferB.toHex();
    stdOut << endl << endl;
    if(rollingBufferA == rollingBufferB)
    {
        stdOut << "they match";
    }
    else
        stdOut << "they don't match";
    stdOut << endl;
    stdOut.flush();

    return 0;
    return q.exec();
}

//I'm forgetting the most important consideration: subsequent reads don't require atomic instruction for the implicit use.
//Btw implicit sharing won anyways rofl, wait nvm they're about even, but still the above sentence is true
//When you get an implicitly shared object, you are guaranteed it won't be deleted so that means you can keep your copy-of (had:reference-to ;-P) it around as a member field of your class.
//One difference is that a signal has to notify every reference count holder of a change, whereas with atomic read and dereference the value will just be different on a read and we won't know because we have nothing to compare it to
