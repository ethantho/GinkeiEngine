#pragma once

#include "AudioHelper.h"
#include "rapidjson/document.h"
#include <unordered_map>

class AudioDB
{

public:
	static void StopBGM();
	static void HaltChannel(int channel);
	static void SetVolume(int channel, int volume);
	static void PlayChannel(int channel, const std::string name, bool loop);
	static bool LoadAudioFromFile(rapidjson::Document& doc, std::string audio_name);
	static bool LoadAudioFromFileName(std::string audio_name, std::string loaded_name);
	static bool HasAudioName(std::string name);

private:
	static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
};

