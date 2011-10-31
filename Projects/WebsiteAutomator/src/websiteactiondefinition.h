#ifndef WEBSITEACTIONDEFINITION_H
#define WEBSITEACTIONDEFINITION_H

#include <QObject>
#include <ParcelizedDefinition.h>

class WebsiteActionDefinition : public QObject
{
    Q_OBJECT
public:
    explicit WebsiteActionDefinition(QObject *parent = 0);
private:
    ParcelizedDefinition m_ParcelizedDefinition;
signals:

public slots:

};

#endif // WEBSITEACTIONDEFINITION_H
