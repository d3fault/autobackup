#include <QObject>

class FirstNameLastNameQObject : public QObject
{
    Q_OBJECT
public:
    explicit FirstNameLastNameQObject(QObject *parent = nullptr);
public slots:
    void someSlot(const QString &firstName, const QString &lastName);
};
