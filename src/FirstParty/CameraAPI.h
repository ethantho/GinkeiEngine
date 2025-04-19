#pragma once

#include "Renderer.h"

class CameraAPI
{

public:
	static inline Renderer* renderer;
	static void SetPosition(float x, float y);
	static float GetPositionX();
	static float GetPositionY();
	static void SetZoom(float zoom_factor);
	static float GetZoom();

};

