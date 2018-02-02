#include "uigeneratorfactory.h"

#include "qtwidgetsuigenerator.h"
#include "qtcliuigenerator.h"

UIGeneratorFactory::UIGeneratorFactory()
    : m_QtWidgetsUiGenerator(new QtWidgetsUiGenerator())
    , m_QtCliUiGenerator(new QtCliUiGenerator())
{ }
IUIGenerator *UIGeneratorFactory::uiGenerator(const QString &cliArgUiTypeString)
{
    if(cliArgUiTypeString == "--qtwidgets")
        return m_QtWidgetsUiGenerator.data();
    if(cliArgUiTypeString == "--qtcli")
        return m_QtCliUiGenerator.data();
    return nullptr;
}
UIGeneratorFactory::~UIGeneratorFactory()
{

}
