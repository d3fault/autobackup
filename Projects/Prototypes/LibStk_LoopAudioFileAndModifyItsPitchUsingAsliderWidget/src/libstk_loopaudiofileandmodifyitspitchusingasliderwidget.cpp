#include "libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h"

using namespace stk;

#include <QMessageBox>

#include "simplesinglefileloopsamplerinstrument.h"

//this class was a line-by-line copy-paste of libstk's "play.cpp" (By Gary P. Scavone, 2000 - 2004) example file (and later took some code from threebees.cpp), omitting as much as I can for readbility, and hopefully ultimately integrating with Qt's QSlider::positionChanged (or w/e the sig's called) to control a pitch rocker effect in libstk
//TODOreq: debug what thread is calling this::stkTick .. because if stk isn't using it's own backend thread then I really don't have ANY clue how it's even functioning O_o. once I know it's using it's own thread, I should maybe use QAtomicInt to safely comm between the GUI and Stk -- the int I'm talking about of course is the one passed to pitchBend/whatever
LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent)
    : QSlider(parent)
{
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate((StkFloat)44100);

    voicer.reset(new Voicer);
    dac.reset(new RtAudio); //delayed initialization in case Stk::setSampleRate needs to happen first

    samplerInstrument.reset(new SimpleSingleFileLoopSamplerInstrument);

    // Try to load the soundfile.
    try
    {
        samplerInstrument->openFile("/run/shm/audio.wav");
    }
    catch(StkError &stkError)
    {
        showStdStringError(stkError.getMessage());
        close();
        return;
    }

    // Set input read rate based on the default STK sample rate.
    double rate = 1.0;
    rate = samplerInstrument->input.getFileRate() / Stk::sampleRate();
    //rate *= 2.0; //uncomment to play back audio at twice the speed
    samplerInstrument->input.setRate(rate);

    //input->ignoreSampleRateChange();

    // Find out how many channels we have.
    int channels = samplerInstrument->input.channelsOut();

    // Figure out how many bytes in an StkFloat and setup the RtAudio stream.
    parameters.deviceId = dac->getDefaultOutputDevice();
    parameters.nChannels = channels;
    RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    try
    {
        dac->openStream(&parameters, NULL /*TODOreq: does nullptr evaluate to 0? would be stupid if it didn't*/, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::staticStkTick, this);
    }
    catch(RtAudioError &error)
    {
        showStdStringError(error.getMessage());
        close();
        return;
    }

    // Resize the StkFrames object appropriately.
    frames.resize(bufferFrames, channels);

    voicer->addInstrument(samplerInstrument.data()); //TODOduringIntegration: use second [optional] arg of addInstrument, the group-id, to differentiate between the 10 fingers
    //samplerInstrument->noteOn(samplerInstrument->input., 1);

    try
    {
        dac->startStream();
    }
    catch(RtAudioError &error)
    {
        showStdStringError(error.getMessage());
        close();
        return;
    }

    // Block waiting until callback signals done.
    //nope? will libstk play nicely with Qt's event loop? actually I kind of doubt it. THIS IS WHY WE PROTOTYPE, so I didn't fubar the Wasdf codebase. hmm mb I need LibStk stuff to be on a separate/backend thread and then I _DO_ sleep? fuck it let's just try with Qt's event loop first and see
    //while ( !done ) Stk::sleep( 100 );


    setRange(0, 1023);
    setValue((minimum() + maximum()) / 2);
    connect(this, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::valueChanged, this, &LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handleValueChanged);
}
int LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::staticStkTick LIBSTKTICK_METHOD_SIGNATURE
{
    LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget *instance = static_cast<LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget*>(userData);
    return instance->stkTick(outputBuffer, unusedInputBuffer, nBufferFrames, streamTime, status, userData); //no neeed to pass userData to the non-static version, but I want to keep using this tick method signature define out of laziness :)
}
// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::stkTick LIBSTKTICK_METHOD_SIGNATURE
{
    register StkFloat *outputBufferAsFloat = (StkFloat*)outputBuffer; //does register even do anything anymore xD? fuggit
    int numTicks = static_cast<int>(nBufferFrames);
    for(int i = 0; i < numTicks; ++i)
    {
          *outputBufferAsFloat++ = voicer->tick(); //copy from Voicer's output buffer thingo to our audio device's output buffer thingo
    }

#if 1
    return 0; //continue normal stream operation
#else
    if(input->isFinished())
    {
        //TODOreq: loop. actually fuck looping, I'll just use a long song file. the FileWvIn shit isn't even used in my Wasdf context
        //qFatal("TODOreq finish"); close(); //mb that's all? just close? mb just return 1 actually and let the... wait what
        close();
        //done = true;
        return 1; //stop the stream and drain the output buffer. maybe I want to return 2 because the call to close(); might get shit closing fast and the stk internals might not have enough time to "drain the output buffer" and stop properly? I'd need to set breakpoints and try it (but it could still be luck/undefined-but-consistent-on-one-machine, so idk)
    }
    else
        return 0; //continue normal stream operation
#endif
}
LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::~LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget()
{
    //the stream is automatically stopped, but we should still close it
    if(dac->isStreamOpen())
    {
        try
        {
            dac->closeStream();
        }
        catch(RtAudioError &error)
        {
            error.printMessage();
        }
    }
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::showStdStringError(const std::string &stkError)
{
    QString stkErrorQString = QString::fromStdString(stkError);
    QMessageBox::critical(this, "StkError:", stkErrorQString);
}
void LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::handleValueChanged(int newValue)
{
    //wtf do I call pitchbend/pitchchange/whatever on?? I need a Voicer I think, like demo.cpp uses. I _think_ (unsure tbh) I need to make an Instrmnt inheritting class that hasA FileWvIn, and I have to manually implement setFrequency (I think Voicer gives me pitchBend free if I provide setFrequency?). I need to inherit from Instrmnt so I can add the instrument to the voicer! I think I can look at other Instrmnt inheritting classes to study how they use setFrequency... in fact the sine wave instrmnt might even have that code perfectly copy/pastable! NVM finally found the class I was looking for (inherits from Instrmnt already :-D): Sampler. I woulda thought there was already this functionality in a fucking synthesis toolkit, but thought maybe I was hitting a corner case [not yet implemented]. Blah now I can't figure out how to USE Sampler class xD, where the fuck do I specify the file path?? oh it's abstract. wtf is a Moog? meh back to orig idea of inheritting Instrmnt (or mb Sampler now, but that actually looks bloated!! mb it isn't idfk)
    //input->setFrequency(69); //eh I don't think this does what it looks like it does. it's not pitch bend :(, it's "loop frequency" xD. I think I gotta go the inherit Instrmnt route... but still unsure wtf <--- thinking about it HARDER, reading the file "faster" (and playing back at same speed) would increase the SOUND frequency exactly like I want! xD maths > me. so I think just need to create an Instrmnt (based loosely on Sampler) and then my Instrmnt's setFrequency just calls FileLoop::setFrequency and bam I'm maybe done if hopefully Voicer can GIVE me pitchBend/whatever for FREE >_> *crosses fingers*. the threebees.cpp example that comes with libstk uses the Voicer class :D (so does demo.cpp, but that shit is massive). OT'ish: I don't understand why [nearly] EVERYTHING inherits from stk::Stk. wtf does that base class provide that's so useful to EVERY class? I bet if I understood the lib better it'd make sense, but my instincts tell me that their class heirarchy isn't laid (sex) out correctly: maybe a FEW classes should inherit Stk, but [nearly] ALL of them? wtf? "an stk object asks an stk object for synth'd audio bytes" -- uh, what?. FUGGIT [/endrant]. WOOT just read Voicer::pitchBend and my guess was [hopefully] correct: it DOES rely on setFrequency :-D
    voicer->pitchBend(69); //TODOreq: map 0-1023 of slider range to... uhh... whatever pitchBend's range is? mb 0.0 <--> 1.0 , idfk?
}
