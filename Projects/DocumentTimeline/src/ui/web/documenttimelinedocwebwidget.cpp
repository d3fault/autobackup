#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WImage>
#include <Wt/WMemoryResource>
using namespace Wt;
#include "documenttimelinedocwebwidget.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>

#include "qbytearraywmemoryresource.h"

DocumentTimelineDocWebWidget::DocumentTimelineDocWebWidget(WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_DocWidgetPlaceholder(new WContainerWidget(this))
{ }
void DocumentTimelineDocWebWidget::setDoc(QByteArray doc)
{
    //m_DocTimestampWidget->setTimestampFromTimestampString(docTimestamp);
    //m_DocSubmitterUsername->setUsername(docSubmitterUsername);
    //NVM BECAUSE THE COMMENT: m_Doc->setDoc(doc); //TODOmb: if setDoc is pure virtual, and doc is assumed to be a json document in a qbytearray, we can use json tags
    //NVM BECAUSE THE COMMENT(already there): m_Doc->setDoc(doc); //TODOmb: doc is assumed to be a json document in a qbytearray, we can use json tagging system to search for [optional] markup hints. an example: mimetype: png/image. elaboring a tiny bit just on that one example: an omitting mimetype is assumed to be application/octet-stream (or whatever) by any mimetype-interested code-paths (had:views)
    //close: visualizeDocBetterBasedOnOptionalJsonTags(doc);
    visualizeDocAsBestYouCanBasedOnOptionalJsonTags(doc);
    //m_DocLicense->setDocLicense(docLicense);
}
DocumentTimelineDocWebWidget::~DocumentTimelineDocWebWidget()
{ }
bool DocumentTimelineDocWebWidget::isValidTimelineDoc(const QJsonObject &jsonDocObject) const //TODOreq: use when sanitizing user input. using in vizualization logic as the bare minimum threshold to be visualized at all
{
    return (jsonDocObject.contains("t") && jsonDocObject.contains("u") && jsonDocObject.contains("d") && jsonDocObject.contains("l"));
}
void DocumentTimelineDocWebWidget::visualizeTimestamp(const QDateTime &timestamp)
{
    new WLabel(timestamp.toString().toStdString(), m_DocWidgetPlaceholder);
}
void DocumentTimelineDocWebWidget::visualizeUsername(const QString &username)
{
    new WLabel(username.toStdString(), m_DocWidgetPlaceholder);
}
void DocumentTimelineDocWebWidget::visualizeLicenseIdentifier(const QString &licenseIdentifier)
{
    //TODOreq: clicking it pops up the long name in a modeless dialog, clicking the long name in that modeless dialog brings up the entire license in another modeless dialog (or the same one, whatever)
    new WLabel(licenseIdentifier.toStdString(), m_DocWidgetPlaceholder);
}
void DocumentTimelineDocWebWidget::visualizeDocAsBestYouCanBasedOnOptionalJsonTags(QByteArray doc) //OLD(was called jsonD): the 'd' is to illustrate that it's the 'd' in t,u,d,l. d is a json object.
{
    m_DocWidgetPlaceholder->clear();

#if 0
    bool foundSomeOptionalJsonTagsThatHelpedMeDetermineDataIsAPicture = false; //etc
    if(foundSomeOptionalJsonTagsThatHelpedMeDetermineDataIsAPicture)
    {
        new WImage(WLink(new WMemoryResource("png/image"), this));
    }
    new WLabel(m_DocWidgetPlaceholder);
#endif

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(doc, &parseError);
    if(parseError.error == QJsonParseError::NoError)
    {
        if(jsonDoc.isObject())
        {
            QJsonObject jsonDocObject = jsonDoc.object();
            if(isValidTimelineDoc(jsonDocObject))
            {
                visualize_TUDL_DocAsBestYouCanBasedOnOptionalJsonTags(jsonDocObject);
                return;
            }
        }
    }
    new WLabel("ERROR: Unable to visualize the document.", m_DocWidgetPlaceholder);
    return;
}
WMemoryResource *DocumentTimelineDocWebWidget::docDjsonFieldToWMemoryStream(const QJsonObject &jsonDocObject, WObject *parent) const
{
    return new QByteArrayWMemoryResource(QByteArray::fromBase64(jsonDocObject.value("d").toString().toUtf8()), parent);
}
void DocumentTimelineDocWebWidget::visualize_TUDL_DocAsBestYouCanBasedOnOptionalJsonTags(const QJsonObject &jsonDocObject)
{
    //keys "t", "u", "d", and "l" are already known to exist (I suppose if I sanitize properly, I can 'know' that they are also valid)

    visualizeTimestamp(QDateTime::fromMSecsSinceEpoch(jsonDocObject.value("t").toString().toLongLong()));
    visualizeUsername(jsonDocObject.value("u").toString());
    visualizeLicenseIdentifier(jsonDocObject.value("l").toString());
    new WBreak(m_DocWidgetPlaceholder);

    if(jsonDocObject.contains("mimetype"))
    {
        visualize_TUDL_Mimetyped_DocAsBestYouCanBasedOnOptionalJsonTags(jsonDocObject, jsonDocObject.value("mimetype").toString());
        return;
    }
    visualize_TUDL_NoMimetype_DocAsBestYouCanBasedOnOptionalJsonTags(jsonDocObject);
    return;
}
void DocumentTimelineDocWebWidget::visualize_TUDL_Mimetyped_DocAsBestYouCanBasedOnOptionalJsonTags(const QJsonObject &jsonDocObject, const QString &mimeType)
{
    QMimeData docMimeData;
    docMimeData.setData(mimeType, QByteArray::fromBase64(jsonDocObject.value("d").toString().toUtf8()));
    if(docMimeData.hasImage())
    {
        //QByteArray docData = QByteArray::fromBase64(jsonDocObject.value("d").toString().toUtf8());
        //WMemoryResource *memoryResource = new WMemoryResource(mimeType.toStdString(), docMimeData.data(mimeType).constData(), this);
        WMemoryResource *memoryResource = docDjsonFieldToWMemoryStream(jsonDocObject, m_DocWidgetPlaceholder);
        WImage *image = new WImage(WLink(memoryResource), m_DocWidgetPlaceholder);
        if(jsonDocObject.contains("tooltipAndAlternativeText"))
        {
            WString tooltipAndAlternativeText(jsonDocObject.value("tooltipAndAlternativeText").toString().toStdString());
            image->setToolTip(tooltipAndAlternativeText);
            image->setAlternateText(tooltipAndAlternativeText);
        }
        return;
    }
    if(docMimeData.hasHtml() || docMimeData.hasText())
    {
        /*WLabel *doc = */new WLabel(QString(QByteArray::fromBase64(jsonDocObject.value("d").toString().toUtf8())).toStdString(), m_DocWidgetPlaceholder);
        return;
    }
    ///*WLabel *fallbackToPlaintextLabel = */new WLabel(QString(QByteArray::fromBase64(jsonDocObject.value("d").toString().toUtf8())).toStdString(), m_DocWidgetPlaceholder);
    visualize_TUDL_NoMimetype_DocAsBestYouCanBasedOnOptionalJsonTags(jsonDocObject);
    return;
}
void DocumentTimelineDocWebWidget::visualize_TUDL_NoMimetype_DocAsBestYouCanBasedOnOptionalJsonTags(const QJsonObject &jsonDocObject)
{
    /*WLabel *fallbackToPlaintextLabel = */new WLabel(QString(QByteArray::fromBase64(jsonDocObject.value("d").toString().toUtf8())).toStdString(), m_DocWidgetPlaceholder);
}

