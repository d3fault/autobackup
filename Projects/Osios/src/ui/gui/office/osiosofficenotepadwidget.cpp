#include "osiosofficenotepadwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QPlainTextEdit>

//TODOreq: list of meta "tags". should be stored seperately though still "next to" the document itself (they can be serialized together for persistence, but we should not make the user use # signs or ANY kind of embedded/inline tagging system. the deserialization needs to be able to split them again). Those same tags apply to ANY document type, not just notepad types
//TODOreq: a title is an index into the tag list (one of them), and it is optional to specify (a system where the first tag is the title by default would be pleasant, but idk if opt in or opt out)
//TODOoptional: tangentify variant/setting/mode/whatever, perhaps a different app that "Write" can also default to just like libreoffice might use
OsiosOfficeNotepadWidget::OsiosOfficeNotepadWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *toolsRow = new QHBoxLayout();
    QPushButton *newButton = new QPushButton(tr("New"));
    QPushButton *closeButton = new QPushButton(tr("Close (Auto-Saved)")); //TODOreq: every piece of user input is recorded (replay mode perhaps as part of timeline view), but this save and quit just gives them confidence in closing the document and it also just "hides"/"archives" it (additionally, gets the doc out of memory and session(session = if message was left open at app close, it is open again on app re-open. could even be the most recent tab on closing but that's a little irrelevant). the already-replayed version is what they see if they open it back up in this view (TODOoptional: "jump to timeline view for this doc" button)
    QPushButton *sendToContactButton = new QPushButton(tr("Send to Contact")); //TODOreq: for all kinds of files
    QPushButton *switchToOfficeSuiteButton = new QPushButton(tr("Switch to Office Productivity Suite")); //TODOreq: just some qt wysiwyg editor for now... maybe just a QTextEdit (not plain)... idk (or care much). TODOoptional: perhaps said office productivity suite is itself a tabwidget with a doc/spreadsheet/presentation/etc sub-tabs. Cool, QTextEdit exports to odt/html/pdf (the example is bitchin), BUT i'm not sure whether or not I can integrate with the actual actions going on to a level i desire for use with timeline view. If only undo/redo stuff was serializeable :-/, then I'd barely even have to try..
    QFont officeButtonFont = switchToOfficeSuiteButton->font();
    officeButtonFont.setBold(true);
    switchToOfficeSuiteButton->setFont(officeButtonFont);

    QTabWidget *osiosOfficeNotepadTabWidget = new QTabWidget();
    QPlainTextEdit *emptyDoc = new QPlainTextEdit(); //TODOreq: on first run, or if they close all their currently opened docs, an empty doc is presented to them
    osiosOfficeNotepadTabWidget->addTab(emptyDoc, "1"); //TODOoptional: come up with better thing to use than numbered

    toolsRow->addWidget(newButton);
    toolsRow->addWidget(closeButton);
    toolsRow->addWidget(sendToContactButton);
    toolsRow->addWidget(switchToOfficeSuiteButton);

    myLayout->addLayout(toolsRow);
    myLayout->addWidget(osiosOfficeNotepadTabWidget, 1);

    setLayout(myLayout);
}
