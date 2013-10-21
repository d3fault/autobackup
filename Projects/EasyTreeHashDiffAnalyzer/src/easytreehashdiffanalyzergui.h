#ifndef EASYTREEHASHDIFFANALYZERGUI_H
#define EASYTREEHASHDIFFANALYZERGUI_H

#include <QObject>
#include <QCoreApplication>

#include "easytreehashdiffanalyzerwidget.h"
#include "objectonthreadhelper.h"
#include "easytreehashdiffanalyzerbusiness.h"

class EasyTreeHashDiffAnalyzerGui : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeHashDiffAnalyzerGui(QObject *parent = 0);
private:
    EasyTreeHashDiffAnalyzerWidget m_Gui;
    ObjectOnThreadHelper<EasyTreeHashDiffAnalyzerBusiness> m_Business;
private slots:
    void handleEasyTreeHashDiffAnalyzerBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // EASYTREEHASHDIFFANALYZERGUI_H
