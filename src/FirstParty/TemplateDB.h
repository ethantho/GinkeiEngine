#pragma once

#include "Actor.h"
#include <string>
#include <unordered_map>

class TemplateDB
{
public:
	static std::unordered_map<std::string, Actor> loaded_templates;
	static void LoadAllTemplates();

};

