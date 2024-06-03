#include "key_holder.h"

bool key_holder::KeyDown(int key)
{
	return keysDown[key];
}

bool key_holder::MouseButtonDown(int button)
{
	return mouseButtonsDown[button];
}