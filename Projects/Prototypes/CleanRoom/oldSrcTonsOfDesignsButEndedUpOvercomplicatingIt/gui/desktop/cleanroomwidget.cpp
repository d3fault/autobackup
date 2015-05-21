#include "cleanroomwidget.h"

#include <QVBoxLayout>

CleanRoomWidget::CleanRoomWidget(QWidget *parent)
    : QWidget(parent)
    , m_Layout(new QVBoxLayout())
{
    setLayout(m_Layout);
}
void CleanRoomWidget::respond(ICleanRoomSessionRequest *request, ICleanRoomSessionResponse *response)
{
    request->responseMetaMethod.invoke(request->responseMetaObject(), Qt::AutoConnection, Q_ARG(ICleanRoomSessionResponse, response));
}
void CleanRoomWidget::handleFrontPageUpdated(ICleanRoomSessionResponse *response)
{
    qDebug("handleFrontPageUpdated");
    QList<CleanRoomDoc> frontPageDocs = responseToFrontPageDocs(response);
    Q_FOREACH(CleanRoomDoc currentCleanRoomDoc, frontPageDocs)
    {
        m_Layout->addWidget(new CleanRoomDocWidget(currentCleanRoomDoc));
    }
}
