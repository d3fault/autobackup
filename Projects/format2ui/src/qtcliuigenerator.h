#ifndef QTCLIUIGENERATOR_H
#define QTCLIUIGENERATOR_H

#include "iuigenerator.h"

class QtCliUiGenerator : public IUIGenerator
{
protected:
    bool generateUi(const QJsonObject &jsonUiFormatInput) override;
private:
    void streamOutUiCliQueriesViaCinCoutShiz(QTextStream &outputSourceFileTextStream, const UIVariable &uiVariable);
};

#endif // QTCLIUIGENERATOR_H
