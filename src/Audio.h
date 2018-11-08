/*
@file Audio.h
*/
#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <cri_adx2le.h>

namespace Audio {

	bool Initialize(const char* acfPath, const char* acbPath, const char* acwPath, const char* dspBusName, size_t playerCount);
	void Update();
	void Destroy();
	void Play(int playerId, int cueId);
	void Resume(int playerId);
	void Pause(int playerId);
	void Stop(int playerId);
	void StopAllSound();
}

#endif