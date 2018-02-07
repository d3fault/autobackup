#ifndef TOP5MOVIESLISTWIDGET_H
#define TOP5MOVIESLISTWIDGET_H

#include <QWidget>

#include <QVariantList>

class QVBoxLayout;
class QLineEdit;

class Top5MoviesListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Top5MoviesListWidget(QWidget *parent = 0);
    //QVariantList variantList() const;
    QStringList top5Movies() const;
private:
    void addNewRow();

    QVBoxLayout *m_MyLayout;

    //QVariantList m_VariantList;
    //QStringList m_Top5Movies;
    QList<QLineEdit*> m_Top5MoviesLineEdits;
private slots:
    void handledAddNewRowButtonClicked();
};

#endif // TOP5MOVIESLISTWIDGET_H
