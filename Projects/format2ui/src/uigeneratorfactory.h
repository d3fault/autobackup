#ifndef UIGENERATORFACTORY_H
#define UIGENERATORFACTORY_H

#include <QScopedPointer>

class IUIGenerator;

class QtWidgetsUiGenerator;

class UIGeneratorFactory
{
public:
    UIGeneratorFactory();
    IUIGenerator *uiGenerator(const QString &cliArgUiTypeString);
    ~UIGeneratorFactory();
private:
    QScopedPointer<QtWidgetsUiGenerator> m_QtWidgetsUiGenerator;
};

#endif // UIGENERATORFACTORY_H
