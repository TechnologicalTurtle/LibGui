#define LIBGUI_DEBUG
#include "include/LibGui.h"
#include <iostream>

using namespace LibGui;

int main()
{
   // initialize library and create main window
   Window MyWindow(Init(true), "My LibGui window", {500, 500});


   DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");

   std::vector<float> data = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

   Graph mygraph({100, 400}, {300, 300}, &data, roboto);

   while (!MyWindow.ShouldClose())
   {
      MyWindow.Draw();

      mygraph.Render();

      MyWindow.PushDraw();
   }

   Terminate();
   return 0;
}