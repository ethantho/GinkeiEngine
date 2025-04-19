#include "CameraAPI.h"

void CameraAPI::SetPosition(float x, float y)
{
	renderer->cam_position = glm::vec2(x, y);
}

float CameraAPI::GetPositionX()
{
	return renderer->cam_position.x;
}

float CameraAPI::GetPositionY()
{
	return renderer->cam_position.y;
}

void CameraAPI::SetZoom(float zoom_factor)
{
	renderer->zoom_factor = zoom_factor;
}

float CameraAPI::GetZoom()
{
	return renderer->zoom_factor;
}
