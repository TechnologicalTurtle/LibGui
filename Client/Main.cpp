#include <iostream>
#include <string>

#include "../LibGui/include/LibGui.h"
using namespace LibGui;

int main()
{
	Debug::LogRepeated = true;
	Window MyMainWindow(Init(), "LibGui - Client.exe", {500, 500});

	DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");

	DynamicText text(MyMainWindow, roboto);
	text.text = "Helloworld!";
	text.maxSize = {160, 68};

	while (!MyMainWindow.ShouldClose())
	{
		MyMainWindow.Draw();

		text.Render();
		MyMainWindow.PushDraw();
	}

	Terminate();

	std::cin.get();
}