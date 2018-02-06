#ifndef QTWIDGETSUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLEWIDGET_H
#define QTWIDGETSUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLEWIDGET_H

#include <QWidget>

#include "../../uishared/firstnamelastnameqobjectcommandlineoptionparser.h"

class QLineEdit;
//class QPlainTextEdit;

class QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget : public QWidget
{
    Q_OBJECT
public:
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget(QWidget *parent = 0);
    bool parseArgs();
private:
    FirstNameLastNameQObjectCommandLineOptionParser m_ArgParser;

    QLineEdit *m_FirstNameLineEdit;
    QLineEdit *m_LastNameLineEdit;
    //QPlainTextEdit *m_Top5MoviesPlainTextEdit;
signals:
    void finishedCollectingUiVariables(const QString &firstName, const QString &lastName, const QStringList &top5Movies);
private slots:
    void handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess();
    void handleCancelButtonClicked();
};

#endif // QTWIDGETSUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLEWIDGET_H
