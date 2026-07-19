#include "include/LibGui.h"

using namespace LibGui;

int main()
{
   // initialize library and create main window
   LibGui::Window MyWindow(LibGui::Init(), "My LibGui window", {500, 500});

   DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");

   TextInput text(roboto);
   text.pos = {100, 100};
   text.minSize = {150, -1};
   text.fontSize = 0.5f;

   while (!MyWindow.ShouldClose())
   {
      MyWindow.Draw();

      text.Update();

      MyWindow.PushDraw();
   }

   LibGui::Terminate();
   return 0;
}