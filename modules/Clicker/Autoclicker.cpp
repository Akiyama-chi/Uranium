#include "Autoclicker.h"
#include "../../ui/ui.hh"
#include "../../Utils/MinecraftChecker.h"
#include <Windows.h>
#include <random>

float Randomization(int minimum, int maximum)
{

	if (minimum == maximum) {
		return static_cast<float>(Random::GenerateNum(260, 880) / minimum);
	}

	float delay = 1000 / ((rand() % ((maximum - minimum) * 10) / 10 + minimum) * 2);
	
	return delay;
}

void LeftClick() {
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(15);

	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void RightClick() {
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;

	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(15);

	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}


void clicker_event()
{
	if (clicker::OnlyMinecraft)
	{
		// Check if the window name contains "Minecraft". I know it's bad, but I unfortunately lost my src way back that has better check, now I forgot how I made that.
		if (MinecraftChecker::IsMinecraftBedrockFocused())
		{
			if (clicker::LeftClicker)
				if (mouseDown && !rightMouseDown)
				{
					LeftClick();
					Sleep(Randomization(clicker::minCPS, clicker::maxCPS) * 1.23);
				}
		}
	}
	else if (clicker::OnlyMinecraft == false)
	{
		if (clicker::LeftClicker)
			if (mouseDown && !rightMouseDown)
			{
				LeftClick();
				Sleep(Randomization(clicker::minCPS, clicker::maxCPS) * 1.23);
			}
	}
}

void right_event()
{
	if (clicker::OnlyMinecraft)
	{
		if (MinecraftChecker::IsMinecraftBedrockFocused())
		{
			if (clicker::RightClicker)
				if (rightMouseDown && !mouseDown)
				{
					RightClick();
					Sleep(Randomization(clicker::minRCPS, clicker::maxRCPS) * 1.23);
				}
		}
	}
	else if (clicker::OnlyMinecraft == false)
	{
		if (clicker::RightClicker)
			if (rightMouseDown && !mouseDown)
			{
				RightClick();
				Sleep(Randomization(clicker::minRCPS, clicker::maxRCPS) * 1.23);
			}
	}
}

void autoclicker()
{
	while (true) {
		clicker_event();
		right_event();
	}
}


void clickerthread()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)hook::hookmouse,0,0,0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)autoclicker, 0, 0, 0);
}