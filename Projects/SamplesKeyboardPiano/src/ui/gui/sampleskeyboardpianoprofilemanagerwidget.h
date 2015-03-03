#ifndef SAMPLESKEYBOARDPIANOPROFILEMANAGERWIDGET_H
#define SAMPLESKEYBOARDPIANOPROFILEMANAGERWIDGET_H

#include <QDialog>

#include "sampleskeyboardpianoprofile.h"

class QLineEdit;
class QComboBox;

class SamplesKeyboardPianoProfileManagerWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SamplesKeyboardPianoProfileManagerWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
    SamplesKeyboardPianoProfile selectedProfile() const;
private:
    QLineEdit *m_NewProfileNameLineEdit;
    QComboBox *m_ProfilesComboBox;
    QPushButton *m_OkButton;
public slots:
    void handleProfilesProbed(QList<SamplesKeyboardPianoProfile> profiles);
private slots:
    void handleAddProfileButtonClicked();
};

#endif // SAMPLESKEYBOARDPIANOPROFILEMANAGERWIDGET_H
