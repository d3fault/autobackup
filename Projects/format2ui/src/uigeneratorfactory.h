#ifndef UIGENERATORFACTORY_H
#define UIGENERATORFACTORY_H

#include <QScopedPointer>

class IUIGenerator;

class QtWidgetsUiGenerator;
class QtCliUiGenerator;

class UIGeneratorFactory
{
public:
    UIGeneratorFactory();
    IUIGenerator *uiGenerator(const QString &cliArgUiTypeString);
    ~UIGeneratorFactory();
private:
    QScopedPointer<QtWidgetsUiGenerator> m_QtWidgetsUiGenerator;
    QScopedPointer<QtCliUiGenerator> m_QtCliUiGenerator;
};

#endif // UIGENERATORFACTORY_H
