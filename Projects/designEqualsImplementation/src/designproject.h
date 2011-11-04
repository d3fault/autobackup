#ifndef DESIGNPROJECT_H
#define DESIGNPROJECT_H

#include <QObject>

class DesignProject : public QObject
{
    Q_OBJECT
public:
    explicit DesignProject(QString projectName);
    QString getProjectName();
private:
    QString m_ProjectName;
signals:

public slots:

};

#endif // DESIGNPROJECT_H
