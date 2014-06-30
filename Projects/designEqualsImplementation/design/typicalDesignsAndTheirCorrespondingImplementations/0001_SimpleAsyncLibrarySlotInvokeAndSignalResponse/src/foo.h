#ifndef FOO_H
#define FOO_H

#include <QObject>

class Foo : public QObject
{
    Q_OBJECT
public:
    explicit Foo(QObject *parent = 0);
signals:
    void diagnosticSignalX();
    void fooSignal(bool success);

    //auto-generated, not part of uml/design:
    void barSlotRequested(const QString &);
public slots:
    void fooSlot(const QString &cunt);
    void handleBarSignal(bool success);
};

#endif // FOO_H
