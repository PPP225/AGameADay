#include "SoundEngine.h"
#include <iostream>

void SoundEngine::Init()
{
	this->Engine = irrklang::createIrrKlangDevice();

	if (!this->Engine)
		std::cout << "ERROR::SoundEngine: Initialization Failed" << std::endl;
}

void SoundEngine::Delete()
{
	this->Engine->removeAllSoundSources();
	this->Engine->drop();
}

irrklang::ISound* SoundEngine::PlaySound(const char* path, bool loop)
{
	return this->Engine->play2D(path, loop, false, true);
}

void SoundEngine::StopSound(const char * path)
{
	this->Engine->removeSoundSource(path);
}

void SoundEngine::StopAllSounds()
{
	this->Engine->removeAllSoundSources();
}

void SoundEngine::VolumeUp()
{
	float volume = this->Engine->getSoundVolume();
	if (volume < 1.0f)
		this->Engine->setSoundVolume(volume + 0.2f);
}

void SoundEngine::VolumeDown()
{
	float volume = this->Engine->getSoundVolume();
	if (volume > 0.0f)
		this->Engine->setSoundVolume(volume - 0.2f);
}

void SoundEngine::SetSoundVolume(irrklang::ISound * sound, float volume)
{
	if (volume < 0.0f || volume > 1.0f)
		std::cout << "ERROR::SoundEngine: ChangeSoundVolume: Invalid volume argument" << std::endl;

	sound->setVolume(volume);
}
