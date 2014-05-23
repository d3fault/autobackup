#ifndef D3FAULTPUBLICLICENSECOPYRIGHTHEADERBULKPREPENDERCLI_H
#define D3FAULTPUBLICLICENSECOPYRIGHTHEADERBULKPREPENDERCLI_H

#include <QObject>
#include <QTextStream>

class D3faultPublicLicenseCopyrightHeaderBulkPrepender;

class D3faultPublicLicenseCopyrightHeaderBulkPrependerCli : public QObject
{
    Q_OBJECT
public:
    explicit D3faultPublicLicenseCopyrightHeaderBulkPrependerCli(QObject *parent = 0);
private:
    D3faultPublicLicenseCopyrightHeaderBulkPrepender *m_D3faultPublicLicenseCopyrightHeaderBulkPrepender;
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void usage();
signals:

private slots:
    void handleD3faultPublicLicenseCopyrightHeaderBulkPrependerFinished(bool success);
    void handleO(const QString &msg);
    void handleE(const QString &msg);
};

#endif // D3FAULTPUBLICLICENSECOPYRIGHTHEADERBULKPREPENDERCLI_H
