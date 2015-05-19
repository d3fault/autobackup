#ifndef ICEPICK_H
#define ICEPICK_H

#include <QObject>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QListIterator>
#include <QUrl>
#include <QScopedPointer>

#define ICEFILMS_TOPSITE_URL "http://www.icefilms.info"
#define ICEFILMS_MOVIES_URL_PREFIX ICEFILMS_TOPSITE_URL "/movies/a-z/"
#define ICEFILMS_ENTRY_BASE_PATH "/ip.php?v="
#define ICEFILMS_ENTRY_URL(v) ICEFILMS_TOPSITE_URL ICEFILMS_ENTRY_BASE_PATH + v + "&"

class IceEntry
{
public:
    IceEntry()
        : Year(0)
    { }
    QString Title;
    int Year;
    QString Url;
};
Q_DECLARE_METATYPE(IceEntry)

typedef QPair<int /*min year*/, int /*max year*/> YearRange;

class IcePick : public QObject
{
    Q_OBJECT
public:
    explicit IcePick(QObject *parent = 0);
private:
    YearRange m_YearRange;
    QSettings m_SettingsAndCache;
    QList<QChar> m_AllIcefilmPages;
    QNetworkAccessManager *m_NetworkAccessManager;
    QScopedPointer<QListIterator<QChar> > m_PagesIterator;

    void addAllIcePagesToCache();
    void requestNextPageOrContinueOntoMainFunctionality();
    void requestIcePage(QChar pageToRequest);
    bool parseNetworkReply(QNetworkReply *networkReply);
    bool parseTitleAndDate(const QStringRef &titleAndDateText, IceEntry& iceEntry);
    void buildFlatListWithinYearRangeAndThenGenerateRandomNumberToSelectEntry();
signals:
    void e(const QString &msg);
    void v(const QString &msg);
    void finishedPickingIce(bool success, IceEntry iceEntry = IceEntry());
public slots:
    void pickIce(YearRange yearRange, bool forceRefreshCache = false);
private slots:
    void handleNetworkRequestFinished(QNetworkReply *networkReply);
};

#endif // ICEPICK_H
