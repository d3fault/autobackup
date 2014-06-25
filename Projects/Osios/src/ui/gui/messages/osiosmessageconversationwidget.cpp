#include "osiosmessageconversationwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

//TODOoptional: for the email plugin/mode/version/whatever, we can bum off of the already existing SSL infrastructure for now. Emails should be downloaded (saved to 3 local drives), kept in server inbox, _AND_ ideally replicated to dht we are a part of
//TODOreq: "sending" a file to a user just links them to the file on the dht (the sha1 is sent directly to them via ssl). Now the file is downloaded by them and they keep it on their local machine too (TODOoptimization: they DON'T need to replicate the file 3x on the dht because it is already there. the two people share keeping track of (including periodic verifying ideally (idk if verify jobs are shared since unsafe)) the files on the dht) since it is now a part of their own history (timeline view)
OsiosMessageConversationWidget::OsiosMessageConversationWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout(this);
    QPlainTextEdit *messageHistoryPlainTextEdit = new QPlainTextEdit();
    messageHistoryPlainTextEdit->setReadOnly(true);
    messageHistoryPlainTextEdit->setMaximumBlockCount(6900); //memory optimization only, has nothing to do with what is persisted (all messages of all conversations). TODOreq: "see older messages", perhaps just a link to timeline view presented horizontally at that position. //TODOoptionial: allow number to be specifiable in some option [in gui]

    QHBoxLayout *sendMessageRow = new QHBoxLayout();
    QPlainTextEdit *messageSendDraftPlainTextEdit = new QPlainTextEdit(); //TODOoptional: backspaces, although kept for archival reasons (maybe opt-out-with-warning adjustable), are not sent to recipient. Likewise, you can opt-IN to viewing your recipient's OWN backspaces [if he didn't opt out of the admittedly-could-be-scary functionality]

    sendMessageRow->addWidget(messageSendDraftPlainTextEdit);
    sendMessageRow->addWidget(new QPushButton(tr("Send")));

    myLayout->addWidget(messageHistoryPlainTextEdit, 1);
    myLayout->addLayout(sendMessageRow);
}
