#include "cleanroomguiwidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include "cleanroomsession.h"

#define CLEAN_ROOM_FRONT_PAGE_DOCS 10

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    for(int i = 0; i < CLEAN_ROOM_FRONT_PAGE_DOCS; ++i)
    {
        QLabel *newLabel = new QLabel();
        myLayout->addWidget(newLabel);
        m_LatestCleanRoomDocsGuiWidget.insert(i, newLabel);
    }

    setLayout(myLayout);
}
void CleanRoomGuiWidget::handleNewSessionCreated(CleanRoomSession* session)
{
    m_Session = session;
    m_Session->requestNewCleanRoomFrontPageDefaultView(this, SLOT(handleFrontPageDefaultViewReceived(QStringList)), 69420);
}
void CleanRoomGuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    //qDebug() << "front end received: " << frontPageDocs;
    //TODOreq: ez
    int max = CLEAN_ROOM_FRONT_PAGE_DOCS;
    int numDocs = qMin(frontPageDocs.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        m_LatestCleanRoomDocsGuiWidget.at(i)->setText(frontPageDocs.at(i));
    }
}
