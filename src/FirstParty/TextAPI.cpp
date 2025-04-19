#include "TextAPI.h"

void TextAPI::Draw(std::string str_content, float x, float y, std::string font_name, int font_size, int r, int g, int b, int a)
{
	renderer->textDrawCalls.push({ str_content, font_name, x, y, font_size, r, g, b, a });
}