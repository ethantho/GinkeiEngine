#include "TextDB.h"
#include <filesystem>
#include <iostream>

std::vector<std::string> TextDB::loaded_text;
TTF_Font* TextDB::loaded_font;

void TextDB::Init()
{
	TTF_Init();
}

void TextDB::LoadFont(std::string font_name, int point_size)
{
	std::string resources_dir = "resources/fonts/" + font_name + ".ttf";
	std::filesystem::path directory_path(resources_dir);

	loaded_font = TTF_OpenFont((resources_dir.c_str()), point_size);

	if (loaded_font == nullptr) {
		std::cout << "error: font " << font_name << " missing";
		exit(0);
	}

	fonts[font_name][point_size] = loaded_font;
}

TTF_Font* TextDB::GetFont(std::string font_name, int point_size)
{
	auto itr = fonts.find(font_name);
	if (itr == fonts.end()) {
		LoadFont(font_name, point_size);
	}
	else {
		auto itr2 = itr->second.find(point_size);
		if (itr2 == itr->second.end()) {
			LoadFont(font_name, point_size);
		}
	}
	return fonts[font_name][point_size];
}
