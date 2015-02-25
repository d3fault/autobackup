#ifndef MUSICFINGERSCLI_H
#define MUSICFINGERSCLI_H

#include <QObject>
#include <QTextStream>

class MusicFingersCli : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingersCli(QObject *parent = 0);
    QTextStream m_StdErr;
private slots:
    void handleE(const QString &e);
};

#endif // MUSICFINGERSCLI_H
