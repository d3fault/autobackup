#ifndef BRAIN_H
#define BRAIN_H

#include <QObject>
#include <QMutableListIterator>

#define THE_CLAIM "Nothing Is Known"
#define THE_OPPOSITE_CLAIM "Something Is Known"

class Brain : public QObject
{
    Q_OBJECT
public:
    explicit Brain(QObject *parent = 0);    
private:
    QList<QString> m_ThingsKnown;
    bool isTrue(QString claim);
signals:
    void reportWhetherOrNotNothingIsKnown(bool nothingIsKnown);
    void reEvaluateAllThingsKnownRequested();
public slots:
    void determineWhetherOrNotNothingIsKnown();
private slots:
    void addReportResultsToListOfThingsKnown(bool nothingIsKnown);
    void reEvaluateAllThingsKnown();
};

#endif // BRAIN_H
