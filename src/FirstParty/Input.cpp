#include "Input.h"

void Input::Init()
{
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
		keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
	}

	//TODO: remove the secret 0th mouse button
	for (int i = 0; i < 4; ++i) {
		mouse_states.push_back(INPUT_STATE_UP);
	}
}

void Input::ProcessEvent(const SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_scancodes.push_back(e.key.keysym.scancode);
	}

	else if (e.type == SDL_KEYUP) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_scancodes.push_back(e.key.keysym.scancode);
	}

	else if (e.type == SDL_MOUSEMOTION) {
		mouse_position.x = e.motion.x;
		mouse_position.y = e.motion.y;
	}

	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		mouse_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_mouse_buttons.push_back(e.button.button);
	}

	else if (e.type == SDL_MOUSEBUTTONUP) {
		mouse_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_mouse_buttons.push_back(e.button.button);
	}

	else if (e.type == SDL_MOUSEWHEEL) {
		mouse_scroll_this_frame = e.wheel.preciseY;
	}
		
}

void Input::LateUpdate()
{
	for (const SDL_Scancode& code : just_became_down_scancodes) {
		keyboard_states[code] = INPUT_STATE_DOWN;
	}
	just_became_down_scancodes.clear();

	for (const SDL_Scancode& code : just_became_up_scancodes) {
		keyboard_states[code] = INPUT_STATE_UP;
	}
	just_became_up_scancodes.clear();

	for (const int& code : just_became_down_mouse_buttons) {
		mouse_states[code] = INPUT_STATE_DOWN;
	}
	just_became_down_mouse_buttons.clear();

	for (const int& code : just_became_up_mouse_buttons) {
		mouse_states[code] = INPUT_STATE_UP;
	}
	just_became_up_mouse_buttons.clear();

	mouse_scroll_this_frame = 0;
}

bool Input::GetKey(std::string keycode)
{
	auto itr = __keycode_to_scancode.find(keycode);
	if (itr == __keycode_to_scancode.end()) return false;
	SDL_Scancode scancode = itr->second;
	return keyboard_states[scancode] == INPUT_STATE_DOWN || keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(std::string keycode)
{
	auto itr = __keycode_to_scancode.find(keycode);
	if (itr == __keycode_to_scancode.end()) return false;
	SDL_Scancode scancode = itr->second;
	return keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(std::string keycode)
{
	auto itr = __keycode_to_scancode.find(keycode);
	if (itr == __keycode_to_scancode.end()) return false;
	SDL_Scancode scancode = itr->second;
	return keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_UP;
}

bool Input::GetMouseButton(int button)
{
	if (button < 1 || button > 3) return false;

	return mouse_states[button] == INPUT_STATE_DOWN || mouse_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonDown(int button)
{
	if (button < 1 || button > 3) return false;

	return mouse_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(int button)
{
	if (button < 1 || button > 3) return false;

	return mouse_states[button] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetMouseScrollDelta()
{
	return mouse_scroll_this_frame;
}

glm::vec2 Input::GetMousePosition()
{
	/*luabridge::LuaRef ret_table = luabridge::newTable(ComponentDB::lua_state);
	ret_table["x"] = mouse_position.x;
	ret_table["y"] = mouse_position.y;
	return ret_table;*/
	return mouse_position;
}

void Input::HideCursor()
{
	SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor()
{
	SDL_ShowCursor(SDL_ENABLE);
}
