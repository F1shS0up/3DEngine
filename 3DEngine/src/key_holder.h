#pragma once

#include <string>
#include <unordered_map>

struct key_holder
{
	std::unordered_map<int, bool> keysDown;
	std::unordered_map<int, bool> mouseButtonsDown;

	bool KeyDown(int key);
	bool MouseButtonDown(int button);
};