#include "include/LibGui.h"
#include <iostream>

using namespace LibGui;

// this will make first word "red" actually red
static std::string preprocess(const std::string& in)
{
   std::string cpy = in;
   // find it
   int first_bold = in.find("red");
   if (first_bold != std::string::npos) {
      // add ending sequence first (because its later and we dont have to move indexes)
      cpy.insert(first_bold+3, "\033ce");
      // add stating sequence
      cpy.insert(first_bold, "\033c#FF0000");
   }
   return cpy;
}

int main()
{
   Window MyWindow(Init(), "MyWindow", {500, 500});

   DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");

   TextInput MyInput(roboto);
   MyInput.preprocessText = preprocess;
   MyInput.pos = {100, 100};

   while (!MyWindow.ShouldClose())
   {
      MyWindow.Draw();

      MyInput.Update();

      MyWindow.PushDraw();
   }

   Terminate();
   return 0;
}