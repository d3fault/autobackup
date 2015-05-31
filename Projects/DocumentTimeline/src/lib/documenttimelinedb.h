#ifndef DOCUMENTTIMELINEDB_H
#define DOCUMENTTIMELINEDB_H

#include <QObject>

class QSettings;

class DocumentTimelineDb : public QObject
{
    Q_OBJECT
public:
    DocumentTimelineDb(QObject *parent = 0);
private:
    QSettings *m_Settings;
signals:
    void addDocToDbFinished(bool dbError, bool addDocToDbSuccess, void *userData);
public slots:
    void addDocToDb(QByteArray docToAddToDb, void *userData);
};

#endif // DOCUMENTTIMELINEDB_H
