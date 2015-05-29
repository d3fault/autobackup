#include <Wt/WDialog>
using namespace Wt;
#ifndef DOCUMENTTIMELINEREGISTERSUBMITVIDEOWIDGET_H
#define DOCUMENTTIMELINEREGISTERSUBMITVIDEOWIDGET_H

#include <QString>

class DocumentTimelineRegisterSubmitVideoWidget : public WDialog
{
public:
    DocumentTimelineRegisterSubmitVideoWidget(const QString &dataUserMustReciteInRegistrationAttemptVideo);
    QString desiredUsername() const;
    QString password() const;
    QString filePathOfJustUploadedRegistrationAttemptVideo() const;
};

#endif // DOCUMENTTIMELINEREGISTERSUBMITVIDEOWIDGET_H
