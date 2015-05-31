#include "cleanroomguiwidget.h"

#include <QVBoxLayout>
#include <QLabel>

CleanRoomGuiWidget::CleanRoomGuiWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    for(int i = 0; i < NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE; ++i)
    {
        QLabel *newLabel = new QLabel();
        myLayout->addWidget(newLabel);
        m_CleanRoomDocsWidgets.insert(i, newLabel);
    }

    setLayout(myLayout);
}
void CleanRoomGuiWidget::handleNewSessionCreated(CleanRoomSession session)
{
    m_Session.reset(new CleanRoomSession(session)); //we do a 'new' here because we want to +1 the implicitly shared session (notice we're using the copy constructor of CleanRoomSession). If we did "&session" instead, then the pointer in the m_Session scoped pointer would be a dangling pointer. I had to use a scoped pointer around CleanRoomSession in the first place because CleanRoomSession has no default constructor
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
        m_CleanRoomDocsWidgets.at(i)->setText(frontPageDocs.at(i));
    }
}
