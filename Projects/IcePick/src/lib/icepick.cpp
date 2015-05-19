#include "icepick.h"

#include <QKeySequence>
//#include <QXmlStreamReader>
#include <QTextStream>
#include <QtNetwork/QNetworkReply>
#include <QDateTime>

//TODOoptional: use stars rating as weight (boost::random for this)
IcePick::IcePick(QObject *parent)
    : QObject(parent)
    , m_SettingsAndCache(QSettings::IniFormat, QSettings::UserScope, "IcePickOrganization", "IcePick")
    , m_NetworkAccessManager(new QNetworkAccessManager(this))
{
    m_AllIcefilmPages.append('1');
    for(int i = Qt::Key_A; i <= Qt::Key_Z; ++i)
    {
        m_AllIcefilmPages.append(QKeySequence(i).toString().at(0).toUpper());
    }

    connect(m_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkRequestFinished(QNetworkReply*)));
}
void IcePick::addAllIcePagesToCache()
{
    m_PagesIterator.reset(new QListIterator<QChar>(m_AllIcefilmPages));
    requestNextPageOrContinueOntoMainFunctionality();
}
void IcePick::requestNextPageOrContinueOntoMainFunctionality()
{
    if(m_PagesIterator->hasNext())
    {
        requestIcePage(m_PagesIterator->next());
        return;
    }
    //continue onto main functionality
    buildFlatListWithinYearRangeAndThenGenerateRandomNumberToSelectEntry();
}
void IcePick::requestIcePage(QChar pageToRequest)
{
    m_NetworkAccessManager->get(QNetworkRequest(QUrl(ICEFILMS_MOVIES_URL_PREFIX + QString(pageToRequest))));
}
bool IcePick::parseNetworkReply(QNetworkReply *networkReply)
{
    QTextStream networkReplyStream(networkReply);
    QString networkReplyString = networkReplyStream.readAll();

    QString hrefValueWeWant(ICEFILMS_ENTRY_BASE_PATH);
    int indexOfFirstCharofHrefWeWant = networkReplyString.indexOf(hrefValueWeWant);
    while(indexOfFirstCharofHrefWeWant > -1)
    {
        QString vKeyInUrl;
        int indexOfNextGreaterThanSignAfterThatHref = networkReplyString.indexOf(">", indexOfFirstCharofHrefWeWant);
        if(indexOfNextGreaterThanSignAfterThatHref > -1)
        {
            int indexOfNextLessThanSignAfterTheFirstGreaterThanSign = networkReplyString.indexOf("<", indexOfNextGreaterThanSignAfterThatHref);
            if(indexOfNextLessThanSignAfterTheFirstGreaterThanSign > -1)
            {
                int endIndexOfHrefPrefix = indexOfFirstCharofHrefWeWant+hrefValueWeWant.length();
                QString vKeyInUrlRaw = networkReplyString.mid(endIndexOfHrefPrefix, indexOfNextGreaterThanSignAfterThatHref-endIndexOfHrefPrefix);
                vKeyInUrlRaw = vKeyInUrlRaw.trimmed();
                if(vKeyInUrlRaw.endsWith("\"") || vKeyInUrlRaw.endsWith("'"))
                    vKeyInUrlRaw = vKeyInUrlRaw.left(vKeyInUrlRaw.size()-1);
                if(vKeyInUrlRaw.endsWith("&"))
                    vKeyInUrlRaw = vKeyInUrlRaw.left(vKeyInUrlRaw.size()-1);
                vKeyInUrl = vKeyInUrlRaw;

                int startIndexOfHrefText = indexOfNextGreaterThanSignAfterThatHref+1;
                const QStringRef titleAndDateRaw = networkReplyString.midRef(startIndexOfHrefText, indexOfNextLessThanSignAfterTheFirstGreaterThanSign-startIndexOfHrefText);
                IceEntry iceEntry;
                iceEntry.Url = vKeyInUrl;
                if(!parseTitleAndDate(titleAndDateRaw, iceEntry))
                    return false;

                m_SettingsAndCache.beginGroup(m_PagesIterator->peekPrevious());
                m_SettingsAndCache.beginGroup(iceEntry.Url);

                m_SettingsAndCache.setValue("title", iceEntry.Title);
                m_SettingsAndCache.setValue("year", iceEntry.Year);

                m_SettingsAndCache.endGroup();
                m_SettingsAndCache.endGroup();
            }
        }

        indexOfFirstCharofHrefWeWant = networkReplyString.indexOf(hrefValueWeWant, indexOfFirstCharofHrefWeWant+1);
    }

#if 0 //doesn't work, html/xml is possibly malformed
    QXmlStreamReader iceParser(networkReply);
    //QByteArray networkReplyBytes = networkReply->readAll();
    //QXmlStreamReader iceParser(networkReplyBytes,);
    bool inHtml = false;
    bool inBody = false;
    bool inDiv = false;
    bool inSpanList = false;
    //while(iceParser.atEnd())
    while(iceParser.readNextStartElement())
    {
        //iceParser.readNext();
        QStringRef currentElementName = iceParser.name();
        if(currentElementName == "html")
        {
            inHtml = iceParser.isStartElement();
            continue;
        }
        if(!inHtml)
            continue;
        if(currentElementName == "body")
        {
            inBody = iceParser.isStartElement();
            continue;
        }
        if(!inBody)
            continue;
        if(currentElementName == "div")
        {
            inDiv = iceParser.isStartElement();
            continue;
        }
        if(!inDiv)
            continue;
        if(currentElementName == "span")
        {
            QXmlStreamAttributes nodeAttributes = iceParser.attributes();
            if(nodeAttributes.value("class") == "list")
            {
                inSpanList = iceParser.isStartElement();
                if(!inSpanList)
                    break; //no more entries after the span element ends
            }
        }
        if(!inSpanList)
            continue;
        if(currentElementName == "a")
        {
            QXmlStreamAttributes nodeAttributes = iceParser.attributes();
            QStringRef hrefValue = nodeAttributes.value("href");
            if(hrefValue.startsWith(ICEFILMS_ENTRY_BASE_PATH))
            {
                IceEntry iceEntry;
                iceEntry.Url = hrefValue.toString();
                parseTitleAndDate(iceParser.text(), iceEntry);

                m_SettingsAndCache.beginGroup(m_PagesIterator->peekPrevious());
                m_SettingsAndCache.beginGroup(iceEntry.Url.toString().toUtf8().toBase64());

                m_SettingsAndCache.setValue("title", iceEntry.Title);
                m_SettingsAndCache.setValue("year", iceEntry.Year);

                m_SettingsAndCache.endGroup();
                m_SettingsAndCache.endGroup();
            }
        }
    }
    if(iceParser.hasError())
    {
        emit e("ice parser had error: " + iceParser.errorString());
        emit finishedPickingIce(false);
        return;
    }
#endif

    return true;
}
bool IcePick::parseTitleAndDate(const QStringRef &titleAndDateText, IceEntry& iceEntry)
{
    if(titleAndDateText.isEmpty())
        return;
    bool sawCloseParenthesis = false;
    bool parsedYear = false;
    int titleAndDateTextLength = titleAndDateText.size();
    QString yearString;
    for(int i = (titleAndDateTextLength - 1); i > -1; --i)
    {
        const QChar currentChar = titleAndDateText.at(i);
        if(!parsedYear)
        {
            //parse year
            if(currentChar == ')')
            {
                sawCloseParenthesis = true;
                continue;
            }
            if(!sawCloseParenthesis)
                continue;
            if(currentChar == '(')
            {
                bool convertOk = false;
                iceEntry.Year = yearString.toInt(&convertOk);
                if(!convertOk)
                {
                    emit e("failed to parse year from line: " + titleAndDateText.toString());
                    emit finishedPickingIce(false);
                    return false;
                }
                parsedYear = true;
                continue;
            }
            yearString.insert(0, currentChar);

            continue;
        }
        else
        {
            //parse title
            iceEntry.Title.insert(0, currentChar);

            continue;
        }
    }
    iceEntry.Title = iceEntry.Title.trimmed(); //there's a trailing slash from the gap between the title and the year
    return true;
}
void IcePick::buildFlatListWithinYearRangeAndThenGenerateRandomNumberToSelectEntry()
{
    m_SettingsAndCache.setValue("haveCache", true);
    QList<IceEntry> iceEntryList;
    QStringList pages = m_SettingsAndCache.childGroups();
    int numTotalEntries = 0;
    Q_FOREACH(QString page, pages)
    {
        m_SettingsAndCache.beginGroup(page);

        QStringList entriesUrls = m_SettingsAndCache.childGroups();
        Q_FOREACH(QString entryUrl, entriesUrls)
        {
            m_SettingsAndCache.beginGroup(entryUrl);

            IceEntry iceEntry;
            iceEntry.Title = m_SettingsAndCache.value("title").toString();
            iceEntry.Url = entryUrl;
            iceEntry.Year = m_SettingsAndCache.value("year").toInt();

            ++numTotalEntries;
            if(iceEntry.Year >= m_YearRange.first && iceEntry.Year <= m_YearRange.second)
            {
                iceEntryList.append(iceEntry);
            }

            m_SettingsAndCache.endGroup();
        }

        m_SettingsAndCache.endGroup();
    }

    if(iceEntryList.isEmpty())
    {
        emit e("failed to get any entries for that range in the ice entry list");
        emit finishedPickingIce(false);
        return;
    }
    qsrand(QDateTime::currentMSecsSinceEpoch());
    int randomEntry = qrand() % iceEntryList.size();
    IceEntry iceEntry = iceEntryList.at(randomEntry);
    emit v("there are " + QString::number(numTotalEntries) + " entries in total");
    emit v("there are " + QString::number(iceEntryList.size()) + " entries within that year range");
    emit finishedPickingIce(true, iceEntry);
}
void IcePick::pickIce(YearRange yearRange, bool forceRefreshCache)
{
    m_YearRange = yearRange;
    bool haveCache = m_SettingsAndCache.value("haveCache", false).toBool();
    if((!haveCache) || forceRefreshCache)
    {
        addAllIcePagesToCache();
        return;
    }
    //have cache, do main functionality
    buildFlatListWithinYearRangeAndThenGenerateRandomNumberToSelectEntry();
}
void IcePick::handleNetworkRequestFinished(QNetworkReply *networkReply)
{
    bool parseAndProcessSuccess = parseNetworkReply(networkReply);
    networkReply->deleteLater();
    if(!parseAndProcessSuccess)
        return;
    requestNextPageOrContinueOntoMainFunctionality();
}
