#include "Actor.h"
#include "AudioDB.h"
#include "CameraAPI.h"
#include "Collision.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "EventBus.h"
#include "ImageAPI.h"
#include "ImageDB.h"
#include "Input.h"
#include "ParticleSystem.h"
#include "PhysicsAPI.h"
#include "Rigidbody.h"
#include "SceneDB.h"
#include "TemplateDB.h"
#include "TextAPI.h"
#include "TextDB.h"
#include <algorithm>

void Engine::GameLoop()
{

	DoInitialSetup();

	while (running) {

		Input();

		Update();

		Render();
	}
}

void Engine::Input()
{
	SDL_Event inputEvent;
	while (Helper::SDL_PollEvent(&inputEvent))
	{
		Input::ProcessEvent(inputEvent);
		switch (inputEvent.type) {
			case SDL_QUIT:
				choice = "quit";
				break;
		}
	}
}

void Engine::Update()
{
	if (gameover) {
		if (choice == "quit") {
			running = false;
		}
		return;
	}
	//if (Rigidbody::world_initialized) Rigidbody::b2world->Step(1.0f / 60.0f, 8, 3);
	UpdateActors();
	Input::LateUpdate();
	EventBus::UpdateSubscriptions();
	if(Rigidbody::world_initialized) Rigidbody::b2world->Step(1.0f / 60.0f, 8, 3);
}

void Engine::Render()
{
	renderer.Clear();

	renderer.DrawActorImages();
	renderer.DrawUI();
	renderer.DrawAllText();
	renderer.DrawAllPixels();

	renderer.PresentRender();

	if (choice == "quit") {
		running = false;
		return;
	}

	if (SceneDB::scene_to_load != "") {
		SceneDB::LoadScene(SceneDB::scene_to_load);
		SceneDB::scene_to_load = "";
	}
}

void Engine::UpdateActors() {

	//add all the instantiated actors to the main actors vector
	size_t num_instantiated = SceneDB::current_scene.instantiated_actors_this_frame.size();
	size_t old_num_actors = SceneDB::current_scene.actors.size();
	SceneDB::current_scene.actors.insert(SceneDB::current_scene.actors.end(), SceneDB::current_scene.instantiated_actors_this_frame.begin(), SceneDB::current_scene.instantiated_actors_this_frame.end());
	SceneDB::current_scene.instantiated_actors_this_frame.clear();
	for (size_t i = 0; i < num_instantiated; ++i) {
		SceneDB::current_scene.new_actors_this_frame.push_back(SceneDB::current_scene.actors[old_num_actors + i]);
	}

	//call OnStart on all new actors
	for (Actor* a : SceneDB::current_scene.new_actors_this_frame) {
		a->Start();
	}
	SceneDB::current_scene.new_actors_this_frame.clear();

	//update all actors
	for (Actor* a : SceneDB::current_scene.actors) {
		a->Update();
	}

	//late update all actors
	for (Actor* a : SceneDB::current_scene.actors) {
		a->LateUpdate();
	}

	//remove actors marked for removal this frame
	//TODO: does this invalidate all my pointers?
	for (Actor* a : SceneDB::current_scene.actors_to_remove) {
		auto itr = std::find(SceneDB::current_scene.instantiated_actors_this_frame.begin(), SceneDB::current_scene.instantiated_actors_this_frame.end(), a);
		if ( itr != SceneDB::current_scene.instantiated_actors_this_frame.end()){
			SceneDB::current_scene.instantiated_actors_this_frame.erase(std::remove(SceneDB::current_scene.instantiated_actors_this_frame.begin(), SceneDB::current_scene.instantiated_actors_this_frame.end(), a), SceneDB::current_scene.instantiated_actors_this_frame.end());
			delete a;
		}
		else {
			SceneDB::current_scene.actors.erase(std::remove(SceneDB::current_scene.actors.begin(), SceneDB::current_scene.actors.end(), a), SceneDB::current_scene.actors.end());
			delete a;
		}
	}
	SceneDB::current_scene.actors_to_remove.clear();
}

void Engine::DoInitialSetup()
{
	ComponentDB::InitializeState();
	InjectClasses();
	ComponentDB::InitializeOther();
	

	std::string resources_path = "resources/";
	EngineUtils::VerifyDirExists(resources_path, true);

	std::string config_path = "resources/game.config";
	EngineUtils::VerifyDirExists(config_path, true);

	rapidjson::Document game_config;
	EngineUtils::ReadJsonFile(config_path, game_config);

	std::string game_start_message = "";
	if (game_config.HasMember("game_start_message")) {
		game_start_message = game_config["game_start_message"].GetString();
	}

	std::string rendering_config_path = "resources/rendering.config";

	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;

	rapidjson::Document rendering_config;

	if (EngineUtils::VerifyDirExists(rendering_config_path, false)) {
		EngineUtils::ReadJsonFile(rendering_config_path, rendering_config);
		EngineUtils::TryLoadInt(rendering_config, "x_resolution", cam_width);
		EngineUtils::TryLoadInt(rendering_config, "y_resolution", cam_height);

		EngineUtils::TryLoadUint8(rendering_config, "clear_color_r", r);
		EngineUtils::TryLoadUint8(rendering_config, "clear_color_g", g);
		EngineUtils::TryLoadUint8(rendering_config, "clear_color_b", b);

		EngineUtils::TryLoadFloat(rendering_config, "zoom_factor", renderer.zoom_factor);
		EngineUtils::TryLoadFloat(rendering_config, "cam_ease_factor", renderer.cam_ease_factor);
	}

	std::string game_title = "";
	EngineUtils::TryLoadString(game_config, "game_title", game_title);


	renderer.CreateWindow(game_title.c_str(), cam_width, cam_height);
	renderer.CreateRenderer(r, g, b);
	renderer.clear_r = r;
	renderer.clear_g = g;
	renderer.clear_b = b;
	renderer.clear_a = 255;

	TextDB::Init();

	TemplateDB::LoadAllTemplates();

	SceneDB::LoadInitialScene();

	if (EngineUtils::VerifyDirExists(rendering_config_path, false)){
		renderer.cam_offset = EngineUtils::LoadCamOffset(rendering_config);
	}
	else {
		renderer.cam_offset = glm::vec2(0.0f, 0.0f);
	}
	

	if (game_start_message != "") {
		std::cout << game_start_message << std::endl;

	}

	Input::Init();
	TextAPI::renderer = &renderer;
	ImageAPI::renderer = &renderer;
	ImageDB::sdl_renderer = renderer.sdl_renderer;
	CameraAPI::renderer = &renderer;
}

bool Engine::CheckForGameEnd()
{
	if (gameover) {
		running = false;
		return true;
	}

	return false;
}

void Engine::InjectClasses()
{
	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Actor")
		.addFunction("Find", SceneDB::FindActor)
		.addFunction("FindAll", SceneDB::FindAllActors)
		.addFunction("Instantiate", SceneDB::InstantiateActor)
		.addFunction("Destroy", SceneDB::DestroyActor)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addData("x", &b2Vec2::x)
		.addData("y", &b2Vec2::y)
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.endClass();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Vector2")
		.addFunction("Distance", b2Distance)
		.addFunction("Dot", static_cast<float(*) (const b2Vec2&, const b2Vec2&)>(b2Dot))
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", EngineUtils::Quit)
		.addFunction("GetFrame", Helper::GetFrameNumber)
		.addFunction("Sleep", EngineUtils::Sleep)
		.addFunction("OpenURL", EngineUtils::OpenURL)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Input")
		.addFunction("GetKey", Input::GetKey)
		.addFunction("GetKeyDown", Input::GetKeyDown)
		.addFunction("GetKeyUp", Input::GetKeyUp)
		.addFunction("GetMousePosition", Input::GetMousePosition)
		.addFunction("GetMouseButton", Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", Input::GetMouseButtonUp)
		.addFunction("GetMouseScrollDelta", Input::GetMouseScrollDelta)
		.addFunction("HideCursor", Input::HideCursor)
		.addFunction("ShowCursor", Input::ShowCursor)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", TextAPI::Draw)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", AudioDB::PlayChannel)
		.addFunction("Halt", AudioDB::HaltChannel)
		.addFunction("SetVolume", AudioDB::SetVolume)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Image")
		.addFunction("DrawUI", ImageAPI::DrawUI)
		.addFunction("DrawUIEx", ImageAPI::DrawUIEx)
		.addFunction("Draw", ImageAPI::Draw)
		.addFunction("DrawEx", ImageAPI::DrawEx)
		.addFunction("DrawPixel", ImageAPI::DrawPixel)
		.addFunction("DrawTile", ImageAPI::DrawTile)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", CameraAPI::SetPosition)
		.addFunction("GetPositionX", CameraAPI::GetPositionX)
		.addFunction("GetPositionY", CameraAPI::GetPositionY)
		.addFunction("SetZoom", CameraAPI::SetZoom)
		.addFunction("GetZoom", CameraAPI::GetZoom)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", SceneDB::SetSceneToLoad)
		.addFunction("GetCurrent", SceneDB::GetCurrent)
		.addFunction("DontDestroy", SceneDB::DontDestroy)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<Rigidbody>("Rigidbody")
		.addData("enabled", &Rigidbody::enabled)
		.addData("key", &Rigidbody::key)
		.addData("type_name", &Rigidbody::type_name)
		.addData("actor", &Rigidbody::actor)
		.addData("x", &Rigidbody::x)
		.addData("y", &Rigidbody::y)
		.addData("body_type", &Rigidbody::body_type)
		.addData("precise", &Rigidbody::precise)
		.addData("gravity_scale", &Rigidbody::gravity_scale)
		.addData("density", &Rigidbody::density)
		.addData("angular_friction", &Rigidbody::angular_friction)
		.addData("rotation", &Rigidbody::rotation)
		.addData("has_collider", &Rigidbody::has_collider)
		.addData("has_trigger", &Rigidbody::has_trigger)
		.addData("collider_type", &Rigidbody::collider_type)
		.addData("trigger_type", &Rigidbody::trigger_type)
		.addData("width", &Rigidbody::width)
		.addData("height", &Rigidbody::height)
		.addData("radius", &Rigidbody::radius)
		.addData("trigger_width", &Rigidbody::trigger_width)
		.addData("trigger_height", &Rigidbody::trigger_height)
		.addData("trigger_radius", &Rigidbody::trigger_radius)
		.addData("friction", &Rigidbody::friction)
		.addData("bounciness", &Rigidbody::bounciness)
		.addFunction("GetPosition", &Rigidbody::GetPosition)
		.addFunction("GetRotation", &Rigidbody::GetRotation)
		.addFunction("OnStart", &Rigidbody::OnStart)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.endClass();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<Collision>("collision")
		.addProperty("other", &Collision::other)
		.addProperty("normal", &Collision::normal)
		.addProperty("point", &Collision::point)
		.addProperty("relative_velocity", &Collision::relative_velocity)
		.endClass();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<HitResult>("HitResult")
		.addProperty("actor", &HitResult::actor)
		.addProperty("point", &HitResult::point)
		.addProperty("normal", &HitResult::normal)
		.addProperty("is_trigger", &HitResult::is_trigger)
		.endClass();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", &PhysicsAPI::Raycast)
		.addFunction("RaycastAll", &PhysicsAPI::RaycastAll)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Event")
		.addFunction("Publish", &EventBus::Publish)
		.addFunction("Subscribe", &EventBus::Subscribe)
		.addFunction("Unsubscribe", &EventBus::Unsubscribe)
		.endNamespace();

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginClass<ParticleSystem>("ParticleSystem")
		.addData("enabled", &ParticleSystem::enabled)
		.addData("key", &ParticleSystem::key)
		.addData("type_name", &ParticleSystem::type_name)
		.addData("actor", &ParticleSystem::actor)
		.addData("x", &ParticleSystem::x)
		.addData("y", &ParticleSystem::y)
		.addData("frames_between_bursts", &ParticleSystem::frames_between_bursts)
		.addData("burst_quantity", &ParticleSystem::burst_quantity)
		.addData("start_scale_min", &ParticleSystem::start_scale_min)
		.addData("start_scale_max", &ParticleSystem::start_scale_max)
		.addData("rotation_min", &ParticleSystem::rotation_min)
		.addData("rotation_max", &ParticleSystem::rotation_max)
		.addData("start_color_r", &ParticleSystem::start_color_r)
		.addData("start_color_g", &ParticleSystem::start_color_g)
		.addData("start_color_b", &ParticleSystem::start_color_b)
		.addData("start_color_a", &ParticleSystem::start_color_a)
		.addData("emit_radius_min", &ParticleSystem::emit_radius_min)
		.addData("emit_radius_max", &ParticleSystem::emit_radius_max)
		.addData("emit_angle_min", &ParticleSystem::emit_angle_min)
		.addData("emit_angle_max", &ParticleSystem::emit_angle_max)
		.addData("image", &ParticleSystem::image)
		.addData("sorting_order", &ParticleSystem::sorting_order)
		.addData("duration_frames", &ParticleSystem::duration_frames)
		.addData("start_speed_min", &ParticleSystem::start_speed_min)
		.addData("start_speed_max", &ParticleSystem::start_speed_max)
		.addData("rotation_speed_min", &ParticleSystem::rotation_speed_min)
		.addData("rotation_speed_max", &ParticleSystem::rotation_speed_max)
		.addData("gravity_scale_x", &ParticleSystem::gravity_scale_x)
		.addData("gravity_scale_y", &ParticleSystem::gravity_scale_y)
		.addData("drag_factor", &ParticleSystem::drag_factor)
		.addData("angular_drag_factor", &ParticleSystem::angular_drag_factor)
		.addData("end_scale", &ParticleSystem::end_scale)
		.addData("end_color_r", &ParticleSystem::end_color_r)
		.addData("end_color_g", &ParticleSystem::end_color_g)
		.addData("end_color_b", &ParticleSystem::end_color_b)
		.addData("end_color_a", &ParticleSystem::end_color_a)
		.addFunction("OnStart", &ParticleSystem::OnStart)
		.addFunction("OnUpdate", &ParticleSystem::OnUpdate)
		.addFunction("Stop", &ParticleSystem::Stop)
		.addFunction("Play", &ParticleSystem::Play)
		.addFunction("Burst", &ParticleSystem::Burst)
		.endClass();
}


