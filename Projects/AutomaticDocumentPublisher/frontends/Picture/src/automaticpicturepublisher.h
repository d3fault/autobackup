#ifndef AUTOMATICPICTUREPUBLISHER_H
#define AUTOMATICPICTUREPUBLISHER_H

#include <QObject>

class AutomaticPicturePublisher : public IAutomaticDocumentPublisher
{
    Q_OBJECT
public:
    explicit AutomaticPicturePublisher(QObject *parent = 0);
signals:
    void picturePublished(bool success);
public slots:
    void publishPicture(const QString &pictureFilePath);
};

#endif // AUTOMATICPICTUREPUBLISHER_H
