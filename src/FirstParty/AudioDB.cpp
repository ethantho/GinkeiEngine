#include "AudioDB.h"


void AudioDB::StopBGM()
{
	AudioHelper::Mix_HaltChannel(0);
}

void AudioDB::HaltChannel(int channel)
{
	AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, int volume)
{
	AudioHelper::Mix_Volume(channel, volume);
}

void AudioDB::PlayChannel(int channel, const std::string name, bool loop)
{
	int result = AudioHelper::Mix_PlayChannel(channel, loaded_audio[name], loop ? -1 : 0);
	//result = result;
}

bool AudioDB::HasAudioName(std::string name)
{
	if (auto it{ loaded_audio.find(name) }; it != std::end(loaded_audio)) {
		return true;
	}
	return false;
}

bool AudioDB::LoadAudioFromFile(rapidjson::Document& doc, std::string audio_name)
{
	rapidjson::Value::ConstMemberIterator itr = doc.FindMember(audio_name.c_str());
	if (itr != doc.MemberEnd() && itr->value.IsString()) {
		const std::string current_item = itr->value.GetString();
		std::string resources_dir = "resources/audio/" + current_item;
		if (std::filesystem::exists(resources_dir + ".wav")) {
			loaded_audio[audio_name] = AudioHelper::Mix_LoadWAV((resources_dir + ".wav").c_str());
		}
		else {
			loaded_audio[audio_name] = AudioHelper::Mix_LoadWAV((resources_dir + ".ogg").c_str());
			if (!loaded_audio[audio_name]) {
				std::cout << "error: failed to play audio clip " << current_item;
				exit(0);
			}
		}
		return true;
	}

	return false;
}

bool AudioDB::LoadAudioFromFileName(std::string audio_file_name, std::string loaded_name)
{
	const std::string current_item = audio_file_name;
	std::string resources_dir = "resources/audio/" + current_item;

	if (std::filesystem::exists(resources_dir + ".wav")) {
		loaded_audio[loaded_name] = AudioHelper::Mix_LoadWAV((resources_dir + ".wav").c_str());
	}
	else {
		loaded_audio[loaded_name] = AudioHelper::Mix_LoadWAV((resources_dir + ".ogg").c_str());
		if (!loaded_audio[loaded_name]) {
			std::cout << "error: failed to play audio clip " << current_item;
			exit(0);
		}
	}

	return true;
}
