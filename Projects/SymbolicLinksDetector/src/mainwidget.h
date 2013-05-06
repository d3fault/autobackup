#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QPlainTextEdit>

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();
private:
    QVBoxLayout *m_Layout;

    QLabel *m_DirToDetectSymbolicLinksInLabel;
    QLineEdit *m_DirToDetectSymbolicLinksInLineEdit;
    QPushButton *m_DirToDetectSymbolicLinksInBrowseButton;

    QLabel *m_WhatToDoWhenSymbolicLinkDetectedLabel;
    QRadioButton *m_QuitOnFirstDetectRadioButton;
    QRadioButton *m_ListAllPathsRadioButton;

    QPushButton *m_StartDetectingSymbolicLinksButton;

    QPlainTextEdit *m_Debug;
signals:
    void startDetectingSymbolicLinksRequested(const QString &dir, bool quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE);
private slots:
    void handleD(const QString &msg);
    void handleDirToDetectSymbolicLinksInBrowseButtonClicked();
    void handleStartDetectingSymbolicLinksButtonClicked();
};

#endif // MAINWIDGET_H
