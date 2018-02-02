#ifndef QTWIDGETSUIGENERATOR_H
#define QTWIDGETSUIGENERATOR_H

#include "iuigenerator.h"

#include "uivariable.h"

class QtWidgetsUiGenerator : public IUIGenerator
{
protected:
    bool generateUi(const QJsonObject &jsonUiFormatInput) override;
private:
    void streamOutUiVariableInstantiationCode(QTextStream &outputSourceFileTextStream, const UIVariable &uiVariable);
};

#endif // QTWIDGETSUIGENERATOR_H
