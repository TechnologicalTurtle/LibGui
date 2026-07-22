#include "include/LibGui.h"

using namespace LibGui;

static std::string preprocess(const std::string& in)
{
   std::string cpy = in;
   int first_bold = in.find("bold");
   if (first_bold != std::string::npos) {
      cpy.insert(first_bold+4, "\033ce\033fe\033he");
      cpy.insert(first_bold, "\033f0\033c#F0FF0F\033h$0000FF90");
   }
   return cpy;
}

int main()
{
   // initialize library and create main window
   Window MyWindow(Init(), "My LibGui window", {500, 500});

   DT_TextureAtlas roboto("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf", 12);
   DT_TextureAtlas roboldo("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Bold.ttf", 12, true);

   TextInput text(roboto);
   text.pos = {100, 100};
   text.rich_text_font_list = {&roboldo};
   text.preprocessText = preprocess;
   text.fontSize = 5.0f;

   /*DynamicText text(roboto, {100, 100});
   text.text = "\033f0\033c#F0FF0Fskybydy\033ce\033fe";
   text.fontSize = 1.0f;
   text.backColor = {0.1f, 0.1f, 0.1f};*/

   while (!MyWindow.ShouldClose())
   {
      MyWindow.Draw();

      text.Update();

      MyWindow.PushDraw();
   }

   Terminate();

   return 0;
}