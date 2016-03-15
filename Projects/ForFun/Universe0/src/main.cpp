#include <QCoreApplication>
#include <QSet>

struct Position3D
{
    qint64 X, Y, Z;
};
struct VectorUnit
{
    Position3D Position;
    //etc (idfk. need to take the physics/chemistry class first)
};
class /*Laws Of*/ Physics
{
public:
    /*Laws Of*/ Physics()
        : m_TickNum(0)
    { }
    void tick()
    {
        ++m_TickNum;
        Q_FOREACH(VectorUnit &vectorUnit, m_AllVectorUnits) //or maybe a pointer instead of reference, so we modify the in-set instance instead of making a copy to modify
        {
            applyLaws/*OfPhysics*/(vectorUnit);
        }
    }
    void applyLaws/*OfPhysics*/(VectorUni &vectorUnit)
    {
        //TODOreq: maths. take a physics/chemistry class to learn the maths. it's something along the lines of "what goes up, must come down" ;-)
    }
private:
    quint64 m_TickNum;
    QSet<VectorUnit> m_AllVectorUnits;
};
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    /*Laws Of*/ Physics /*lawsOf*/physics;
    while(true)
    {
        /*lawsOf*/physics.tick(); //or step(), or progressOneVectorFrameForwards()
    }
    return 0;
}

/*
So anyways the main physics.tick() part was the first thought and it got me thinking about how we're just vector units that have assimilated into conscious groups of vector units. There are enough vector units (and of course enough time) that they eventually/always become conscious/self-aware. Our interactions with one another are just groups of vector units communicating (interestingly enough, USING vector units) to other groups of vector units. It made me wonder how we could possibly "communicate with the programmer". Well I didn't get very far with that thought but I did stumble upon the thought that we could change the true in the while(true) to false so that the loop breaks out. My rationale is that  "we share the same memory/hardware as that boolean". This doesn't help in communicating with the programmer and is also very probably death xD. I do think it would be possible for the programmer to contact/interact-with us, however. Anyways I of course asked myself for the millionth time "what's it all about?" and for the hundred-thousandth time answered "nothing!". But yea aside from nothing I think love is the best CHOICE. It got me thinking about my grandpa and I decided I want to spend more time with him. Not knowing what to do with him, I did what any english-speaking human would do and I went down the alphabet: A. One of the first things I thought of was a restaurant with the word apple in it. Yea apples are cliche/obvious Mr. Me Jobs. I didn't want it to revolve around food though, and I wanted something that would last longer and be more interesting. I kept with the letter A, because I wasn't in a rush or anything (just sittin out here in the desert). Airplane! Hmm, airplane museum? Meh, we've been there done that, neither special nor long lasting. I kept with airplane though, it's something interesting that we both... err... are interested in? Well with glaucoma he's no longer allowed to fly :(, but I do recall him making various remote controlled airplanes out of fucking WOOD xD and I do recall even saying myself a few years back that it's something I'd want to get into. So that's what I want to fucking do dammnit. Arbitrary stupidity of us being merely groups of vector units aside, love seems real, I love him, and that's a damn good use of my time if you ask me. It's genuinely interesting and additionally gives us something to bond around/over. A lot of times when we hang out, we don't say/do much and we fill the silence with small talk... the same bullshit over and over. And of course let's not forget that maybe it would somehow influence his answer to my asking him again for the hundredth time if he'd sign up to be cryonically frozen when he "dies" of "natural causes" (probably parksinsons at this point). I would want to build the remote control to the RC airplane myself. I'd want to put a video camera on the front and  possibly to fly/control it "in the first person" wearing hat+wasdf. We'd combine our collective intelligence to make some badass fucking toy plane. Sure I could probably figure it out on my own/using-the-internet, but that's not the point! Well anyways here's the kicker to this whole thing: I realized that I'd need/want to record the creation of this whole thing, should between us any patent-worthy ideas emerge (so I/we can defensively publish them, ofc!)... but also just as a tutorial for others to follow. Since I can't [afford to] record myself, I can't/won't do the idea at all (until). That the world is filled with sociopaths who steal ideas, and that it's actually completely legal and there's even an incentive to steal ideas..... is totally putting a hamper on my life[style]. I am filled with rage. [...].
*/
