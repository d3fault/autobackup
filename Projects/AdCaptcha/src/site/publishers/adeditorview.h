#ifndef ADEDITORVIEW_H
#define ADEDITORVIEW_H

#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WAnchor>
using namespace Wt;

#include "../adcaptchasite.h"
#include "../../database.h"

class AdEditorView : public WContainerWidget
{
public:
    AdEditorView(Database *dbInstance, WContainerWidget *parent = 0);
private:
    Database *m_MyDb;
};

#endif // ADEDITORVIEW_H
