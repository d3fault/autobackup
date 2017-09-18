#include "libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h"

using namespace stk;

#include <QMessageBox>

//this class is a line-by-line copy-paste of libstk's "play.cpp" (By Gary P. Scavone, 2000 - 2004) example file, omitting as much as I can for readbility, and hopefully ultimately integrating with Qt's QSlider::positionChanged (or w/e the sig's called) to control a pitch rocker effect in libstk
LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget::LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget(QWidget *parent)
    : QSlider(parent)
{
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate((StkFloat)44100);

    // Initialize our WvIn and RtAudio pointers.
    dac.reset(new RtAudio); //delayed initialization in case Stk::setSampleRate needs to happen first
    input.reset(new stk::FileWvIn);

    // Try to load the soundfile.
    try
    {
        input->openFile("/run/shm/audio.wav");
    }
    catch(StkError &stkError)
    {
        showStdStringError(stkError.getMessage());
        close();
        return;
    }

    // Set input read rate based on the default STK sample rate.
    double rate = 1.0;
    rate = input->getFileRate() / Stk::sampleRate();
    //rate *= 2.0; //uncomment to play back audio at twice the speed
    input->setRate(rate);

    input->ignoreSampleRateChange();

    // Find out how many channels we have.
    int channels = input->channelsOut();

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
    input->tick(frames);

    for(unsigned int i=0; i<frames.size(); ++i)
        *outputBufferAsFloat++ = frames[i]; //copy from input buffer to output buffer

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
    //wtf do I call pitchbend/pitchchange/whatever on?? I need an Stk object (like Voicer in demo.cpp), wtf play.cpp doesn't use one O_o
}
