#include <iostream>
#include "AudioSystem.h"

using namespace std;

int16_t floatToInt16(float val) 
{
    int16_t newVal;
    if (val >= 1.0)
        newVal = 32767;
    else if (val <= -1.0)
        newVal = -32768;
    else
        newVal = (int16_t) floor(val * 32768.0);

    return newVal; 
}

AudioSystem::AudioSystem()
{
    // Attempt to open default audio device
    openALDevice = alcOpenDevice(nullptr);

    if (!openALDevice) {
        cerr << "Failed to open default audio device" << endl;
        exit(2);
    }

    // Create a context
    openALContext = alcCreateContext(openALDevice, nullptr);
    if (!openALContext) {
        cerr << "Failed to create audio context" << endl;
        exit(2);
    }

    // Make the newly created context current
    if (!alcMakeContextCurrent(openALContext)) {
        cerr << "Failed to make audio context current" << endl;
        exit(2);
    }
}

AudioSystem::~AudioSystem()
{
    // Delete sources and buffers
    alDeleteSources(audioSources.size(), audioSources.data());
    alDeleteBuffers(audioBuffers.size(), audioBuffers.data());
    
    alcMakeContextCurrent(nullptr);

    // Destroy context
    alcDestroyContext(openALContext);
    
    // Close audio device
    alcCloseDevice(openALDevice);
}

ALuint AudioSystem::loadFile(string path)
{
    // Load audio file
    cout << "\nLoading Audio File: " + path << endl;
    auto audioFile = AudioFile<float>();

    if (audioFile.load(path)) 
        cout << "Sucessfully loaded audio" << endl;
    else
        cerr << "Failed to load audio: " + path << endl;

    // audioFile.printSummary();

    // Set audio format
    ALenum format;
    int channels = audioFile.getNumChannels();
    int bitDepth = audioFile.getBitDepth();
    if (channels == 1 && bitDepth == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitDepth == 16)
        format = AL_FORMAT_STEREO16;
    else {
        cerr << "Unsupported format" << endl;
        return 0;
    }

    // Generate and setup audio buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);
    vector<int16_t> audioData;

    // Interleave data if necessary
    if (channels = 2) {
        for (int i = 0; i < audioFile.getNumSamplesPerChannel(); i++) {
            audioData.push_back(floatToInt16(audioFile.samples[0][i]));
            audioData.push_back(floatToInt16(audioFile.samples[1][i]));
        }
    }
    else {
        for (int i = 0; i < audioFile.getNumSamplesPerChannel(); i++) 
            audioData.push_back(floatToInt16(audioFile.samples[0][i]));
    }

    alBufferData(buffer, format, (ALvoid*) audioData.data(), 
        audioData.size()*sizeof(int16_t), audioFile.getSampleRate());

    
    audioBuffers.push_back(buffer);
    return buffer;
}

ALuint AudioSystem::createSource(float x, float y, float z)
{
    ALuint source;

    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 0.5f);
    alSource3f(source, AL_POSITION, x, y, z);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    audioSources.push_back(source);
    return source;
}

void AudioSystem::bind(ALuint source, ALuint buffer)
{
    alSourcei(source, AL_BUFFER, buffer);
}

void AudioSystem::play(ALuint source)
{
    alSourcePlay(source);
}