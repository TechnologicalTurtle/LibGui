#define LIBGUI_DEBUG
#include "include/LibGui.h"
#include <iostream>

using namespace LibGui;

int main()
{
   // initialize library and create main window
   Window MyWindow(Init(true), "My LibGui window", {500, 500});


   DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");
   DynamicText text(roboto, {100, 100});
   text.text = MyWindow.GetClipboard();

   while (!MyWindow.ShouldClose())
   {
      MyWindow.Draw();

      text.Render();

      MyWindow.PushDraw();
   }

   Terminate();
   return 0;
}