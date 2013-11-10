#include <QtCore/QCoreApplication>
#include <QFile>

#include "lastmodifieddateheirarchymolestercli.h"

//This is fun because I don't think I've EVER written a qt console application, or the times that I did I used iostream instead of the (WTF I INCLUDED _NOTHING_ stdin/stdout wrapped in a QTextStream (sex)).
//I'm going to over analyze and perfect this design, because I've ranted many times about how the perfect operating system "shares a lib", "has a cli and a gui" that are "auto generated" (I'm going to hand code them as if they were auto generated, prototyping in mah brain motha fucka). This application is simple as fuck so yea.
//Also in trying to determine if I need threads (my hypothesis atm follows): CLI blocks on the "lib" method calls, GUIs always use a backend thread. I'm still iffy on that, but I _THINK_ that the only reason you'd want to use a backend thread in a CLI application is if you had an [interactive] "menu" system that could be opened/closed without the backend... err... process/daemon (so i guess even in this case it wouldn't be on a different thread). Ok the cases where the backend takes a REALLy long time (aww fuck, is that these cases? Heirarchy molester does take a bit of time... so does it warrant a backend thread? I would say that depends entirely on whether or not there's a "timeout... want to kill process X?" system functionality like there is for GUI (and rightly so). I genuinely don't know :). /nub. I'm confident that if I was "encoding a video" on the CLI, that I should put that on a backend thread and push "status updates" to the main (had:gui) thread. Hmm so is the "timeout not responding, want to kill" timeout an X11 functionality? Bah. i r teh nub. I want to say "to be on the safe side just put everything on backend thread lol", but that's just stupid/needless/wasteful (one could even argue that my "always use backend thread with GUIs is wasteful", but I care significantly less about that waste). CLIs are more likely to be in "server" environments, so although it doesn't matter for this app in particular (LOL KISS MOTHA FUCKA), I still want to know. "I just gotta know" ... "gotta hack em all"
//Still I _COULD_ do that "interactive menu controlling backend thread" design with ease... and sure definitely would when I deem it necessary (this, however, is a scriptable/chainable app... so a different use case altogether

//TODOreq: auto gen'd gui, auto gen'd cli in "script/chainable" mode, auto-gen'd cli in "interactive" mode. basically go into interactive mode when "not enough" arguments are supplied (if some are provided, we can 'skip'/jump to varioius points in interactive mode AND/OR have certain user inputs pre-filled in (should they still be editable if supplied and used to pre-fill-in?)). all 3 auto gen'd from design of course :)

//What is the "proper" way to do CLI (lol such a basic ass question... but I've been in love with Qt's GUI shit so I kinda don't give a fuck about CLI (even though I very much do (chainability)). I mean since I have no finished products, all/most of my prototypes use GUIs lol because yea (less chance of typos, less effort overall, etc -- i love my "browse" dialogs wew (TODOreq: when using a gui with a respective cli counter-part (god knowledge to determine this), show the command line that "would have been run". so in this example app it would be what's in the two line edits and the checkbox state just before pressing "molest"))

//I'm kinda just taking a night of fun coding I guess you could say... but still this IS research for osios/designEquals type stuff. WORK = FUN, but I am admittedly taking a tangent and !KISS

//My instinct/impulse is to say "no, I don't need a backend thread. It is ok to block indefinitely in a console application"... because I see that shit happening all the time. BUT I don't "know" that such cases aren't merely waiting on a backend thread to "report in" and don't just have the main thread idle/processing-events until then (which could APPEAR as if it was blocking)

//I can visualize in my brain what's happening if I stay on main thread vs. use backend thread, and I can even code/design both scenarios... but I can't determine which is "best" for the typical use case :(. Even "search engine"-ing isn't helping :(

//Just noting for reference, I have all the ifdef'd code in this main.cpp written (was just testing at that point), and right now my int main is empty and no classes have been created (though I know they will come)

//Thx Jim Loco I was about to conclude indecisively that your "The Correct Method..." was what I was going to use because: KISS and 'IDGAF, can always thread it later (assuming I used events like he suggested (and I was planning on it))'. Seeing his thread and just seeing that he seems to have a shred of intelligence over the other posts I was seeing (just like the ones he complains about in the same article :-P), and the fact that he didn't use a backend thread, gave me "some" confidence. "Some confidence" + idgaf = choice. Code coming now, trying to mimic how I do GUI (but will refactor the way I do that in a heartbeat if it suits my needs (and will memorize new method and use it from now on (and presumably in osios ofc))).

//Inb4 I should be using submodules instead of pri relative path includes :-/...

#if 0

#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QTextStream blah(stdin);
    QTextStream blah2(stdout);
    QString line;
    do
    {
        line = blah.readLine();
        blah2 << line << endl;
    }while(!line.isEmpty());

    for(int i = 0; i < argc; ++i)
    {
        blah2 << "arg #" << i << ": " << argv[i] << endl;
    }

    blah2 << "quitting" << endl;
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);

    return a.exec();
}
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(QFile::exists("/bin/touch"))
    {
        LastModifiedDateHeirarchyMolesterCli cli;
        QMetaObject::invokeMethod(&cli, "parseArgsAndThenMolestLastModifiedDateHeirarchy", Qt::QueuedConnection);
        return a.exec();
    }
    qWarning("Couldn't find touch executable, quitting");
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
    return a.exec();
}
