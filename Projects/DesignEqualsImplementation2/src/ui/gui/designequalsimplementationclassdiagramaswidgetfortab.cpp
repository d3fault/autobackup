#include "designequalsimplementationclassdiagramaswidgetfortab.h"

#include "classdiagramgraphicsscene.h"

//TODOreq: at the time of writing, the only two uml items I can think of for class diagrams are "interfaces" and "classes", heh. Fucking UML spec is so bloated. Oh actually after looking at Dia just now, I think "notes" are pretty handy too (but I want to have them more "attached" to various objects instead of just eh floating)... but notes are hardly UML xD. OT: weird in Dia there are two "implements a specific interface" lines, and idfk the difference...
//TODOoptional: "search", which highlights uml items if they start with what the user types in
DesignEqualsImplementationClassDiagramAsWidgetForTab::DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : IHaveAGraphicsViewAndScene(new ClassDiagramGraphicsScene(designEqualsImplementationProject), parent)
{ }
