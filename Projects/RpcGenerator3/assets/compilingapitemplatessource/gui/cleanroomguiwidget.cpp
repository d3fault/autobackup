#include "cleanroomguiwidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include "cleanroomsession.h"

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    for(int i = 0; i < NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE; ++i)
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
    int max = NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE;
    int numDocs = qMin(frontPageDocs.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        m_LatestCleanRoomDocsGuiWidget.at(i)->setText(frontPageDocs.at(i));
    }
}
