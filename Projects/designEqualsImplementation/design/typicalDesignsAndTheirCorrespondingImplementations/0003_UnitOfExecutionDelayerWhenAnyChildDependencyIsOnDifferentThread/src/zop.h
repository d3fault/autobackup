#ifndef ZOP_H
#define ZOP_H

#include <QObject>

class Foo;

class Zop : public QObject
{
    Q_OBJECT
public:
    explicit Zop(QObject *parent = 0);
private:
    Foo *m_Foo;


    QString m_CuntPassThroughAutoGen;
    bool m_AllRecursiveChildrenOnOtherThreadsAreInitialized;
    bool m_ZopSlotCalled;

    void zopSlotActual();
signals:
    void zopSignal(bool success);
public slots:
    void zopSlot(const QString &cunt);
private slots:
    void handleAllFoosChildrenOnDifferentThreadsInitialized();
    void handleFooSignal(bool success);
};

#endif // ZOP_H
