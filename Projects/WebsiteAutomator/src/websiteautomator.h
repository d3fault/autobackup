#ifndef WEBSITEAUTOMATOR_H
#define WEBSITEAUTOMATOR_H

#include <QObject>

class WebsiteAutomator : public QObject
{
    Q_OBJECT
public:
    explicit WebsiteAutomator(QObject *parent = 0);
    void startAutomating();
signals:

public slots:

};

#endif // WEBSITEAUTOMATOR_H
