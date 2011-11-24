#include "layoutappendupdatesgui.h"

LayoutAppendUpdatesGui::LayoutAppendUpdatesGui(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *mainWidget = new QWidget();
    m_LayoutToAppendTo = new QVBoxLayout(mainWidget);
    m_ItemToAppend = new QPushButton(tr("Fuck"));
    m_LayoutToAppendTo->addWidget(m_ItemToAppend);
    mainWidget->setLayout(m_LayoutToAppendTo);
    this->setCentralWidget(mainWidget);
    connect(m_ItemToAppend, SIGNAL(clicked()), this, SLOT(appendItem()));
}
LayoutAppendUpdatesGui::~LayoutAppendUpdatesGui()
{

}
//bool LayoutAppendUpdatesGui::doesLayoutAppendUpdateGui()
//{

//}
void LayoutAppendUpdatesGui::appendItem()
{
    m_LayoutToAppendTo->addWidget(m_ItemToAppend);
    m_LayoutToAppendTo->parentWidget()->setLayout(m_LayoutToAppendTo);
    this->setCentralWidget(m_LayoutToAppendTo->parentWidget());

    this->update();
}
