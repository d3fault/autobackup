#include "functiondecomposerwithnewtypedetectorgui.h"

#include "functiondecomposerwithnewtypedetector.h"
#include "functiondecomposerwithnewtypedetectorwidget.h"

FunctionDecomposerWithNewTypeDetectorGui::FunctionDecomposerWithNewTypeDetectorGui(QObject *parent)
    : QObject(parent)
{
    FunctionDecomposerWithNewTypeDetector *functionDecomposerWithNewTypeDetector = new FunctionDecomposerWithNewTypeDetector(this);
    m_Gui = new FunctionDecomposerWithNewTypeDetectorWidget();

    connect(m_Gui, SIGNAL(parseFunctionDeclarationRequested(QString)), functionDecomposerWithNewTypeDetector, SLOT(parseFunctionDeclaration(QString)));
    connect(functionDecomposerWithNewTypeDetector, SIGNAL(syntaxErrorDetected()), m_Gui, SLOT(handleSyntaxErrorDetected()));
    connect(functionDecomposerWithNewTypeDetector, SIGNAL(o(QString)), m_Gui, SLOT(handleO(QString)));

    m_Gui->show();
}
FunctionDecomposerWithNewTypeDetectorGui::~FunctionDecomposerWithNewTypeDetectorGui()
{
    delete m_Gui;
}
