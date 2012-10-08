#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>

#include "easytree.h"

class mainWidget : public QWidget
{
    Q_OBJECT
public:
    mainWidget(QWidget *parent = 0);
    void setBusiness(EasyTree *easyTreeBusiness);
    ~mainWidget();
private:
    EasyTree *m_EasyTreeBusiness;
    inline void connectToBusiness();

    //gui
    QVBoxLayout *m_Layout;
    QLineEdit *m_DirToTreeLe;
    QPushButton *m_BrowseButton;
    QPushButton *m_ClearAndThenTreeButton;
    QPlainTextEdit *m_Debug;
signals:
    void treeTextGenerationRequested(const QString &dirString, QList<QString> *dirNamesToIgnore, QList<QString> *fileNamesToIgnore, QList<QString> *fileNamesEndWithIgnoreList, QList<QString> *dirNamesEndsWithIgnoreList);
private slots:
    void handleD(const QString &msg);
    void handleBrowseButtonClicked();
    void handleClearAndThenTreeButtonClicked();
};

#endif // MAINWIDGET_H
