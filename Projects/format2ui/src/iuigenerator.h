#ifndef IUIGENERATOR_H
#define IUIGENERATOR_H

#include <QJsonObject>

class IUIGenerator
{
public:
    virtual bool generateUi(const QJsonObject &jsonUiFormatInput)=0;
};

#endif // IUIGENERATOR_H
