#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = 0);

    template<class T>
    void connectToUi(T *uiPtr)
    {
        connect(uiPtr, &T::doFuckRequested, this, &Backend::doFuck);
    }
public slots:
    void doFuck();
};

#endif // BACKEND_H
