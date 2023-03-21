#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <memory>
#include <string>
#include <AL/al.h>
#include <AL/alc.h>
#include <AudioFile.h>

using namespace std;

class AudioSystem 
{
    public:
        ALCdevice* openALDevice;
        ALCcontext* openALContext;

        vector<ALuint> audioBuffers;
        vector<ALuint> audioSources;

        AudioSystem();
        ~AudioSystem();
        ALuint loadFile(string path);
        ALuint createSource(float x, float y, float z);
        void bind(ALuint source, ALuint buffer);
        void play(ALuint source);
};

#endif