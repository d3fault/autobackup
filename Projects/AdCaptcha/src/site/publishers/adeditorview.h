#ifndef ADEDITORVIEW_H
#define ADEDITORVIEW_H

#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
using namespace Wt;

#include "../../database.h"

class AdEditorView : public WContainerWidget
{
public:
    AdEditorView(Database *dbInstance, WContainerWidget *parent = 0);
private:
    Database *m_MyDb;
};

#endif // ADEDITORVIEW_H
