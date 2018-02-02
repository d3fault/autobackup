#include "uigeneratorfactory.h"

#include "qtwidgetsuigenerator.h"

UIGeneratorFactory::UIGeneratorFactory()
    : m_QtWidgetsUiGenerator(new QtWidgetsUiGenerator())
{ }
IUIGenerator *UIGeneratorFactory::uiGenerator(const QString &cliArgUiTypeString)
{
    if(cliArgUiTypeString == "--qtwidgets")
    {
        return m_QtWidgetsUiGenerator.data();
    }
    return nullptr;
}
UIGeneratorFactory::~UIGeneratorFactory()
{

}
