#ifndef ENGINEUTILS_H
#define ENGINEUTILS_H

#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

class EngineUtils
{
public:

	static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
	{
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "rb");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif
		char buffer[65536];
		rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
		out_document.ParseStream(stream);
		std::fclose(file_pointer);

		if (out_document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			std::cout << errorCode;
			exit(0);
		}
	}

	static bool VerifyDirExists(std::string& path, bool required)
	{
		std::filesystem::path resources_dir = path;
		if (required && !std::filesystem::exists(resources_dir)) {
			std::cout << "error: " + path + " missing";
			exit(0);
		}

		return std::filesystem::exists(resources_dir);
	}

	static void TryLoadString(rapidjson::Document& doc, const char* member_name, std::string& target) {
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(member_name);
		if (itr != doc.MemberEnd()) target = itr->value.GetString();
	}

	static void TryLoadInt(rapidjson::Document& doc, const char* member_name, int& target) {
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(member_name);
		if (itr != doc.MemberEnd()) target = itr->value.GetInt();
	}

	static void TryLoadFloat(rapidjson::Document& doc, const char* member_name, float& target) {
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(member_name);
		if (itr != doc.MemberEnd()) target = itr->value.GetFloat();
	}

	static void TryLoadBool(rapidjson::Document& doc, const char* member_name, bool& target) {
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(member_name);
		if (itr != doc.MemberEnd()) target = itr->value.GetBool();
	}

	static void TryLoadUint8(rapidjson::Document& doc, const char* member_name, uint8_t& target) {
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(member_name);
		if (itr != doc.MemberEnd()) target = static_cast<uint8_t>(itr->value.GetInt());
	}

	static std::string ObtainWordAfterPhrase(const std::string& input, const std::string& phrase) {
		size_t pos = input.find(phrase);

		if (pos == std::string::npos) return "";

		pos += phrase.length();
		while (pos < input.size() && std::isspace(input[pos])) {
			++pos;
		}

		if (pos == input.size()) return "";

		size_t end_pos = pos;

		while (end_pos < input.size() && !std::isspace(input[end_pos])) {
			++end_pos;
		}

		return input.substr(pos, end_pos - pos);
	}
	
	static uint64_t To64(glm::ivec2 vec) {
		uint64_t high_bits = static_cast<uint64_t>(static_cast<uint32_t>(vec.x));
		uint64_t low_bits = static_cast<uint64_t>(static_cast<uint32_t>(vec.y));
		return (high_bits << 32) | low_bits;
	}

	static glm::vec2 LoadCamOffset(rapidjson::Document& doc) {

		rapidjson::Value::ConstMemberIterator itr = doc.FindMember("cam_offset_x");
		float x_off = 0.0f;
		if (itr != doc.MemberEnd()) {
			x_off = itr->value.GetFloat();
		}

		float y_off = 0.0f;
		itr = doc.FindMember("cam_offset_y");
		if (itr != doc.MemberEnd()) {
			y_off = itr->value.GetFloat();
		}

		return glm::vec2(x_off, y_off);
	}

	static void Quit() {
		exit(0);
	}

	static void Sleep(int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	static void OpenURL(std::string URL) {
#ifdef _WIN32
		std::string command = "start " + URL;
		std::system(command.c_str());
#elif __APPLE__
		std::string command = "open " + URL;
		std::system(command.c_str());
#else
		//linux
		std::string command = "xdg-open " + URL;
		std::system(command.c_str());
#endif

	}
};




#endif // !ENGINEUTILS_H

