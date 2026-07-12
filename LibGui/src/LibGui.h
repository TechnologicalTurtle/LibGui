#pragma once

#include <vector>
#include <map>
#include <fstream>

struct Vec2i;
// 2D float vector
struct Vec2
{
    float x = 0;
    float y = 0;

    Vec2(int X, int Y) :x(static_cast<float>(X)), y(static_cast<float>(Y)) {};
    Vec2(float X, float Y) :x(X), y(Y) {};
    Vec2(double X, double Y) :x(static_cast<float>(X)), y(static_cast<float>(Y)) {};

    Vec2 operator+ (const Vec2& a) const
    {
        return Vec2(x + a.x, y + a.y);
    }
    Vec2 operator- (const Vec2& a) const
    {
        return Vec2(x - a.x, y - a.y);
    }
    Vec2 operator* (const Vec2& a) const
    {
        return Vec2(x * a.x, y * a.y);
    }
    Vec2 operator/ (const Vec2& a) const
    {
        return Vec2(x / a.x, y / a.y);
    }

    Vec2 operator+ (const float& a) const
    {
        return Vec2(x + a, y + a);
    }
    Vec2 operator- (const float& a) const
    {
        return Vec2(x - a, y - a);
    }
    Vec2 operator* (const float& a) const
    {
        return Vec2(x * a, y * a);
    }
    Vec2 operator/ (const float& a) const
    {
        return Vec2(x / a, y / a);
    }

    bool operator== (const Vec2& a)const
    {
        return x == a.x && y == a.y;
    }

    static Vec2 Abs(Vec2 a);
    static float Distance(Vec2 a, Vec2 b);

    operator Vec2i() const;
    // for std::cout
    friend std::ostream& operator<<(std::ostream& os, const Vec2& dt);
};

// 2D integer vector
struct Vec2i
{
    int x, y = 0;

    Vec2i() :x(0), y(0) {};
    Vec2i(int X, int Y) :x(X), y(Y) {};
    Vec2i(unsigned int X, unsigned int Y) :x(X), y(Y) {};
    Vec2i(float X, float Y) :x(static_cast<int>(X)), y(static_cast<int>(Y)) {};
    Vec2i(double X, double Y) :x(static_cast<int>(X)), y(static_cast<int>(Y)) {};

    Vec2i operator+ (const Vec2i& a) const
    {
        return Vec2i(x + a.x, y + a.y);
    }
    Vec2i operator- (const Vec2i& a) const
    {
        return Vec2i(x - a.x, y - a.y);
    }
    Vec2i operator* (const Vec2i& a) const
    {
        return Vec2i(x * a.x, y * a.y);
    }
    Vec2i operator/ (const Vec2i& a) const
    {
        return Vec2i(x / a.x, y / a.y);
    }

    Vec2i operator+ (const int& a) const
    {
        return Vec2i(x + a, y + a);
    }
    Vec2i operator- (const int& a) const
    {
        return Vec2i(x - a, y - a);
    }
    Vec2i operator* (const int& a) const
    {
        return Vec2i(x * a, y * a);
    }
    Vec2i operator/ (const int& a) const
    {
        return Vec2i(x / a, y / a);
    }

    bool operator== (const Vec2i& a)const
    {
        return x == a.x && y == a.y;
    }

    operator Vec2() const;
    // for std::cout
    friend std::ostream& operator<<(std::ostream& os, const Vec2i& dt);
};

Vec2 clamp(Vec2 v, Vec2 min, Vec2 max);
Vec2i clamp(Vec2i v, Vec2i min, Vec2i max);

struct Color
{
    float r, g, b, a;

    Color() :r(0), g(0), b(0), a(0) {};

    Color(int R, int G, int B) : r(static_cast<float>(R)), g(static_cast<float>(G)), b(static_cast<float>(B)), a(1) {}
    Color(float R, float G, float B) : r(R), g(G), b(B), a(1) {}

    Color(int R, int G, int B, int A) : r(static_cast<float>(R)), g(static_cast<float>(G)), b(static_cast<float>(B)), a(static_cast<float>(A)) {}
    Color(float R, float G, float B, float A) : r(R), g(G), b(B), a(A) {}

    Color operator+ (const Color& A)
    {
        return Color(r + A.r, g + A.g, b + A.b, a + A.a);
    }
    Color operator- (const Color& A)
    {
        return Color(r - A.r, g - A.g, b - A.b, a - A.a);
    }
    Color operator* (const Color& A)
    {
        return Color(r * A.r, g * A.g, b * A.b, a * A.a);
    }
    Color operator/ (const Color& A)
    {
        return Color(r / A.r, g / A.g, b / A.b, a / A.a);
    }

    Color operator+ (const float A)
    {
        return Color(r + A, g + A, b + A, a + A);
    }
    Color operator- (const float A)
    {
        return Color(r - A, g - A, b - A, a - A);
    }
    Color operator* (const float A)
    {
        return Color(r * A, g * A, b * A, a * A);
    }
    Color operator/ (const float A)
    {
        return Color(r / A, g / A, b / A, a / A);
    }

    bool operator== (const Color& A)
    {
        return r == A.r && g == A.g && b == A.b && a == A.a;
    }

    static const Color Transparent;
    static const Color Black;
    static const Color White;
    static const Color Gray;
    static const Color Red;
    static const Color Blue;
    static const Color Green;
    static const Color Purple;
    static const Color Yellow;
    static const Color Brown;
};

struct GLFWwindow;
namespace LibGui
{
    extern bool DebugRender;

    extern double DeltaTime;
    extern double Framerate;

    class Window;
    enum DefShaderType_
    {
        DefShaderType_defVertex   = 0,
        DefShaderType_defFragment = 1
    };
    enum WindowInitFlag_
    {
        WindowInitFlag_FullscreenOnF11 = 0b001,
        WindowInitFlag_CantBeResized   = 0b010
    };
    constexpr int DefaultWindowInitFlags = WindowInitFlag_FullscreenOnF11;
    constexpr int InfoWindowInitFlags = WindowInitFlag_CantBeResized;
    enum Direction_
    {
        Direction_Up, Direction_Bottom, Direction_Right, Direction_Left
    };
    // All functional keys, btw this part is basically copied&pasted from glfw3.h
    enum FunctionalKey_
    {
        // Functional keys
        FunctionalKey_Escape             = 256,
        FunctionalKey_Enter              = 257,
        FunctionalKey_Tab                = 258,
        FunctionalKey_Backspace          = 259,
        FunctionalKey_Insert             = 260,
        FunctionalKey_Delete             = 261,
        FunctionalKey_Right              = 262,
        FunctionalKey_Left               = 263,
        FunctionalKey_Down               = 264,
        FunctionalKey_Up                 = 265,
        FunctionalKey_PageUp             = 266,
        FunctionalKey_PageDown           = 267,
        FunctionalKey_Home               = 268,
        FunctionalKey_End                = 269,
        FunctionalKey_CapsLock           = 280,
        FunctionalKey_ScrollLock         = 281,
        FunctionalKey_NumLock            = 282,
        FunctionalKey_PrintScreen        = 283,
        FunctionalKey_Pause              = 284,
        FunctionalKey_F1                 = 290,
        FunctionalKey_F2                 = 291,
        FunctionalKey_F3                 = 292,
        FunctionalKey_F4                 = 293,
        FunctionalKey_F5                 = 294,
        FunctionalKey_F6                 = 295,
        FunctionalKey_F7                 = 296,
        FunctionalKey_F8                 = 297,
        FunctionalKey_F9                 = 298,
        FunctionalKey_F10                = 299,
        FunctionalKey_F11                = 300,
        FunctionalKey_F12                = 301,
        FunctionalKey_F13                = 302,
        FunctionalKey_F14                = 303,
        FunctionalKey_F15                = 304,
        FunctionalKey_F16                = 305,
        FunctionalKey_F17                = 306,
        FunctionalKey_F18                = 307,
        FunctionalKey_F19                = 308,
        FunctionalKey_F20                = 309,
        FunctionalKey_F21                = 310,
        FunctionalKey_F22                = 311,
        FunctionalKey_F23                = 312,
        FunctionalKey_F24                = 313,
        FunctionalKey_F25                = 314,
        // KP -> keypad
        FunctionalKey_KP_0               = 320,
        // KP -> keypad
        FunctionalKey_KP_1               = 321,
        // KP -> keypad
        FunctionalKey_KP_2               = 322,
        // KP -> keypad
        FunctionalKey_KP_3               = 323,
        // KP -> keypad
        FunctionalKey_KP_4               = 324,
        // KP -> keypad
        FunctionalKey_KP_5               = 325,
        // KP -> keypad
        FunctionalKey_KP_6               = 326,
        // KP -> keypad
        FunctionalKey_KP_7               = 327,
        // KP -> keypad
        FunctionalKey_KP_8               = 328,
        // KP -> keypad
        FunctionalKey_KP_9               = 329,
        // KP -> keypad
        FunctionalKey_KP_Decimal         = 330,
        // KP -> keypad
        FunctionalKey_KP_Divide          = 331,
        // KP -> keypad
        FunctionalKey_KP_Multiply        = 332,
        // KP -> keypad
        FunctionalKey_KP_SUBTRACT        = 333,
        // KP -> keypad
        FunctionalKey_KP_Add             = 334,
        // KP -> keypad
        FunctionalKey_KP_Enter           = 335,
        // KP -> keypad
        FunctionalKey_KP_Equal           = 336,
        FunctionalKey_Left_Shift         = 340,
        FunctionalKey_Left_Control       = 341,
        FunctionalKey_Left_Alt           = 342,
        FunctionalKey_Left_Super         = 343,
        FunctionalKey_Right_Shift        = 344,
        FunctionalKey_Right_Control      = 345,
        FunctionalKey_Right_Alt          = 346,
        FunctionalKey_Right_Super        = 347,
        FunctionalKey_Menu               = 348
    };
    // list of all printable characters so you can easily use KeyTracker.Track(AllPrintableChars)
    extern const char AllPrintableChars[];
    enum CursorShape_
    {
        CursorShape_Default                    = 0x00000000,
        CursorShape_Arrow                      = 0x00036001,
        CursorShape_IBeam                      = 0x00036002,
        CursorShape_Crosshair                  = 0x00036003,
        CursorShape_PointingHand               = 0x00036004,
        CursorShape_HorizontalResize           = 0x00036005,
        CursorShape_VerticalResize             = 0x00036006,
        CursorShape_FromTopLeftDiagonalResize  = 0x00036007,
        CursorShape_FromTopRightDiagonalResize = 0x00036008,
        CursorShape_AllResize                  = 0x00036009,
        CursorShape_NotAllowed                 = 0x0003600A
    };

    class TextFile
    {
    private:
        std::fstream file;
    public:
        TextFile(const std::string& path, bool append = true);
        ~TextFile();

        void Write(std::string& data);
        std::string Get();
    };

    // Shader abstraction
    class Shader
    {
    private:
        unsigned int id;

        bool CheckForCompilationErrors(unsigned int shader, bool isVertex);
    public:
        bool compilationSuccessful = false;
        std::vector<std::string> compilationErrors = {};

        void Compile(const std::string vertexSrc, const std::string fragmentSrc);
        void CompileFromFile(const std::string vertexPath, const std::string fragmentPath);

        Shader();
        Shader(std::string vertexPath, std::string fragmentPath);
        ~Shader();

        void Bind();
        void Unbind();

        void SetUniform1Int(std::string name, int val);
        void SetUniformVec2i(std::string name, Vec2i val); // aka SetUniform2Int()
        void SetUniform3Int(std::string name, int x, int y, int z);
        void SetUniform4Int(std::string name, int x, int y, int z, int w);

        void SetUniform1UInt(std::string name, unsigned int val);
        void SetUniform2UInt(std::string name, unsigned int x, unsigned int y);
        void SetUniform3UInt(std::string name, unsigned int x, unsigned int y, unsigned int z, unsigned int w);
        void SetUniform4UInt(std::string name, unsigned int x, unsigned int y, unsigned int z, unsigned int w);

        void SetUniform1Float(std::string name, float val);
        void SetUniformVec2(std::string name, Vec2 val);
        void SetUniform3Float(std::string name, float x, float y, float z);
        void SetUniform4Float(std::string name, float x, float y, float z, float w);

        unsigned int GetID();
    };

    // Vertex abstraction
    struct Vertex
    {
        Vec2 pos;
        Vec2 texCoord;
        Color color;

        Vertex(const Vec2& Pos, const Color& Color);
        Vertex(const Vec2& Pos, const Vec2& TexCoord, const Color& Color);
    };

    // Basic RGBA pixel
    struct Pixel
    {
        unsigned char R, G, B, A = 0;

        Color ToColor();
        static Pixel FromColor(const Color& color);
    };

    // Editable RGBA image
    class Image
    {
    private:
        std::vector<Pixel> data;
    public:
        Vec2i size;

        // Loads data from file at 'path'. Does not change id. Returns true if loading succeed.
        bool LoadFile(const std::string& path);
        // Fills whole image with fillColor.
        void Clear(Color fillColor = Color::Black);
        // Resize image, then clears it with Clear(fillColor).
        void Resize(Vec2i newSize, Color fillColor = Color::Black);
        // [internal]
        void SetDataRaw(unsigned char* Data, Vec2i Size);

        Image();
        Image(const std::string& path);
        Image(Vec2i Size, Color color = Color::Black);
        // Used to create text characters
        Image(unsigned char* Data, Vec2i Size);

        // Returns reference to pixel at 'pos'
        Pixel& GetPixel(Vec2i pos);
        // Returns reference to pixel at index 'index'
        Pixel& GetPixel(size_t index);

        // Fancy input wrapper for GetPixel()
        void SetPixelColor(Vec2i pos, Color color);
        // Fancy output wrapper for GetPixel()
        Color GetPixelColor(Vec2i pos);
        // Apply function 'effect' to every pixel in image
        void ApplyEffect(void(*effect)(Pixel&, size_t));

        // Flip x-axis
        void FlipXAxis();
        // Flip y-axis
        void FlipYAxis();

        // Remove 'count' of pixels from 'side' of this image
        void RemoveCount(Direction_ side, int count);
        
        // Add image 'img' to 'side' of this image, if 'img' is smaller tha this image, fill blank space with 'fill'
        void MergeImage(Direction_ side, Image& img, Color fill = Color::Transparent);
        // Add 'count' of  pixels with 'color' to 'side' of this image (just MergeImage wrapper)
        void MergeCount(Direction_ side, int cout, Color color);

        // save as .png at 'path'
        bool SavePNG(const std::string& path);
        // save as .jpg at 'path', quality in % (1 is worst, 100 is best)
        bool SaveJPG(const std::string& path, int quality);
        // save as .tga at 'path'
        bool SaveTGA(const std::string& path);
        // save as .bmp at 'path'
        bool SaveBMP(const std::string& path);

        // Returns pointer to first pixel of image, useful for loading textures and saving.
        unsigned char* GetData();
        // Returns copy of vector of pixels in this image, useful for debugging (I guess nothing more actually).
        std::vector<Pixel> GetDataCopy();
    };

    // Texture abstraction
    class Texture
    {
    private:
        unsigned int id;
    public:
        unsigned int texSlot = 0;

        // [GET ONLY] additional info about the file
        int nrChannels = -1;
        Vec2i size;

        // Loads data from file at 'path'. Does not change id. Returns true if loading succeed.
        bool LoadFile(const std::string& path);
        void LoadImage(Image& source);

        Texture();
        Texture(Image& source, bool smooth = true, unsigned int TextureSlot = 0);
        Texture(const std::string& path, bool smooth = true, unsigned int TextureSlot = 0);

        void Bind();
        void Unbind();
        unsigned int GetID();
    };

    extern ::GLFWwindow* MainWindow;
    extern Shader DefaultShader;

    class RoutineManager
    {
    public:
        // all routines 
        // the weird structure is for ability to modify times in a loop; 
        // ...routines don't have to be modified in a loop
        std::map<void(*)(), std::pair<float, float>> routines;

        // add routine 'routine' that is called every 'delay' seconds (or as close as possible); runs in main thread
        void Track(float delay, void(*routine)());
        // removes all routines which ma to function 'routine'
        void Stop(void(*routine)());

        // must be called, runs all routines which are supposed to
        void Update();
    };

    // DO NOT CREATE YOUR OWN! USE window.cursor!
    class MouseCursor
    {
    private:
        GLFWwindow* parent_window;
    public:
        Vec2 position{ 0, 0 };
        double scroll;
        bool left_pressed = false;
        bool left_clicked = false;
        bool left_released = false;

        MouseCursor();
        MouseCursor(GLFWwindow* parent);

        void SetLooks(CursorShape_ shape);
        void SetLooks(Image shape, Vec2i pivot = {0, 0});

        void Update();
    };

    struct KeyStatus
    {
        bool pressed, clicked, released = false;
    };

    class KeyTracker
    {
    private:
        GLFWwindow* parent_window;
        std::map<int, KeyStatus> tracked_keys;
    public:
        KeyTracker();
        KeyTracker(GLFWwindow* window);

        void Update();

        void Track(int key, bool update = true);
        void Track(char key, bool update = true);
        void Track(FunctionalKey_ key, bool update = true);
        void Track(const std::vector<int>& keys);

        const KeyStatus operator[] (int key);
        const KeyStatus operator[] (char key);
        const KeyStatus operator[] (FunctionalKey_ key);
    };

    class Window
    {
    private:
        GLFWwindow* window;
        bool closed = false;

        bool fullscreen = false;
        Vec2i fSize;
        Vec2i fPos;
    public:
        unsigned int pressed_character = 0;

        // mouse cursor
        MouseCursor cursor;
        // key tracker
        KeyTracker key_tracker;

        // background color
        Color BackgroundColor = Color::Black;
        // transparency
        float transparency = 1;

        // can Draw() function close window when user pressed close button?
        bool CanAutoClose = false;

        bool FullscreenOnF11;

        // READ-ONLY window size
        Vec2i size;

        Window(std::string name, Vec2i scale, int InitFlags = DefaultWindowInitFlags);
        Window(GLFWwindow* PreMadeWindow, std::string name, Vec2i scale, int InitFlags = DefaultWindowInitFlags);
        ~Window();

        // Input functions
        // Returns true if specified key is being pressed, false otherwise
        const bool GetKeyPressed(int key);
        // Returns true if specified printable key is being pressed, false otherwise
        const bool GetKeyPressed(char key);
        // Returns true if specified functional key is being pressed, false otherwise
        const bool GetKeyPressed(FunctionalKey_ key);

        // call before render
        void Draw();
        // call after render
        void PushDraw();

        bool ShouldClose();
        void Close();

        // Just change window 'InitFlags' from constructor parameters to new values of 'flags'
        void Reflag(int flags);
        // Set window icon to Image 'img'; 'clamp' the icon must be square, so should cut or squeeze 'img' to fit.
        void SetIcon(Image img, bool clamp = true);

        /* Returns screenshot of last render, so you must do:
           window.Draw();
           //render stuff
           LibGui::Image img = window.Screenshot();
           window.PushDraw();
         */
        Image Screenshot(bool transparent = false);

        void Bind();
    };
    extern Window* DefaultWindow;

    class RectangleInput
    {
    protected:
        // window the object is bound to
        Window& context;

        bool CompareMouseToThis(Vec2 pos, Vec2 scale, float direction);
    public:
        RectangleInput(Window& window) :context(window) {}

        virtual bool Rect_OnMouseEnter(Vec2 anchor = Vec2{ 0.5f, 0.5f });
        bool Rect_OnMouseClick(Vec2 anchor = Vec2{ 0.5f, 0.5f });
        bool Rect_OnMouseDrag(Vec2 anchor = Vec2{ 0.5f, 0.5f });
        bool Rect_OnMouseRelease(Vec2 anchor = Vec2{ 0.5f, 0.5f });

        Window& GetContext();
    };

    enum UniformFlag
    {
        UniformFlag_XPos_Normalized   = 0b1,
        UniformFlag_YPos_Normalized   = 0b10,
        UniformFlag_XScale_Normalized = 0b100,
        UniformFlag_YScale_Normalized = 0b1000,
        UniformFlag_RenderAsLine      = 0b10000
    };
    class Object: public RectangleInput
    {
    private:
        unsigned int VAO{ 0 }, VBO{ 0 }, EBO{ 0 };

        unsigned int indicesCount = 0;

        // 'Custom' type constructor
        void Construct(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices);

        // Send necessary data to GPU
        void SetupUniforms();
    public:
        Vec2 pos{ 0, 0 }; // in case of panel, it's pos1
        Vec2 scale{ 1, 1 }; // in case of panel, it's pos2
        Color color{ 1, 1, 1, 1 };

        // direction
        float direction = 90;

        // describes what axes (X/Y) are normalized
        char UniformFlags = 0;

        Shader& shader;
        Texture* texture = nullptr;
        
        // constructors
        void InitAsCustomPolygon(const std::vector<Vertex>& Vertices);
        void InitAsCustom(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices);
        void InitAsRectangle(Vec2 position, Vec2 size, Color Color, Vec2 anchor = Vec2{ 0.5f, 0.5f });
        void InitAsRectangle(Vec2 position, Vec2 size, Texture& tex, Vec2 anchor = Vec2{ 0.5f, 0.5f });
        void InitAsRegularPolygon(Vec2 position, int verticesCount, int radius, Color Color);
        void InitAsLineStrip(const std::vector<Vertex>& points);
        void InitAsLineStrip(const std::vector<Vec2>& points, Color Color);

        Object();
        Object(Window& window);
        Object(Shader& Shader);
        Object(Window& window, Shader& Shader);

        // Render; 'custom_uniSetup' determines if SetupUniforms() should be called before render
        void Render(bool custom_uniSetup = false);

        // Interaction
        bool Rect_OnMouseEnter(Vec2 anchor = Vec2{ 0.5f, 0.5f }) override;
    };

    class DT_TextureAtlas;
    // Dynamically rendered text (the normal way to render it); kinda faster if you want to change it
    class DynamicText : public RectangleInput
    {
    private:
        Object core;
        Object background;

        void UpdateTFSandBackground();
        int textYOffset = 0;

        Color charBackColor = Color::Transparent;
    public:
        DT_TextureAtlas* atlas;

        std::string text;
        Vec2i pos{ 0, 0 };
        float fontSize = 1;
        // size of smallest rectangle to cover rendered text
        Vec2i textFieldSize{ 0, 0 };
        // also called background padding
        Vec2 backgroundAddSize{ 10, 10 };

        Vec2 minSize{ -1, -1 };
        Vec2 maxSize{ -1, -1 };

        Color textColor = Color::White;
        Color backColor = Color::Transparent;

        float letterGap = 0.06f;

        DynamicText(DT_TextureAtlas& font, Vec2i position = {0, 0}, Color text_color = Color::White, Color background_color = Color::Transparent);
        DynamicText(Window& window, DT_TextureAtlas& font, Vec2i position = {0, 0}, Color text_color = Color::White, Color background_color = Color::Transparent);

        int RenderChar(char character, Vec2 char_pos);
        void Render();
        // if 'text' changed -> update 'textFieldSize' value
        void UpdateTextFieldSize();
        Vec2 GetFinalSize();

        // Interaction - ignore 'anchor' parameter
        bool Rect_OnMouseEnter(Vec2 anchor = Vec2{ 0.5f, 0.5f }) override;

        // Interaction - returns index of character mouse cursor is on; returns -1 if cursor is outside of text field
        int GetTouchedCharIndex();
        Vec2 GetPositionOnIndex(int character_index, bool left_side = true);
    };
    struct DT_Character {
        Texture texture;
        Vec2i   bearing;    // Offset from baseline to left/top of glyph
        unsigned int advance;    // Offset to advance to next glyph
    };
    class DT_TextureAtlas
    {
    private:
        std::map<char, DT_Character> atlas;
    public:
        int maxHeight;
        DT_TextureAtlas(const std::string& source, unsigned int quality = 64, bool pixelated = false);

        DT_Character GetChar(char character);
        Vec2i GetCharSize(char character);

        friend int DynamicText::RenderChar(char character, Vec2 char_pos);
    };

    class TextInput: public RectangleInput
    {
    private:
        DynamicText render;
        Object textCursor;
    public:
        bool active = false;

        std::string allowedChars;
        std::string text;
        std::string(*preprocessText)(const std::string&) = nullptr;

        Vec2& minSize = render.minSize;
        Vec2& maxSize = render.maxSize;
        Color& textColor = render.textColor;
        Color& backColor = render.backColor;

        Vec2i pos;
        int cursor_pos = 0;

        float textCursorTickRate = 0.5f;

        TextInput(Window& window, DT_TextureAtlas& font);

        void Update(bool render_afterwards = true);
        void Render();

        bool Rect_OnMouseEnter(Vec2 anchor = {0, 0}) override;
    };

    class Graph: public RectangleInput
    {
    private:
        Object lines;
        Object background;
        Object info_line;
        DynamicText text;

        float biggest = 0;
    public:
        std::vector<float>* data_source = nullptr;

        float scale = 1;
        Vec2 position{ 250, 250 };
        // center of the object
        Vec2 anchor{ 0, 0 };
        Vec2 desired_size{ 150, 150 };
        Vec2 background_padding{ 10, 10 };

        Vec2 finalSize{ 0, 0 };

        Color lineColor = Color::White;
        Color textColor = Color::White;
        Color infoLineColor = { 1.0f, 1.0f, 1.0f, 0.5f };
        Color backgroundColor = Color::Black;

        bool anyText = true;
        float fontSize = 0.2f;
        int textLeftOffset = 5;
        float infoLineYDelta = 0;

        Graph(Vec2 pos, Vec2 desSize, std::vector<float>* data_source, DT_TextureAtlas& font);

        void Update();

        void Render();

        // Interaction - ignore 'anchor' parameter
        bool Rect_OnMouseEnter(Vec2 anchor = Vec2{ 0.5f, 0.5f }) override;
    };

    /* Initialization of the library, use at start as following
    * LibGui::Window myMainWindow(LibGui::Init(), "name of my cool window", { width, height }); */
    GLFWwindow* Init();
    // Handles non-critical stuff (for example Framerate), is called from MainWindow .Draw() function, but if you want to freeze main window, you have to call this manually.
    void ManualUpdate();
    void Terminate();

    // Get time since initialization of this library
    double GetLibTime();

    // Returns source code of LibGui's default shaders
    std::string GetDefShaderCode(DefShaderType_ which);

    // If user hovered button 'obj.color' is set to 'defaultColor'; returns if button is pressed; 'ChangeMouseShape' is ok with one call per frame, else it'll get buggy
    bool RectButton(Object& obj, Color defaultColor, Color hoverColor, Vec2 anchor = {0.5f, 0.5f}, bool ChangeMouseShape = false);
    // If user hovered button 'obj.backColor' is set to 'defaultColor'; returns if button is pressed 'ChangeMouseShape' is ok with one call per frame, else it'll get buggy
    bool RectButton(DynamicText& obj, Color defaultColor, Color hoverColor, bool ChangeMouseShape = false);
    // If user hovered button 'obj.backgroundColor' is set to 'defaultColor'; returns if button is pressed 'ChangeMouseShape' is ok with one call per frame, else it'll get buggy
    bool RectButton(Graph& obj, Color defaultColor, Color hoverColor, bool ChangeMouseShape = false);

    void SetEdges(Vec2 min, Vec2 max);

    // Just a few debug log functions.
    namespace Debug
    {
        // Log the same 'message', if the last one is same?
        extern bool LogRepeated;
        // Crash on Error()
        extern bool ErrorCrash;

        void Happy(const std::string& message);
        void Log(const std::string& message);
        void Warn(const std::string& message);
        void Error(const std::string& message);
        void FatalError(const std::string& message);
    }
}