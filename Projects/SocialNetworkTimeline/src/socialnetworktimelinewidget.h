#ifndef SOCIALNETWORKTIMELINEWIDGET_H
#define SOCIALNETWORKTIMELINEWIDGET_H

#include <QWidget>

class QPlainTextEdit;

class SocialNetworkTimelineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SocialNetworkTimelineWidget(QWidget *parent = nullptr);
private:
    QPlainTextEdit *m_DebugOutput;
signals:
    void initializeSocialNetworkTimelineRequested();
    void appendJsonObjectToSocialNetworkTimelineRequested(const QJsonObject & data);
public slots:
    void handleE(QString msg);
    void handleO(QString msg);
    void handleInitializeSocialNetworkTimelineFinished(bool success);
    void handleAppendJsonObjectToSocialNetworkTimelineFinished(bool success);
};

#endif // SOCIALNETWORKTIMELINEWIDGET_H
