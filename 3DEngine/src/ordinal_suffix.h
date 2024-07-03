#pragma once

const char* ordinal_suffix(int n)
{
	static const char suffixes[][3] = {"th", "st", "nd", "rd"};
	auto ord = n % 100;
	if (ord / 10 == 1)
	{
		ord = 0;
	}
	ord = ord % 10;
	if (ord > 3)
	{
		ord = 0;
	}
	return suffixes[ord];
}