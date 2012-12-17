#ifndef ENDRESULTTARGETTEST_H
#define ENDRESULTTARGETTEST_H

#include <QObject>

//i need a flag that my moc-hack can see and know to generate the thread helper
//the flag SHOULD IDEALLY only need to mention the class
//but i am too much of a noob so i think i'm going to need to mention it's constructor + args manually as like a string that the user types (or copies :-P). there is also the filepath to deal with TODOreq, but i can deal with that after getting constructor args working

#define Q_DECLARE_OBJECT_FOR_THREAD(asdf) ; //hack to make it compile? the generator will have stripped it from the pre-moc code by now (though the IDE would still underline it and bitch about it (can fix this))

Q_DECLARE_OBJECT_FOR_THREAD(EndResultTargetBusiness(int simpleArg1, QString implicitlySharedQString, QObject *parent = 0))
//^^must be only on one line, because that's how it easily/hackily finds the last parenthesis. The generator/parser replaces this line with the include that's below, making the above empty #define not necessary

//TODOreq: found my point of entry for the MOC hack: /moc/preprocessor.cpp Preprocessor::preprocess, right at the top :). I _THINK_ it's true that I should be looking for my Q_DECL* in every file that is preprocessed. It does not hurt me to search in files that I wouldn't normally search in (.pro, .idfk), it just wastes a little bit of time. And I'm 99.9% sure that every file that would go through the preprocessor are the files that might potentially contain the Q_DECL*
//I also think I need to do my changes before the preprocessor gets there. SHIT FOUND AN ERROR: my generated file "thread helper" won't be preprocessed :-/ GAH... and I don't know that it's safe to just add it to that "currentFiles" array...


//I want a file that is generated AND preprocessed (since it has Q_OBJECT in it). But I need to hook into the "list of preprocess files" and modify it (append to it) as well! Wtfz.

//^^solution: in between:
//else
//AND
//files = Option::mkfile::project_files;
//in: /qmake/main.cpp
//append the results of your MANUAL PARSE OF "project_files" (QStringList) _TO_ project_files just before the "files = " assignment above. I am 99% sure that will work...


//I definitely underestimated the difficulty of this. I want to GENERATE a file that is not seen by user (like "moc_" files), but then that generated file needs to have it's own generated file (the "moc_" files themselves!)... so it's hard to figure out where in qmake/moc the hack should go. If I put it in as a pre-qmake item, I could parse the .pro file myself for the SOURCES / HEADERS variables for the m_ListOfFilesToSearchForDECL*MacroIn ....... BUT that would mean i'd be modifying the .pro file before passing it into qmake (adding my generated file (then let qmake/moc run normally to generate the "moc_" for it/them)). Maybe this is OK if I forward all the arguments to qmake properly and then introduce the concept (HHHAAAACCKKKK) of a temporary .pro file (which goes in the build folder [that only QMake knows the destination of (unless I introduce more hacks to figure that out (in the .user?)]. Fuck.
//I think it's possible to parse the $PRO_FILENAME_INPUT + ".user" to get the currently active target (usually just 1), then get it's build configuration (ProjectExplorer.Target.ActiveBuildConfiguration) and then to find the output folder based on that.... but I think this might introduce circular dependency errors because I'm going to be modifying the destination and contents of the .pro file and I think qmake uses both of those but now that our .pro file is in the output folder it will create yet another sub-dir output folder???? IDFK TBH gah this hack blows I should just use something else so I can get back to Rpc shit


//I am unsure if I need to #include any constructor arg TYPES that are "custom" (or Qt-implicit-etc that require including). I think that I am able to type it in the macro means that it's already included... so not putting it in the ObjectOnThreadHelper class might not be a biggy (though I am unsure of this :-/)

//TODOreqERROR: if you want to use the same object on thread helper that is made from the macro, you include it wherever you want to use it and then put the Q_DECL* macro in the nearest parent. Except that nearest parent might not know WHAT the constructor args are (and so now has to include them! bah humbug! I guess it isn't ALLLL that bad... he also has to know the business object itself! So wouldn't it get included when including the business object, which happens before the macro? This might not be an error idfk

//this include is generated (ANSWER: YEP)? or i guess it shouldn't be so that we can "DECLARE/FLAG" once and then use the qthread helper all around the project by simply including it (this gives us multiple thread-objects (the object on threads))
#include "endresulttargetbusinessonthreadhelper.h"

class EndResultTargetTest : public QObject
{
    Q_OBJECT
public:
    explicit EndResultTargetTest(int simpleArg1, QString implicitlySharedQString, QObject *parent = 0);
private:
    EndResultTargetBusinessOnThreadHelper m_EndResultTargetBusiness;
signals:
    
public slots:
    void handleBusinessInstantiated();
};

#endif // ENDRESULTTARGETTEST_H
