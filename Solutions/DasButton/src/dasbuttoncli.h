#ifndef DASBUTTONCLI_H
#define DASBUTTONCLI_H

#include <QObject>
#include <QTextStream>

class DasButton;

class DasButtonCli : public QObject
{
    Q_OBJECT
public:
    explicit DasButtonCli(QObject *parent = 0);
private:
    DasButton *m_DasButton;
    QTextStream m_StdOut;
    QTextStream m_StdErr;
signals:

private slots:
    void handlePressFinished(bool pressWasSuccessful);
    void handleO(const QString &msg);
    void handleE(const QString &msg);
};

#endif // DASBUTTONCLI_H
