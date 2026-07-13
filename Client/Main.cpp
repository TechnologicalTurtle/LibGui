#include <iostream>
#include <string>

#include "../LibGui/include/LibGui.h"
using namespace LibGui;

void llog()
{
	std::cout << "PRESS: "   << DefaultWindow->cursor.left_button.pressed << '\n';
}

int main()
{
	Debug::LogRepeated = true;
	Window MyMainWindow(Init(), "LibGui - Client.exe", {500, 500});

	//DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");

	//DynamicText text(MyMainWindow, roboto);
	//text.text = "Helloworld!";
	//text.maxSize = {160, 68};

	RoutineManager routines;
	routines.Track(3, llog);

	while (!MyMainWindow.ShouldClose())
	{
		MyMainWindow.Draw();

		if (MyMainWindow.cursor.left_button.clicked) std::cout << "CLICK\n";
		if (MyMainWindow.cursor.left_button.released) std::cout << "RELEASE\n";

		routines.Update();

		//text.Render();
		MyMainWindow.PushDraw();
	}

	Terminate();

	std::cin.get();
}