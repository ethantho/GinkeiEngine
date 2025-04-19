#include "EngineUtils.h"
#include "TemplateDB.h"

std::unordered_map<std::string, Actor> TemplateDB::loaded_templates;

void TemplateDB::LoadAllTemplates()
{
	std::string template_dir = "resources/actor_templates/";

    if (!EngineUtils::VerifyDirExists(template_dir, false)) {
        return;
    }
    for (const auto& file : std::filesystem::directory_iterator(template_dir)) {
        if (file.is_regular_file() && file.path().extension() == ".template") {

            rapidjson::Document current_template;
            EngineUtils::ReadJsonFile(file.path().string(), current_template);
            const rapidjson::Value& actor = current_template;
            Actor a(actor, true);
			//ActorHelper::ApplyActorOverrides(&a, actor);
            loaded_templates[file.path().stem().string()] = a;
        }
    }
}