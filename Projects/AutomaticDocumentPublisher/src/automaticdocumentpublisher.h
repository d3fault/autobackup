#ifndef AUTOMATICDOCUMENTPUBLISHER_H
#define AUTOMATICDOCUMENTPUBLISHER_H

#include <QObject>

#include <QMimeType>

class AutomaticDocumentPublisher : public QObject
{
    Q_OBJECT
public:
    explicit AutomaticDocumentPublisher(QObject *parent = 0);

signals:
    void documentPublished(bool success);
public slots:
    void publishDocument(const QString &documentFilePath, const QMimeType &documentType);
};

#endif // AUTOMATICDOCUMENTPUBLISHER_H
