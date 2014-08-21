#ifndef FUNCTIONDECOMPOSERWITHNEWTYPEDETECTORGUI_H
#define FUNCTIONDECOMPOSERWITHNEWTYPEDETECTORGUI_H

#include <QObject>

class FunctionDecomposerWithNewTypeDetectorWidget;

class FunctionDecomposerWithNewTypeDetectorGui : public QObject
{
    Q_OBJECT
public:
    explicit FunctionDecomposerWithNewTypeDetectorGui(QObject *parent = 0);
    ~FunctionDecomposerWithNewTypeDetectorGui();
private:
    FunctionDecomposerWithNewTypeDetectorWidget *m_Gui;
};

#endif // FUNCTIONDECOMPOSERWITHNEWTYPEDETECTORGUI_H
