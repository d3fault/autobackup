//THIS FILE IS NOT _GENERATED_, BUT IS USED ONLY FOR TESTING COMPILATION

#include <QObject>

class FirstNameLastNameQObject : public QObject
{
    Q_OBJECT
public:
    explicit FirstNameLastNameQObject(QObject *parent = nullptr);
public slots:
    void someSlot(const QString &firstName, const QString &lastName, const QStringList &top5Movies, const QString &favoriteDinner, const QString &favoriteLunch);
};
