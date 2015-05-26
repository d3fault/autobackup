#include <Wt/WContainerWidget>
#include <Wt/WMemoryResource>
using namespace Wt;
#ifndef DOCUMENTTIMELINEDOCWEBWIDGET_H
#define DOCUMENTTIMELINEDOCWEBWIDGET_H

#include <QByteArray>
#include <QJsonObject>
#include <QDateTime>

class DocumentTimelineDocWebWidget : public WContainerWidget
{
public:
    DocumentTimelineDocWebWidget(WContainerWidget *parent = 0);
    void setDoc(/*const std::string &docTimestamp, const std::string &docSubmitterUsername, */QByteArray doc/*, const std::string &docLicense*/);
    ~DocumentTimelineDocWebWidget();
private:
    WContainerWidget *m_DocWidgetPlaceholder;

    bool isValidTimelineDoc(const QJsonObject &jsonDocObject) const;
    void visualizeTimestamp(const QDateTime &timestamp);
    void visualizeUsername(const QString &username);
    void visualizeLicenseIdentifier(const QString &licenseIdentifier);
    void visualizeDocAsBestYouCanBasedOnOptionalJsonTags(QByteArray doc);
    Wt::WMemoryResource *docDjsonFieldToWMemoryStream(const QJsonObject &jsonDocObject, WObject *parent = 0) const;
    void visualize_TUDL_DocAsBestYouCanBasedOnOptionalJsonTags(const QJsonObject &jsonDocObject);
    void visualize_TUDL_Mimetyped_DocAsBestYouCanBasedOnOptionalJsonTags(const QJsonObject &jsonDocObject, const QString &mimeType);
    void visualize_TUDL_NoMimetype_DocAsBestYouCanBasedOnOptionalJsonTags(const QJsonObject &jsonDocObject);
};

#endif // DOCUMENTTIMELINEDOCWEBWIDGET_H
