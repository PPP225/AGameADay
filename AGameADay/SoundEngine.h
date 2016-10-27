#pragma once

#include <irrKlang/irrKlang.h>
#include <string>

class SoundEngine
{
public:
	SoundEngine() = default;
	~SoundEngine() = default;
	void Init();
	void Delete();

	irrklang::ISound* PlaySound(const char* path, bool loop = false);
	void StopSound(const char* path);
	void StopAllSounds();

	void VolumeUp();
	void VolumeDown();

	// "sound" reference returned by Play2D. 
	// "value" between 0.0f and 1.0f.
	void SetSoundVolume(irrklang::ISound* sound, float volume);

private:
	irrklang::ISoundEngine* Engine = nullptr;
};
