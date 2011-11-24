#ifndef DESIGNPROJECTTEMPLATES_H
#define DESIGNPROJECTTEMPLATES_H

#include <QObject>
#include <QMultiMap>

//forward declaration
class DiagramSceneNode;

//TODOreq: make this a singleton. accessed/instantiated by "Instance"... and either implement copy-on-write for the templates or allow them to be copied when read
class DesignProjectTemplates : QObject
{
    Q_OBJECT
public:
    enum DesignProjectViewType { UseCaseViewType, ClassDiagramViewType };
    static DesignProjectTemplates* Instance();
    void populateDesignProjectViewsAndTheirNodes();
    QMultiMap<DesignProjectViewType, DiagramSceneNode*> *getAllDesignProjectNodesByProjectViewType();
private:
    DesignProjectTemplates(); //private so that it cannot be called
    DesignProjectTemplates(DesignProjectTemplates const&){} //copy constructor is private
    DesignProjectTemplates& operator=(DesignProjectTemplates const&){} //assignment operator is private
    static DesignProjectTemplates *m_pInstance;


    //QList<DesignProjectView*> *m_AllDesignProjectViews; //"ever" (can be added pre-compile). needs to be static/const/enum/something i can't figure out and don't care to
    //QList<DesignProjectViewType*> *m_AllDesignProjectViewsByProjectViewType;
    QMultiMap<DesignProjectViewType, DiagramSceneNode*> *m_AllDesignProjectNodesByProjectViewType;
    //OLDODOopt: should this be in a DesignProjectTemplate class? each type can/will/should(?) have it's own helper function. "createUseCaseBackendNode" (which just returns a pointer to our template (should be copy on write obv, but for now just copy on read. the OUTPUT code needs copy-on-read more than the first bootstrapping instance itself (what i'm writing right now))

signals:
    //void onViewTypePopulated(DesignProjectViewType);
    //void onDesignProjectNodeAdded(DesignProjectViewType, DiagramSceneNode*);
    void onTemplatesPopulated();
};

#endif // DESIGNPROJECTTEMPLATES_H
