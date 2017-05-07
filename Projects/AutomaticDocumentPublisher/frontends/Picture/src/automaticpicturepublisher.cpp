#include "automaticpicturepublisher.h"

#include <QDir>

AutomaticPicturePublisher::AutomaticPicturePublisher(QObject *parent)
    : QObject(parent)
{ }
void AutomaticPicturePublisher::publishPicture(const QString &pictureFilePath)
{
    QString localPictureRepoPath = "/run/shm/pictures"; //TODOreq: custom obviously
    QString remotePictureRepoPath = "/run/shm/picturesRemote";

    //TODOreq: [genericize BEGIN]
    QDir localPictureRepoPathDir(localPictureRepoPath);
    if(!localPictureRepoPathDir.exists(localPictureRepoPath))
    {
        if(!localPictureRepoPathDir.mkpath(localPictureRepoPath))
        {
            emit e("failed to make path: " + localPictureRepoPath);
            emit picturePublished(false);
            return;
        }
    }
    gitAdd(pictureFilePath);
    gitCommit("Picture");
    gitPush(remotePictureRepoPath);
    //I'm already bored of this app... but it's beauty isn't in it's complexity. It's extremely simple but it will still save assloads of time because it's auto-fucking-matic. I've obviously designed it to be extensible as fuck. so even though I dread the thought of coding another "git add/commit/push" app, I'll power through this shit because it's worth it
    emit picturePublished(true);
    //[genericize END]
}
