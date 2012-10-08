#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QList>
#include <QHash>
#include <QPair>
#include <QFileDialog>

class mainWidget : public QWidget
{
    Q_OBJECT
    
public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();
private:
    QVBoxLayout *m_Layout;
    QPushButton *m_AddNewRowBtn;
    QPushButton *m_PersistTcRows;

    //int m_TcRowsCount;
    //QMap<int, QPair<QLineEdit*, QLineEdit*> > m_TcRowsMap;
    QList<QPair<QLineEdit*, QLineEdit*> > m_TcRowsList;
    QHash<QPushButton*, QLineEdit*> m_BrowseButtonLineEditAssociations;
private slots:
    QHBoxLayout *createNewTcFileAndPwFileRow(QString containerLocation = QString(), QString containerPasswordLocation = QString());
    void handleAddNewRowBtnClicked();
    void handlePersistTcRowsBtnClicked();
    void handleBrowseButtonClickedAndFigureOutWhich();
};

#endif // MAINWIDGET_H
