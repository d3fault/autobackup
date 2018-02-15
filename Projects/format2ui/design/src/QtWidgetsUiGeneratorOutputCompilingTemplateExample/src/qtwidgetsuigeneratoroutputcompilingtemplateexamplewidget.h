#ifndef QTWIDGETSUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLEWIDGET_H
#define QTWIDGETSUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLEWIDGET_H

#include <QWidget>

#include "../../uishared/firstnamelastnameqobjectcommandlineoptionparser.h"

#include "somewidgetlist.h"

/*format2ui-compiling-template-example_BEGIN_dsfsflsjdflkjowe0834082934*/
class QLineEdit;
class QBoxLayout;
class Top5MoviesListWidget;
/*format2ui-compiling-template-example_END_dsfsflsjdflkjowe0834082934*/

class QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget : public QWidget
{
    Q_OBJECT
public:
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget(QWidget *parent = 0);
    bool parseArgs();
    static void addLineEditToLayout(const QString &humanReadable, QLineEdit *lineEdit, QBoxLayout *theLayout);
private:
    FirstNameLastNameQObjectCommandLineOptionParser m_ArgParser;

    /*format2ui-compiling-template-example_BEGIN_lskjdfouewr08084097342098*/
    QLineEdit *m_FirstNameLineEdit;
    QLineEdit *m_LastNameLineEdit;
    Top5MoviesListWidget *m_Top5MoviesListWidget;
    QLineEdit *m_FavoriteDinnerLineEdit;
    QLineEdit *m_FavoriteLunchLineEdit;
    SomeWidgetList *m_SomeWidgetList;
    /*format2ui-compiling-template-example_END_lskjdfouewr08084097342098*/
signals:
    void finishedCollectingUiVariables(/*format2ui-compiling-template-example_BEGIN_sdlfkjsdklfjoure978234978234*/const QString &firstName, const QString &lastName, const QStringList &top5Movies, const QString &favoriteDinner, const QString &favoriteLunch, const QList<SomeWidgetListEntryType> &someWidgetListValues/*format2ui-compiling-template-example_END_sdlfkjsdklfjoure978234978234*/);
private slots:
    void handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess();
    void handleCancelButtonClicked();
};

#endif // QTWIDGETSUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLEWIDGET_H
