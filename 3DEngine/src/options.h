#pragma once
#include <iostream>
#include <string>
struct options
{
	enum aspect_ratio_mode
	{
		KeepWidth,
		KeepHeight,
		KeepBoth, // STREACH
		BlackBarsHorizontally,
		BlackBarsVertically,
		AutoBlackBars
	};
	enum window
	{
		Windowed,
		Fullscreen,
		Borderless
	};

	aspect_ratio_mode upscalingBehaviour = AutoBlackBars;
	window windowOption = Windowed;

	int wndWidth = 1920, wndHeight = 1920;
	int maxFPS = 0;
	bool centerWindow = true;
	bool vSync = false;
	bool resizable = false;
	std::string windowName = "Game";
};