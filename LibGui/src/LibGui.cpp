#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "../include/LibGui.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <functional>

// just random math function
constexpr float PI = 3.1415927f;
static float radians(float degrees)
{
	return degrees * (PI / 180);
}

std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
	os << v.x << ", " << v.y;
	return os;
}
std::ostream& operator<<(std::ostream& os, const Vec2i& v)
{
	os << v.x << ", " << v.y;
	return os;
}

static Vec2 LG_EdgeMin{-1, -1};
static Vec2 LG_EdgeMax{-1, -1};

static LibGui::Shader dtS;
// Shaders source code
// DEFAULT shaders
constexpr std::string_view src_defVertex =
"#version 330 core\n\n"
"uniform vec2 u_ParentPos;\n"
"uniform vec2 u_UpVector;\n"
"uniform vec2 u_RightVector;\n"
"uniform vec4 u_Color;\n\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"layout (location = 2) in vec4 aColor;\n\n"
"out vec4 fColor;\n"
"out vec2 fTexCoord;\n\n"
"void main()\n{\n"
"  fColor = aColor * u_Color;\n"
"  fTexCoord = aTexCoord;\n\n"
"  // position transformations\n"
"  vec2 pos = aPos;\n\n"
"  // direction & scale\n"
"  pos = (pos.x*u_RightVector) + (pos.y*u_UpVector);\n\n"
"  // position\n"
"  pos += vec2(u_ParentPos.x, -u_ParentPos.y);\n\n"
"  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\n"
"}";
constexpr std::string_view src_defFragment =
"#version 330 core\n"
"out vec4 FragColor;\n\n"
"uniform sampler2D myTexture;\n"
"uniform bool u_HasTexture;\n\n"
"uniform vec2 u_EdgeMin;\n"
"uniform vec2 u_EdgeMax;\n\n"
"in vec4 fColor;\n"
"in vec2 fTexCoord;\n\n"
"void main()\n{\n"
"	if (gl_FragCoord.x >= u_EdgeMin.x && gl_FragCoord.y >= u_EdgeMin.y && \n"
"	gl_FragCoord.x <= u_EdgeMax.x && gl_FragCoord.y <= u_EdgeMax.y){\n"
"	if (u_HasTexture)\n"
"		FragColor = texture(myTexture, fTexCoord) * fColor;\n"
"	else\n"
"		FragColor = fColor;\n}\nelse{\n"
"	FragColor = vec4(0, 0, 0, 0);\n}"
"}\n";
// DYNAMIC TEXT shaders
constexpr std::string_view src_DT_Vertex =
"#version 330 core\n\n"
"uniform vec2 u_ParentPos;\n"
"uniform vec2 u_CharPos;\n"
"uniform vec2 u_Size;\n\n"
"uniform vec4 u_TextColor;\n"
"uniform vec4 u_BackColor;\n\n"
"layout(location = 0) in vec2 aPos;\n"
"layout(location = 1) in vec2 aTexCoord;\n"
"layout(location = 2) in vec4 aColor;\n\n"
"out vec4 fTextColor;\n"
"out vec4 fBackColor;\n"
"out vec2 fTexCoord;\n\n"
"void main()\n{\n"
"	fTextColor = aColor * u_TextColor;\n"
"	fBackColor = u_BackColor;\n"
"	fTexCoord = aTexCoord;\n\n"
"	// position transformations\n"
"	vec2 pos = aPos * u_Size;\n\n"
"	// character position\n"
"	pos += u_CharPos;\n\n"
"	// position\n"
"	pos += vec2(u_ParentPos.x, -u_ParentPos.y);\n\n"
"	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\n}";
constexpr std::string_view src_DT_Fragment =
"#version 330 core\n"
"out vec4 FragColor;\n\n"
"uniform sampler2D myTexture;\n\n"
"uniform vec2 u_EdgeMin;\n"
"uniform vec2 u_EdgeMax;\n"
"in vec4 fTextColor;\n"
"in vec4 fBackColor;\n"
"in vec2 fTexCoord;\n\n"
"void main()\n{\n"
"	if (gl_FragCoord.x >= u_EdgeMin.x && gl_FragCoord.y >= u_EdgeMin.y && \n"
"	gl_FragCoord.x <= u_EdgeMax.x && gl_FragCoord.y <= u_EdgeMax.y){\n"
"		float alpha = texture(myTexture, fTexCoord).a;\n"
"		FragColor = (fTextColor * alpha) + (fBackColor * (1 - alpha));\n}\nelse{\n"
"	FragColor = vec4(0, 0, 0, 0);\n}}";

Vec2 clamp(Vec2 v, Vec2 min, Vec2 max)
{ 
	return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) };
}
Vec2i clamp(Vec2i v, Vec2i min, Vec2i max)
{
	return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) };
}

Vec2 Vec2::Abs(Vec2 a)
{
	return { std::abs(a.x), std::abs(a.y) };
}
float Vec2::Distance(Vec2 a, Vec2 b)
{
	return sqrt(
		(a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)
	);
}

Vec2::operator Vec2i() const
{
	return { x, y };
}


Vec2i::operator Vec2() const
{
	return { x, y };
}

// Color definitions
const Color Color::Transparent(0, 0, 0, 0);
const Color Color::Black(0, 0, 0);
const Color Color::White(1, 1, 1);
const Color Color::Gray(0.5f, 0.5f, 0.5f);
const Color Color::Red(1, 0, 0);
const Color Color::Green(0, 1, 0);
const Color Color::Blue(0, 0, 1);
const Color Color::Purple(0.78f, 0.0f, 1.0f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f);
const Color Color::Brown(0.23f, 0.1f, 0.0f);

Color Color::FromHEX(std::string hex)
{
	if (hex[0] == '#' || hex[0] == '$') hex.erase(hex.begin());

	if (hex.size() != 6 && hex.size() != 8) {
		LibGui::Debug::Error("Color::FromHEX(), hex code provided isn't long 6/8 characters!");
		return Color{1, 1, 1};
	}

	// check if correct
	try
	{
		std::stoull(hex, nullptr, 16);
	}
	catch (std::invalid_argument)
	{
		LibGui::Debug::Error("Color::FromHEX(): invalid hex code! (#" + hex + ")");
		std::cout << std::endl;
		return {1, 1, 1};
	}

	// string to color channel
	auto quick_parse = [hex](const int i) {
		return static_cast<float>(
			stoi(
				hex.substr(i, 2),
				nullptr,
				16
			)
		)/255;
	};
	return {
		quick_parse(0),
		quick_parse(2),
		quick_parse(4),
		(hex.size()==8?quick_parse(6):1.0f)
	};
}

namespace LibGui {
	bool DebugRender = false;

	double DeltaTime = 0;
	double Framerate = 0;

	//------------------<GLOBAL VARIABLES>------------------
	GLFWwindow* MainWindow;
	Window* DefaultWindow;
	Shader DefaultShader;

	// Function definitions
	static void SetEdges(const Shader& shader, const Vec2 min, const Vec2 max)
	{
		int xs, ys;
		glfwGetWindowSize(glfwGetCurrentContext(), &xs, &ys);
		shader.Bind();
		glUniform2f(glGetUniformLocation(shader.GetID(), "u_EdgeMin"), (min.x == -1)? 0:min.x, (max.y == -1)?0 :ys - max.y);
		glUniform2f(glGetUniformLocation(shader.GetID(), "u_EdgeMax"), (max.x == -1)?xs:max.x, (min.y == -1)?ys:ys - min.y);
	}

	//------------------<TEXT FILE>-------------------
	TextFile::TextFile(const std::string& path, bool append)
	{
		file.open(path, std::ios::in | std::ios::out | (append ? (std::ios::app) : (std::ios::trunc)));
	}
	TextFile::~TextFile()
	{
		file.close();
	}

	void TextFile::Write(const std::string& data)
	{
		file << data;
	}

	std::string TextFile::Get()
	{
		std::string content;
		std::string str;
		while (std::getline(file, str))
		{
			content += str + '\n';
		}
		content.pop_back();
		return content;
	}

	//------------------<SHADER>-------------------
	bool Shader::CheckForCompilationErrors(unsigned int shader, bool isVertex)
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			compilationErrors.push_back((isVertex ? "VERTEX:" : "FRAGMENT:") + std::string(infoLog));
		}
		compilationSuccessful = success;
		return !success;
	}
	void Shader::Compile(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		compilationErrors.clear();
		compilationSuccessful = true;

		// compile vertex shader
		const char* vertexShaderSource = vertexSrc.c_str();
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
		glCompileShader(vertexShader);
		if (CheckForCompilationErrors(vertexShader, true))
		{
			Debug::Error("SHADER::VERTEX::COMPILATION_FAILED:" + compilationErrors[compilationErrors.size() - 1]);
		}

		// compile fragment shader
		const char* fragmentShaderSource = fragmentSrc.c_str();
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
		glCompileShader(fragmentShader);
		if (CheckForCompilationErrors(fragmentShader, false))
		{
			Debug::Error("SHADER::FRAGMENT::COMPILATION_FAILED:" + compilationErrors[compilationErrors.size() - 1]);
		}

		// link them into program
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);

		// check for link errors
		int  success;
		char infoLog[512];
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, nullptr, infoLog);
			compilationErrors.push_back("LINKER:" + std::string(infoLog));
			Debug::Error("ERROR::SHADER::LINKER_FAILED:" + std::string(infoLog));
		}

		// delete unnecessary garbage
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void Shader::CompileFromFile(const std::string& vertexPath, const std::string& fragmentPath)
	{
		Compile(TextFile(vertexPath).Get(), TextFile(fragmentPath).Get());
	}

	Shader::Shader()
		:id(0) {
	}
	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		CompileFromFile(vertexPath, fragmentPath);
	}
	Shader::~Shader()
	{
		glDeleteProgram(id);
	}

	void Shader::Bind() const
	{
		glUseProgram(id);
	}
	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetUniform1Int(const std::string& name, const int val) const
	{ Bind(); glUniform1i(glGetUniformLocation(id, name.c_str()), val); }
	void Shader::SetUniformVec2i(const std::string& name, const Vec2i val) const
	{ Bind(); glUniform2i(glGetUniformLocation(id, name.c_str()), val.x, val.y); }
	void Shader::SetUniform3Int(const std::string&  name, const int x, const int y, const int z) const
	{ Bind(); glUniform3i(glGetUniformLocation(id, name.c_str()), x, y, z); }
	void Shader::SetUniform4Int(const std::string&  name, const int x, const int y, const int z, const int w) const
	{ Bind(); glUniform4i(glGetUniformLocation(id, name.c_str()), x, y, z, w); }

	void Shader::SetUniform1UInt(const std::string& name, const unsigned int val) const
	{ Bind(); glUniform1ui(glGetUniformLocation(id, name.c_str()), val); }
	void Shader::SetUniform2UInt(const std::string& name, const unsigned int x, const unsigned int y) const
	{ Bind(); glUniform2ui(glGetUniformLocation(id, name.c_str()), x, y); }
	void Shader::SetUniform3UInt(const std::string& name, const unsigned int x, const unsigned int y, const unsigned int z) const
	{ Bind(); glUniform3ui(glGetUniformLocation(id, name.c_str()), x, y, z); }
	void Shader::SetUniform4UInt(const std::string& name, const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int w) const
	{ Bind(); glUniform4ui(glGetUniformLocation(id, name.c_str()), x, y, z, w); }

	void Shader::SetUniform1Float(const std::string& name, const float val) const
	{ Bind(); glUniform1f(glGetUniformLocation(id, name.c_str()), val); }
	void Shader::SetUniformVec2(const std::string& name, const Vec2 val) const
	{ Bind(); glUniform2i(glGetUniformLocation(id, name.c_str()), val.x, val.y); }
	void Shader::SetUniform3Float(const std::string& name, const float x, const float y, const float z) const
	{ Bind(); glUniform3i(glGetUniformLocation(id, name.c_str()), x, y, z); }
	void Shader::SetUniform4Float(const std::string& name, const float x, const float y, const float z, const float w) const
	{ Bind(); glUniform4i(glGetUniformLocation(id, name.c_str()), x, y, z, w); }

	unsigned int Shader::GetID() const
	{
		return id;
	}

	//------------------<VERTEX>--------------------
	Vertex::Vertex(const Vec2& Pos, const Color& Color)
		:pos(Pos), texCoord(Pos), color(Color) {
	}

	Vertex::Vertex(const Vec2& Pos, const Vec2& TexCoord, const Color& Color)
		:pos(Pos), texCoord(TexCoord), color(Color) {
	}

	//------------------<IMAGE>--------------------
	// PIXEL struct
	Color Pixel::ToColor() const
	{
		return Color{
			(R / 255.0f),
			(G / 255.0f),
			(B / 255.0f),
			(A / 255.0f)
		};
	}
	Pixel Pixel::FromColor(const Color& color)
	{
		return Pixel{
			static_cast<unsigned char>(color.r * 255.0f),
			static_cast<unsigned char>(color.g * 255.0f),
			static_cast<unsigned char>(color.b * 255.0f),
			static_cast<unsigned char>(color.a * 255.0f)
		};
	}

	// IMAGE class
	// CONSTRUCTOR functions
	bool Image::LoadFile(const std::string& path)
	{
		int nrChannels;
		unsigned char* Data = stbi_load(path.c_str(), &size.x, &size.y, &nrChannels, 4);

		data.resize(size.x * size.y);

		if (Data)
		{
			memcpy(data.data(), Data, size.x * size.y * 4 * sizeof(char));
		}
		else
		{
			Debug::Error("Failed to load texture on '" + path + "'!\n");
			return false;
		}
		stbi_image_free(Data);

		return true;
	}
	void Image::Clear(Color fillColor)
	{
		data.resize(size.x * size.y, Pixel::FromColor(fillColor));
	}
	void Image::Resize(Vec2i newSize, Color fillColor)
	{
		size = newSize;
		Clear(fillColor);
	}

	Image::Image()
		:size{ 2, 2 }
	{
		Clear(Color::Black);
		SetPixelColor({ 0, 0 }, Color::Purple);
		SetPixelColor({ 1, 1 }, Color::Purple);
	}
	Image::Image(const std::string& path)
	{
		LoadFile(path);
	}
	Image::Image(Vec2i Size, Color color)
		:size(Size)
	{
		Clear(color);
	}
	Image::Image(const unsigned char* Data, const Vec2i Size)
		:size(Size)
	{
		Clear(Color::White);

		const int content = size.x * size.y;

		int i = 0;
		while (i < content)
		{
			int x = i % size.x;
			int y = size.y - (i / size.x) - 1;

			data[i].A = Data[x + (y * size.x)];

			i++;
		}
	}

	// GET&&EDIT functions
	Pixel& Image::GetPixel(Vec2i pos)
	{
		return data[std::clamp(pos.x + (pos.y * size.x), 0, size.x * size.y - 1)];
	}
	Pixel& Image::GetPixel(size_t index)
	{
		return data[index];
	}

	void Image::SetPixelColor(Vec2i pos, Color color)
	{
		GetPixel(pos) = Pixel::FromColor(color);
	}
	Color Image::GetPixelColor(Vec2i pos)
	{
		return GetPixel(pos).ToColor();
	}
	void Image::ApplyEffect(void(*effect)(Pixel&, size_t))
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			effect(data[i], i);
		}
	}

	void Image::FlipXAxis()
	{
		for (int y = 0; y < size.y; y++)
		{
			std::reverse(data.begin() + y * size.x, data.begin() + (y + 1) * size.x);
		}
	}
	void Image::FlipYAxis()
	{
		std::vector<Pixel> n;
		n.reserve(data.size());

		for (int y = 0; y < size.y; y++)
		{
			n.insert(n.begin(), data.begin() + y * size.x, data.begin() + (y + 1) * size.x);
		}

		data.clear();
		data.insert(data.begin(), n.begin(), n.end());
	}

	void Image::RemoveCount(Direction_ side, int count)
	{
		switch (side)
		{
			case Direction_Up:
			{
				data.erase(data.end() - count * size.x, data.end());
				size.y -= count;
				break;
			}
			case Direction_Bottom:
			{
				data.erase(data.begin(), data.begin() + count * size.x);
				size.y -= count;
				break;
			}
			case Direction_Right:
			{
				for (int y = size.y; y > 0; y--)
				{
					data.erase(
						data.begin() + y * size.x - count,
						data.begin() + y * size.x
					);
				}
				size.x -= count;
				break;
			}
			case Direction_Left:
			{
				for (int y = size.y - 1; y >= 0; y--)
				{
					data.erase(
						data.begin() + y * size.x,
						data.begin() + y * size.x + count
					);
				}
				size.x -= count;
				break;
			}
		}
	}

	void Image::MergeImage(Direction_ side, Image& img, Color fill)
	{
		switch (side)
		{
			case Direction_Up:
			{
				if (size.x > img.size.x)
				{
					Image i(Vec2i{ size.x - img.size.x, img.size.y }, fill);
					img.MergeImage(Direction_Right, i);
				}
				else if (size.x < img.size.x)
				{
					Image i(Vec2i{ img.size.x - size.x, img.size.y }, fill);
					img.MergeImage(Direction_Right, i);
				}

				data.insert(data.end(), img.data.begin(), img.data.end());
				size.y += img.size.y;
				break;
			}
			case Direction_Bottom:
			{
				if (size.x > img.size.x)
				{
					Image i(Vec2i{ size.x - img.size.x, img.size.y }, fill);
					img.MergeImage(Direction_Right, i);
				}
				else if (size.x < img.size.x)
				{
					Image i(Vec2i{ img.size.x - size.x, img.size.y }, fill);
					img.MergeImage(Direction_Right, i);
				}

				data.insert(data.begin(), img.data.begin(), img.data.end());
				size.y += img.size.y;
				break;
			}
			case Direction_Right:
			{
				const std::vector<Pixel> clear(img.size.x, Pixel::FromColor(fill));
				for (int y = size.y; y > 0; y--)
				{
					if (y < img.size.y)
						data.insert(data.begin() + y * size.x, img.data.begin() + y * img.size.x, img.data.begin() + (y + 1) * img.size.x);
					else
						data.insert(data.begin() + y * size.x, clear.begin(), clear.end());
				}
				size.x += img.size.x;
				break;
			}
			case Direction_Left:
			{
				const std::vector<Pixel> clear(img.size.x, Pixel::FromColor(fill));
				for (int y = size.y - 1; y >= 0; y--)
				{
					if (y < img.size.y)
						data.insert(data.begin() + y * size.x, img.data.begin() + y * img.size.x, img.data.begin() + (y + 1) * img.size.x);
					else
						data.insert(data.begin() + y * size.x, clear.begin(), clear.end());
				}
				size.x += img.size.x;
				break;
			}
		}
	}
	void Image::MergeCount(Direction_ side, int count, Color color)
	{
		switch (side)
		{
			case Direction_Up:
			case Direction_Bottom:
			{
				Image img(Vec2i{ size.x, count }, color);
				MergeImage(side, img);
				break;
			}
			case Direction_Right:
			case Direction_Left:
			{
				Image img(Vec2i{ count, size.y }, color);
				MergeImage(side, img);
				break;
			}
		}
	}

	bool Image::SavePNG(const std::string& path) const
	{
		return stbi_write_png(path.c_str(), size.x, size.y, 4, data.data(), size.x * sizeof(Pixel));
	}
	bool Image::SaveJPG(const std::string& path, int quality) const
	{
		return stbi_write_jpg(path.c_str(), size.x, size.y, 4, data.data(), quality);
	}
	bool Image::SaveTGA(const std::string& path) const
	{
		return stbi_write_tga(path.c_str(), size.x, size.y, 4, data.data());
	}
	bool Image::SaveBMP(const std::string& path) const
	{
		return stbi_write_bmp(path.c_str(), size.x, size.y, 4, data.data());
	}

	unsigned char* Image::GetData()
	{
		return reinterpret_cast<unsigned char*>(data.data());
	}
	std::vector<Pixel> Image::GetDataCopy()
	{
		return data;
	}

	void Image::SetDataRaw(const unsigned char* Data, const Vec2i Size)
	{
		Resize(Size);
		memcpy(data.data(), Data, size.x * size.y * 4 * sizeof(char));
	}

	// used to pick format from count of channels in an textures
	constexpr GLenum ChannelRefTable[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
	//------------------<TEXTURE>------------------
	// CONSTRUCTOR functions
	bool Texture::LoadFile(const std::string& path)
	{
		unsigned char* data = stbi_load(path.c_str(), &size.x, &size.y, &nrChannels, 0);

		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, ChannelRefTable[nrChannels - 1], GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			Debug::Error("Failed to load texture on '" + path + "'");
			return false;
		}
		stbi_image_free(data);

		return true;
	}
	void Texture::LoadImage(Image& source)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, source.size.x, source.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		size = source.size;
	}

	Texture::Texture()
		:id(0) {
	}
	Texture::Texture(Image& source, bool smooth, unsigned int TextureSlot, bool clamp)
		:texSlot(TextureSlot)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp?GL_CLAMP_TO_BORDER:GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp?GL_CLAMP_TO_BORDER:GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST);

		if (clamp)
		{
			float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		LoadImage(source);
	}
	Texture::Texture(const std::string& path, bool smooth, unsigned int TextureSlot, bool clamp)
		:texSlot(TextureSlot)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp?GL_CLAMP_TO_BORDER:GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp?GL_CLAMP_TO_BORDER:GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST);

		if (clamp)
		{
			float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		LoadFile(path);
	}

	void Texture::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + texSlot);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int Texture::GetID() const
	{
		return id;
	}

	//------------------<MOUSE CURSOR>------------------
	void MouseButton::Update(int who_am_i, GLFWwindow* parent_window)
	{
		if (clicked)
			clicked = false;
		if (released)
			released = false;

		if (glfwGetMouseButton(parent_window, who_am_i) == GLFW_PRESS && !pressed)
			clicked = true;
		if (glfwGetMouseButton(parent_window, who_am_i) == GLFW_RELEASE && pressed)
			released = true;

		pressed = glfwGetMouseButton(parent_window, who_am_i) == GLFW_PRESS;
	}

	MouseCursor::MouseCursor()
		:parent_window(nullptr) {
	}
	MouseCursor::MouseCursor(GLFWwindow* parent)
		:parent_window(parent) {
	}

	void MouseCursor::Update()
	{
		if (parent_window == nullptr)
			Debug::Error("Trying to .Update() a non-initialized LibGui::MouseCursor");

		double xPos, yPos = 0;
		glfwGetCursorPos(parent_window, &xPos, &yPos);
		position = Vec2{ xPos, yPos };

		left_button.Update(GLFW_MOUSE_BUTTON_LEFT, parent_window);
		middle_button.Update(GLFW_MOUSE_BUTTON_MIDDLE, parent_window);
		right_button.Update(GLFW_MOUSE_BUTTON_RIGHT, parent_window);
	}

	void MouseCursor::SetLooks(const CursorShape_ shape) const
	{
		if (shape == CursorShape_Default)
		{
			glfwSetCursor(parent_window, nullptr);
		}
		GLFWcursor* cursor = glfwCreateStandardCursor(shape);
		glfwSetCursor(parent_window, cursor);
	}
	void MouseCursor::SetLooks(Image shape, const Vec2i pivot) const
	{
		GLFWimage image;
		image.width = shape.size.x; image.height = shape.size.y;
		image.pixels = shape.GetData();

		GLFWcursor* cursor = glfwCreateCursor(&image, pivot.x, pivot.y);
		glfwSetCursor(parent_window, cursor);
	}

	//------------------<KEY TRACKER>------------------
	// UPPERCASE FUNCTION - glfwGetKey() takes letter names only as UPPERCASE
	static int UpperKey(int key)
	{
		if (key >= 'a' && key <= 'z') key -= 32;
		return key;
	}

	// KEY TRACKER
	KeyTracker::KeyTracker()
		:parent_window(nullptr) {
	}
	KeyTracker::KeyTracker(GLFWwindow* parent)
		:parent_window(parent) {
	}

	void KeyTracker::Update()
	{
		if (parent_window == nullptr)
			Debug::Error("Trying to .Update() a non-initialized LibGui::KeyTracker");

		for (auto& [key, status] : tracked_keys)
		{
			if (status.clicked)
				status.clicked = false;
			if (status.released)
				status.released = false;

			if (glfwGetKey(parent_window, key) == GLFW_PRESS && !status.pressed)
				status.clicked = true;
			if (glfwGetKey(parent_window, key) == GLFW_RELEASE && status.pressed)
				status.released = true;

			status.pressed = glfwGetKey(parent_window, key) == GLFW_PRESS;
		}
	}

	void KeyTracker::Track(int key, bool update)
	{
		if (tracked_keys.contains(key)) return;

		key = UpperKey(key);
		tracked_keys.emplace(std::pair<int, KeyStatus>({ key, {false} }));

		if (update)
			Update();
	}
	void KeyTracker::Track(char key, bool update)
	{
		Track(static_cast<int>(key));
	}
	void KeyTracker::Track(FunctionalKey_ key, bool update)
	{
		Track(static_cast<int>(key));
	}
	void KeyTracker::Track(const std::vector<int>& keys)
	{
		for (const int key : keys)
		{
			Track(key);
		}
	}
	KeyStatus KeyTracker::operator[] (int key)
	{
		key = UpperKey(key);
		if (!tracked_keys.contains(key))
		{
			Debug::Error("Trying to use KeyTracker::operator[](int) to get non-tracked key; wanted key = " + std::to_string(key));
			return {};
		}

		return tracked_keys[key];
	}
	KeyStatus KeyTracker::operator[] (char key)
	{
		key = UpperKey(key);
		if (!tracked_keys.contains(key))
		{
			Debug::Error("Trying to use KeyTracker::operator[](char) to get non-tracked key; wanted key = " + std::to_string(key));
			return {};
		}

		return tracked_keys[key];
	}
	KeyStatus KeyTracker::operator[] (FunctionalKey_ key)
	{
		key = static_cast<FunctionalKey_>(UpperKey(key));
		if (!tracked_keys.contains(key))
		{
			Debug::Error("Trying to use KeyTracker::operator[](LibGui::FunctionalKey_) to get non-tracked key; wanted key = " + std::to_string(key));
			return {};
		}

		return tracked_keys[key];
	}

	//------------------<WINDOW>------------------
	static std::map<GLenum, std::string> gl_error_source_table =
	{
		{GL_DEBUG_SOURCE_API,             "API"},
		{GL_DEBUG_SOURCE_WINDOW_SYSTEM,   "Window System"},
		{GL_DEBUG_SOURCE_SHADER_COMPILER, "Shader Compiler"},
		{GL_DEBUG_SOURCE_THIRD_PARTY,     "Third Party"},
		{GL_DEBUG_SOURCE_APPLICATION,     "Application"},
		{GL_DEBUG_SOURCE_OTHER,           "Other"}
	};
	static std::map<GLenum, std::string> gl_error_type_table =
	{
		{GL_DEBUG_TYPE_ERROR,               "error"},
		{GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "deprecated behaviour"},
		{GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,  "undefined behaviour"},
		{GL_DEBUG_TYPE_PORTABILITY,         "portability"},
		{GL_DEBUG_TYPE_PERFORMANCE,         "performance"},
		{GL_DEBUG_TYPE_MARKER,              "marker"},
		{GL_DEBUG_TYPE_PUSH_GROUP,          "push group"},
		{GL_DEBUG_TYPE_POP_GROUP,           "pop group"},
		{GL_DEBUG_TYPE_OTHER,               "other"},
	};
	static std::map<GLenum, std::string> gl_error_severity_table =
	{
		{GL_DEBUG_SEVERITY_HIGH,         "HIGH"},
		{GL_DEBUG_SEVERITY_MEDIUM,       "MEDIUM"},
		{GL_DEBUG_SEVERITY_LOW,          "LOW"},
		{GL_DEBUG_SEVERITY_NOTIFICATION, "Notification"}
	};
	static void APIENTRY glDebugOutput(GLenum source,
							GLenum type,
							unsigned int id,
							GLenum severity,
							GLsizei length,
							const char *message,
							const void *userParam)
	{
		// ignore non-significant error/warning codes
		if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		const std::string out =	"(" + std::to_string(id) + ") (OpenGL " + gl_error_type_table[type] + ") from " + gl_error_source_table[source] + " with " + gl_error_severity_table[severity] + " severity:\n" +
								std::string(message);

		Debug::Error(out);
	}
	static void glfw_error_callback(int error_code, const char* description)
	{
		Debug::Error("(GLFW error)" + std::string(description) + "("+std::to_string(error_code)+")");
	}

	static bool gl_debug_build = false;

	static std::map<GLFWwindow*, Window*> window_pointer_map;
	static void PressedCharCallback(GLFWwindow* window, const unsigned int character)
	{
		window_pointer_map[window]->pressed_character = character;
	}
	static void MouseScrollCallback(GLFWwindow* window, const double x_offset, const double y_offset)
	{
		window_pointer_map[window]->cursor.scroll = y_offset;
	}
	Window::Window(std::string name, Vec2i scale, int InitFlags)
		:FullscreenOnF11(InitFlags & 1)
	{
		if ((InitFlags & 2) >> 1)
			glfwWindowHint(GLFW_RESIZABLE, false);

		if (gl_debug_build) glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		window = glfwCreateWindow(scale.x, scale.y, name.c_str(), nullptr, MainWindow);
		glfwSetCharCallback(window, PressedCharCallback);
		window_pointer_map.insert({window, this});
		glfwSetScrollCallback(window, MouseScrollCallback);

		size = { scale.x, scale.y };
		cursor = MouseCursor(window);
		key_tracker = KeyTracker(window);
		if (!window)
		{
			closed = true;
			Debug::FatalError("Failed to create window!");
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);

		if (FullscreenOnF11)
			key_tracker.Track(FunctionalKey_F11);
	}
	Window::Window(GLFWwindow* PreMadeWindow, std::string name, Vec2i scale, int InitFlags)
		:FullscreenOnF11(InitFlags & 1)
	{
		if (DefaultWindow == nullptr) DefaultWindow = this;
		window = PreMadeWindow;
		glfwSetCharCallback(window, PressedCharCallback);
		window_pointer_map.insert({window, this});
		glfwSetScrollCallback(window, MouseScrollCallback);

		glfwSetWindowTitle(window, name.c_str());
		glfwSetWindowSize(window, scale.x, scale.y);
		size = { scale.x, scale.y };
		cursor = MouseCursor(window);
		key_tracker = KeyTracker(window);

		if (FullscreenOnF11)
			key_tracker.Track(FunctionalKey_F11);
		if ((InitFlags & 2) >> 1)
			glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);
	}
	Window::~Window()
	{
		Close();
	}

	bool Window::GetKeyPressed(const int key) const
	{
		return glfwGetKey(window, UpperKey(key)) == GLFW_PRESS;
	}
	bool Window::GetKeyPressed(const char key) const
	{
		return GetKeyPressed(static_cast<int>(key));
	}
	bool Window::GetKeyPressed(const FunctionalKey_ key) const
	{
		return GetKeyPressed(static_cast<int>(key));
	}

	std::string Window::GetClipboard() const
	{
		return glfwGetClipboardString(window);
	}
	void Window::SetClipboard(const std::string& text) const
	{
		glfwSetClipboardString(window, text.c_str());
	}

	static double lastTick;
	void Window::Draw()
	{
		// Update library
		if (this == DefaultWindow)
		{
			ManualUpdate();
		}
		else if (GetLibTime() - 3.0f > lastTick) Debug::Warn("It looks like you haven't updated MainWindow for awhile, some features may break. See LibGui::ManualUpdate()");

		if (closed)
		{
			Debug::Warn("Trying to .Draw() a window that is already closed.");
			return;
		}

		pressed_character = 0;
		glfwPollEvents();
		if (CanAutoClose && ShouldClose())
		{
			Close();
			return;
		}

		cursor.Update();
		key_tracker.Update();

		if (FullscreenOnF11)
		{
			if (key_tracker[FunctionalKey_F11].clicked)
			{
				if (fullscreen)
					glfwSetWindowMonitor(window, nullptr, fPos.x, fPos.y, fSize.x, fSize.y, GLFW_DONT_CARE);
				else 
				{
					int len = 0;
					int wxp, wyp;
					glfwGetWindowPos(window, &wxp, &wyp);
					Vec2i w1{ wxp, wyp };
					Vec2i w2{ wxp + size.x, wyp + size.y };

					GLFWmonitor** monitors = glfwGetMonitors(&len);
					GLFWmonitor* final = glfwGetPrimaryMonitor();
					float best = 0;
					for (int i = 0; i < len; i++)
					{
						GLFWmonitor* monitor = monitors[i];
						int xp, yp, xs, ys;
						glfwGetMonitorWorkarea(monitor, &xp, &yp, &xs, &ys);
						Vec2i m1{ xp, yp };
						Vec2i m2{ xp + xs, yp + ys };

						// from: https://stackoverflow.com/questions/9324339/how-much-do-two-rectangles-overlap
						float IntersectionArea = std::max(0, std::min(w2.x, m2.x) - std::max(w1.x, m1.x)) * std::max(0, std::min(w2.y, m2.y) - std::max(w1.y, m1.y));
						float IntersectionRatio = IntersectionArea / (size.x * size.y);
						if (IntersectionRatio > best)
						{
							best = IntersectionRatio;
							final = monitor;
						}
					}

					int xp, yp, xs, ys;
					glfwGetMonitorWorkarea(final, &xp, &yp, &xs, &ys);
					glfwSetWindowMonitor(window, final, xp, yp, xs, ys, GLFW_DONT_CARE);
				}
				fullscreen = !fullscreen;
			}
		}

		glfwMakeContextCurrent(window);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, BackgroundColor.a);

		glfwSetWindowOpacity(window, transparency);

		int xs, ys = 0;
		glfwGetWindowSize(window, &xs, &ys);
		glViewport(-xs, 0, xs * 2, ys * 2);
		size = { xs, ys };
		if (!fullscreen)
		{
			int xp, yp; glfwGetWindowPos(window, &xp, &yp);
			fPos = { xp, yp };
			fSize = { xs, ys };
		}
	}
	void Window::PushDraw() const
	{
		if (closed)
		{
			Debug::Warn("Trying to .PushDraw() a window that is already closed.");
			return;
		}

		glfwSwapBuffers(window);
	}

	void Window::Close()
	{
		closed = true;
		window_pointer_map.erase(window);
		glfwDestroyWindow(window);
	}
	bool Window::ShouldClose()
	{
		closed = glfwWindowShouldClose(window);
		return closed;
	}

	void Window::Reflag(const int flags)
	{
		FullscreenOnF11 = flags & 1;

		if (FullscreenOnF11)
			key_tracker.Track(FunctionalKey_F11);
		if ((flags & 2) >> 1)
			glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);
	}

	void Window::SetIcon(Image img, bool clamp) const
	{
		if (clamp)
		{
			if (img.size.x > img.size.y)
				img.RemoveCount(Direction_Right, img.size.x - img.size.y);
			if (img.size.x < img.size.y)
				img.RemoveCount(Direction_Bottom, img.size.y - img.size.x);
		}

		img.FlipYAxis();

		GLFWimage i;
		i.width = img.size.x;
		i.height = img.size.y;
		i.pixels = img.GetData();
		glfwSetWindowIcon(window, 1, &i);
	}
	void Window::SetWindowsName(const std::string& name) const
	{
		glfwSetWindowTitle(window, name.c_str());
	}

	Image Window::Screenshot(const bool transparent) const
	{
		glfwMakeContextCurrent(window);

		const auto Data =
			static_cast<unsigned char*>(
				malloc(
					static_cast<size_t>(size.x * size.y * 4)
				)
			)
		;

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, Data);

		Image img;
		img.SetDataRaw(Data, size);
		if (!transparent) img.ApplyEffect([](Pixel& pixel, size_t index){pixel.A = 255;});

		return img;
	}

	void Window::Bind() const
	{
		if (closed)
		{
			Debug::Warn("Trying to .Bind() a window that is already closed.");
			return;
		}
		glfwMakeContextCurrent(window);
	}

	GLFWwindow* Window::GetID() const
	{
		return window;
	}

	//------------------<RECTANGLE INPUT FUNCTIONS>-------
	RectangleInput::~RectangleInput(){}

	bool RectangleInput::CompareMouseToThis(const Vec2 pos, const Vec2 scale, const float direction) const
	{
		// instead of rotating the block, i... rotate the mouse backwards
		Vec2 up = Vec2{ std::cos(radians(-direction)), std::sin(radians(-direction)) };
		Vec2 right = Vec2{ std::cos(radians(-direction + 90)), std::sin(radians(-direction + 90)) };

		Vec2 r_mouse = pos - context.cursor.position;
		r_mouse = right * r_mouse.x + up * r_mouse.y;
		r_mouse = r_mouse + pos;

		return
			std::abs(r_mouse.x - pos.x) < scale.x / 2 &&
			std::abs(r_mouse.y - pos.y) < scale.y / 2;
	}

	bool RectangleInput::Rect_OnMouseEnter(Vec2 anchor)
	{
		Debug::Warn("You're trying to use RectangleInput::Rect_OnMouseEnter; not possible -> just return false;");
		return false;
	}

	bool RectangleInput::Rect_OnMouseClick(Vec2 anchor)
	{
		return Rect_OnMouseEnter(anchor) && context.cursor.left_button.clicked;
	}
	bool RectangleInput::Rect_OnMouseDrag(Vec2 anchor)
	{
		return Rect_OnMouseEnter(anchor) && context.cursor.left_button.pressed;
	}
	bool RectangleInput::Rect_OnMouseRelease(Vec2 anchor)
	{
		return Rect_OnMouseEnter(anchor) && context.cursor.left_button.released;
	}

	Window& RectangleInput::GetContext() const
	{
		return context;
	}

	//------------------<SHAPES>--------------------------
	namespace Shapes
	{
		Shape Rectangle(const Vec2 anchor)
		{
			return Shape{
				{
					//point pos/						                    /tex coord/           /point tint/
					Vertex{Vec2{ 0.0f, -1.0f}+anchor,		Vec2{0, 0},     Color::White},
					Vertex{Vec2{ 0.0f,  0.0f}+anchor,		Vec2{0, 1},     Color::White},
					Vertex{Vec2{-1.0f, -1.0f}+anchor,		Vec2{1, 0},     Color::White},
					Vertex{Vec2{-1.0f,  0.0f}+anchor,		Vec2{1, 1},     Color::White}
				},
				{ 0, 1, 2, 1, 2, 3 }
			};
		};
		Shape Polygon(std::vector<Vertex>& vertices)
		{
			std::vector<unsigned int> indices = {};
			for (int x = 1; x < vertices.size() - 1; x++)
			{
				indices.push_back(0);
				indices.push_back(x);
				indices.push_back(x + 1);
			}

			return {vertices, indices};
		}
		Shape RegularPolygon(const int verticesCount)
		{
			std::vector<Vertex> vertices;
			for (int i = 0; i < verticesCount; i++)
			{
				// swapped sine & cosine to be rotated 'correctly' without changing direction
				vertices.push_back(Vertex{ Vec2{
					std::sin(radians(i * (360 / verticesCount))),
					std::cos(radians(i * (360 / verticesCount)))
				}, Color::White });
			}
			return Polygon(vertices);
		}
		Shape LineStrip(const std::vector<Vec2>& points)
		{
			std::vector<unsigned int> indices; indices.reserve(points.size());
			std::vector<Vertex> vertices; vertices.reserve(points.size());
			for (unsigned int i = 0; i < points.size(); i++)
			{
				vertices.emplace_back(Vec2{0, 0}-points[i], Color::White);
				indices.push_back(i);
			}

			return {vertices, indices};
		}
		Shape LineStrip(std::vector<Vertex> points)
		{
			std::vector<unsigned int> indices; indices.reserve(points.size());
			for (unsigned int i = 0; i < points.size(); i++)
			{
				points[i].pos = Vec2{0, 0}-points[i].pos;
				indices.push_back(i);
			}

			return {points, indices};
		}
	}

	//------------------<STATIC OBJECTS>------------------
	// CONSTRUCTOR functions
	void Object::Construct(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices) {
		context.Bind();

		if (VAO != 0)
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}

		indicesCount = static_cast<unsigned int>(Indices.size());

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(4 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	Object::Object(const Vec2 position, const Vec2 scale, const Color color, const Shapes::Shape& shape, const int UniformFlags)
		:RectangleInput(*DefaultWindow), pos(position), scale(scale), color(color), UniformFlags(UniformFlags), shader(DefaultShader)
	{
		Construct(shape.first, shape.second);
	}
	Object::Object(const Vec2 position, const Vec2 scale, Texture& texture, const Shapes::Shape& shape, const int UniformFlags)
		:RectangleInput(*DefaultWindow), pos(position), scale(scale), color(Color::White), UniformFlags(UniformFlags), shader(DefaultShader), texture(&texture)
	{
		Construct(shape.first, shape.second);
	}

	Object::Object(const Vec2 position, const Vec2 scale, const Color color, const Shapes::Shape& shape, Window& window, const int UniformFlags)
		:RectangleInput(window), pos(position), scale(scale), color(color), UniformFlags(UniformFlags), shader(DefaultShader)
	{
		Construct(shape.first, shape.second);
	}
	Object::Object(const Vec2 position, const Vec2 scale, Texture& texture, const Shapes::Shape& shape, Window& window, const int UniformFlags)
		:RectangleInput(window), pos(position), scale(scale), color(Color::White), UniformFlags(UniformFlags), shader(DefaultShader), texture(&texture)
	{
		Construct(shape.first, shape.second);
	}

	Object::Object(const Vec2 position, const Vec2 scale, const Color color, const Shapes::Shape& shape, Shader& shader, const int UniformFlags)
		:RectangleInput(*DefaultWindow), pos(position), scale(scale), color(color), UniformFlags(UniformFlags), shader(shader)
	{
		Construct(shape.first, shape.second);
	}
	Object::Object(const Vec2 position, const Vec2 scale, Texture& texture, const Shapes::Shape& shape, Shader& shader, const int UniformFlags)
		:RectangleInput(*DefaultWindow), pos(position), scale(scale), color(Color::White), UniformFlags(UniformFlags), shader(shader), texture(&texture)
	{
		Construct(shape.first, shape.second);
	}

	Object::Object(const Vec2 position, const Vec2 scale, const Color color, const Shapes::Shape& shape, Window& window, Shader& shader, const int UniformFlags)
		:RectangleInput(window), pos(position), scale(scale), color(color), UniformFlags(UniformFlags), shader(shader)
	{
		Construct(shape.first, shape.second);
	}
	Object::Object(const Vec2 position, const Vec2 scale, Texture& texture, const Shapes::Shape& shape, Window& window, Shader& shader, const int UniformFlags)
		:RectangleInput(window), pos(position), scale(scale), color(Color::White), UniformFlags(UniformFlags), shader(shader), texture(&texture)
	{
		Construct(shape.first, shape.second);
	}

	Object::~Object()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	// RENDER functions
	/*
	Returns 1D coord, based on 'normalized';
	If normalized just return 'x', else returns 'x'/'x_scale'
	*/
	constexpr static float GetCoord(float x, bool normalized, int x_size)
	{
		return x / (!normalized * static_cast<float>(x_size) + normalized);
	}
	void Object::SetupUniforms() const
	{
		bool scaleX = ((UniformFlags & UniformFlag_XScale_Normalized) >> 2);
		bool scaleY = ((UniformFlags & UniformFlag_YScale_Normalized) >> 3);
		glUniform2f(
			glGetUniformLocation(shader.GetID(), "u_ParentPos"),
			GetCoord(pos.x, (UniformFlags & UniformFlag_XPos_Normalized), context.size.x),
			GetCoord(pos.y, ((UniformFlags & UniformFlag_YPos_Normalized) >> 1), context.size.y)
		);
		glUniform2f(
			glGetUniformLocation(shader.GetID(), "u_RightVector"),
			std::cos(radians(direction + 90)) * GetCoord(scale.x, scaleX, context.size.x),
			std::sin(radians(direction + 90)) * GetCoord(scale.x, scaleX, context.size.y)
		);
		glUniform2f(
			glGetUniformLocation(shader.GetID(), "u_UpVector"),
			std::cos(radians(direction)) * GetCoord(scale.y, scaleY, context.size.x),
			std::sin(radians(direction)) * GetCoord(scale.y, scaleY, context.size.y)
		);

		glUniform4f(
			glGetUniformLocation(shader.GetID(), "u_Color"),
			color.r,
			color.g,
			color.b,
			color.a
		);

		glUniform1i(
			glGetUniformLocation(shader.GetID(), "u_HasTexture"),
			texture != nullptr
		);

		SetEdges(shader, LG_EdgeMin, LG_EdgeMax);
	}
	void Object::Render(bool custom_uniSetup) const
	{
		if (!custom_uniSetup)
		{
			shader.Bind();
			SetupUniforms();
			if (texture != nullptr) texture->Bind();
		}

		glPolygonMode(GL_FRONT_AND_BACK, DebugRender ? GL_LINE : GL_FILL);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(
			((UniformFlags & UniformFlag_RenderAsLine) >> 4)? GL_LINE_STRIP : GL_TRIANGLES,
			indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	// INTERACTION functions
	bool Object::Rect_OnMouseEnter(const Vec2 anchor)
	{
		// pos if anchor was on default (0.5, 0.5)
		Vec2 r_pos{ pos.x - (anchor.x - 0.5f) * scale.x, pos.y - (anchor.y - 0.5f) * scale.y };
		return CompareMouseToThis(r_pos, scale, direction);
	}

	void Object::GetBuffers(unsigned int& vao, unsigned int& vbo, unsigned int& ebo) const
	{
		vao = VAO; vbo = VBO; ebo = EBO;
	}

	//------------------<DYNAMIC TEXT>------------------
	// DYNAMIC TEXT
	DynamicText::DynamicText(DT_TextureAtlas& font, Vec2i position, Color text_color, Color background_color)
		:RectangleInput(*DefaultWindow),
		core(static_cast<Vec2>(position), { 0, 0 }, Color::White, Shapes::Rectangle({1, 1})),
		background(static_cast<Vec2>(position), { 0, 0 }, Color::Transparent, Shapes::Rectangle({ 0, 0 })),
		highlight({0, 0}, {10, 10}, Color::Transparent, Shapes::Rectangle({0, 1})),
		atlas(&font),
		pos(position),
		textColor(text_color),
		backColor(background_color){
	}
	DynamicText::DynamicText(Window& window, DT_TextureAtlas& font, Vec2i position, Color text_color, Color background_color)
		:RectangleInput(window),
		core(static_cast<Vec2>(position), { 0, 0 }, Color::White, Shapes::Rectangle({1, 1}), window),
		background(static_cast<Vec2>(position), { 0, 0 }, Color::Transparent, Shapes::Rectangle({ 0, 0 }), window),
		highlight({0, 0}, {10, 10}, Color::Transparent, Shapes::Rectangle({0, 1})),
		atlas(&font),
		pos(position),
		textColor(text_color),
		backColor(background_color){
	}

	void DynamicText::UpdateTextFieldSize(std::vector<DT_TextureAtlas*> fonts)
	{
		std::vector<DT_TextureAtlas*> font_stack = {atlas};

		Vec2i size{ 0.0f, atlas->maxHeight*fontSize }; float newX = 0;
		for (int i = 0; i < text.size(); ++i)
		{
			const char c = text[i];
			if (c == ' ')
				newX += 20.0f*fontSize;
			else if (c == '\n')
			{
				size.x = std::max(static_cast<float>(size.x), newX);
				size.y += atlas->maxHeight*fontSize;
				newX = 0;
			}
			else if (c == '\033')
			{
				// color
				if (text[i+1] == 'c' && text[i+2] == 'e')
					i += 2;
				else if (text[i+1] == 'c' && text[i+2] == '#')
					i += 8;
				else if (text[i+1] == 'c' && text[i+2] == '$')
					i += 10;

				// highlight
				if (text[i+1] == 'h' && text[i+2] == 'e')
					i += 2;
				else if (text[i+1] == 'h' && text[i+2] == '#')
					i += 8;
				else if (text[i+1] == 'h' && text[i+2] == '$')
					i += 10;

				else if (text[i+1] == 'f')
				{
					if (text[i+2] != 'e')
					{
						if (text[i+2] < 48 || text[i+2] > 57 || text[i+2]-48 < 0 || text[i+2]-48 >= fonts.size())
						{
							Debug::Error("DynamicText::UpdateTextFieldSize(): '\\033f" + std::to_string(text[i+2]-48) + "' invalid font formatting parameter");
							continue;
						}
						font_stack.push_back(fonts[text[i+2]-48]);
						i += 2;
					}
					else
					{
						if (font_stack.size() <= 1)
						{
							Debug::Error("DynamicText::UpdateTextFieldSize(): \\033fe operator without start.");
							continue;
						}
						font_stack.pop_back();
						i += 2;
					}
				}
			}
			else
			{
				newX += font_stack[font_stack.size()-1]->GetChar(c).advance * fontSize;
			}
		}
		size.x = std::max(static_cast<float>(size.x), newX);
		textYOffset = atlas->maxHeight * fontSize + backgroundAddSize.y*0.5f;
		textFieldSize = size;
	}

	void DynamicText::RenderHighlight(Vec2 position, Vec2 scale, Color color)
	{
		highlight.pos = position;
		highlight.scale = scale;
		highlight.color = color;

		highlight.Render();
	}
	float DynamicText::RenderChar(char character, Vec2 char_pos, Color clr, DT_TextureAtlas& font, const Color char_highlight)
	{
		dtS.Bind();
		Texture& texture = font.atlas[character].texture;

		glUniform2f(
			glGetUniformLocation(dtS.GetID(), "u_ParentPos"),
			GetCoord(pos.x, false, context.size.x),
			GetCoord(pos.y + textYOffset, false, context.size.y)
		);
		glUniform2f(
			glGetUniformLocation(dtS.GetID(), "u_CharPos"),
			GetCoord(char_pos.x+font.atlas[character].bearing.x*fontSize, false, context.size.x),
			GetCoord(char_pos.y-(texture.size.y-font.atlas[character].bearing.y)*fontSize, false, context.size.y)
		);

		glUniform2f(
			glGetUniformLocation(dtS.GetID(), "u_Size"),
			GetCoord(texture.size.x*fontSize, false, context.size.x),
			GetCoord(texture.size.y*fontSize, false, context.size.y)
		);

		glUniform4f(
			glGetUniformLocation(dtS.GetID(), "u_TextColor"),
			clr.r,
			clr.g,
			clr.b,
			clr.a
		);
		glUniform4f(
			glGetUniformLocation(dtS.GetID(), "u_BackColor"),
			charBackColor.r,
			charBackColor.g,
			charBackColor.b,
			charBackColor.a
		);

		// little function to do repetitive if-statements in code below
		auto shortIf = [](float A, float B, float POS)
		{
			if (A == -1) return B;
			if (B == -1) return A+POS;
			return std::min(POS + A, B);
		};
		SetEdges(dtS,
			{std::max(static_cast<float>(pos.x), LG_EdgeMin.x), std::max(static_cast<float>(pos.y), LG_EdgeMin.y)},
			{shortIf(maxSize.x, LG_EdgeMax.x, pos.x), shortIf(maxSize.y, LG_EdgeMax.y, pos.y)}
		);

		texture.Bind();
		core.Render(true);

		if (char_highlight == Color::Transparent)  return font.atlas[character].advance*fontSize;

		RenderHighlight(pos + Vec2{char_pos.x, -char_pos.y + static_cast<float>(textYOffset)+font.maxHeight*0.1f},
		Vec2{font.atlas[character].advance*fontSize, static_cast<float>(font.maxHeight)},
		char_highlight);

		return font.atlas[character].advance*fontSize;
	}

	void DynamicText::UpdateTFSandBackground(std::vector<DT_TextureAtlas*> fonts)
	{
		UpdateTextFieldSize(fonts);

		const Vec2 ns{ static_cast<Vec2>(textFieldSize) + backgroundAddSize };

		if (minSize.x != -1 && ns.x < minSize.x)
			background.scale.x = minSize.x + backgroundAddSize.x;
		else if (maxSize.x != -1 && ns.x > maxSize.x)
			background.scale.x = maxSize.x + backgroundAddSize.x;
		else
			background.scale.x = ns.x;

		if (minSize.y != -1 && ns.y < minSize.y)
			background.scale.y = minSize.y + backgroundAddSize.y;
		else if (maxSize.y != -1 && ns.y > maxSize.y)
			background.scale.y = maxSize.y + backgroundAddSize.y;
		else
			background.scale.y = ns.y;

		background.pos = pos + backgroundAddSize * Vec2{-0.5f, 0.5f};
	}

	void DynamicText::Render(std::vector<DT_TextureAtlas*> fonts, Vec2 selection)
	{
		background.color = backColor;

		glPolygonMode(GL_FRONT_AND_BACK, DebugRender ? GL_LINE : GL_FILL);

		UpdateTFSandBackground(fonts);
		charBackColor = backColor;
		background.Render();

		std::vector<Color> color_stack = {textColor};
		std::vector<Color> high_stack = {Color::Transparent};
		std::vector<DT_TextureAtlas*> font_stack = {atlas};

		int srcI = 0;
		Vec2i char_pos{0, 0};
		for (int i = 0; i < text.size(); i++)
		{
			if (const char c = text[i]; c == ' ')
			{
				RenderHighlight(pos + Vec2{static_cast<float>(char_pos.x), -char_pos.y + static_cast<float>(textYOffset)+font_stack[font_stack.size()-1]->maxHeight*0.1f},
				Vec2{20.0f, static_cast<float>(font_stack[font_stack.size()-1]->maxHeight)},
				(srcI < selection.y && srcI >= selection.x)?
					selectionColor:high_stack[high_stack.size()-1]);
				char_pos.x += 20.0f*fontSize;
				srcI++;
			}
			else if (c == '\n')
			{
				RenderHighlight(pos + Vec2{static_cast<float>(char_pos.x), -char_pos.y + static_cast<float>(textYOffset)+font_stack[font_stack.size()-1]->maxHeight*0.1f},
				Vec2{8.0f, static_cast<float>(font_stack[font_stack.size()-1]->maxHeight)},
				(srcI < selection.y && srcI >= selection.x)?
					selectionColor:high_stack[high_stack.size()-1]);
				char_pos = Vec2i{ 0.0f, char_pos.y - atlas->maxHeight*fontSize };
				srcI++;
			}
			else if (c == '\033')
			{
				if (text[i+1] == 'c')
				{
					if (text[i+2] == '#')
					{
						color_stack.push_back(Color::FromHEX(text.substr(i+2, 7)));
						i += 8;
					}
					else if (text[i+2] == '$')
					{
						color_stack.push_back(Color::FromHEX(text.substr(i+2, 9)));
						i += 10;
					}
					else if (text[i+2] == 'e')
					{
						if (color_stack.size() <= 1)
						{
							Debug::Error("DynamicText::Render(): \\033ce operator without start.");
							continue;
						}
						color_stack.pop_back();
						i += 2;
					}
				}
				else if (text[i+1] == 'h')
				{
					if (text[i+2] == '#')
					{
						high_stack.push_back(Color::FromHEX(text.substr(i+2, 7)));
						i += 8;
					}
					else if (text[i+2] == '$')
					{
						high_stack.push_back(Color::FromHEX(text.substr(i+2, 9)));
						i += 10;
					}
					else if (text[i+2] == 'e')
					{
						if (high_stack.size() <= 1)
						{
							Debug::Error("DynamicText::Render(): \\033he operator without start.");
							continue;
						}
						high_stack.pop_back();
						i += 2;
					}
				}
				else if (text[i+1] == 'f')
				{
					if (text[i+2] != 'e')
					{
						if (text[i+2] < 48 || text[i+2] > 57 || text[i+2]-48 < 0 || text[i+2]-48 >= fonts.size())
						{
							Debug::Error("DynamicText::Render(): '\\033f" + std::to_string(text[i+2]-48) + "' invalid font formatting parameter");
							continue;
						}
						font_stack.push_back(fonts[text[i+2]-48]);
						i += 2;
					}
					else
					{
						if (font_stack.size() <= 1)
						{
							Debug::Error("DynamicText::Render(): \\033fe operator without start.");
							continue;
						}
						font_stack.pop_back();
						i += 2;
					}
				}
			}
			else
			{
				char_pos.x += RenderChar(c, char_pos, color_stack[color_stack.size()-1], *font_stack[font_stack.size()-1],
				(srcI < selection.y && srcI >= selection.x)?
					selectionColor:high_stack[high_stack.size()-1]);
				srcI++;
			}
		}
	}

	Vec2 DynamicText::GetFinalSize(std::vector<DT_TextureAtlas*> fonts)
	{
		UpdateTFSandBackground(fonts);
		return background.scale;
	}

	bool DynamicText::Rect_OnMouseEnter(Vec2 anchor)
	{
		return background.Rect_OnMouseEnter({0, 0});
	}

	int DynamicText::SourceIndexToRichTextIndex(int in) const
	{
		int srcI = 0;
		int richI = 0;
		while (srcI < in) {
			if (text[richI] == '\033')
			{
				// color
				if (text[richI+1] == 'c' && text[richI+2] == 'e')
					richI += 2;
				else if (text[richI+1] == 'c' && text[richI+2] == '#')
					richI += 8;
				else if (text[richI+1] == 'c' && text[richI+2] == '$')
					richI += 10;

				// highlight
				if (text[richI+1] == 'h' && text[richI+2] == 'e')
					richI += 2;
				else if (text[richI+1] == 'h' && text[richI+2] == '#')
					richI += 8;
				else if (text[richI+1] == 'h' && text[richI+2] == '$')
					richI += 10;

				// any font case
				else if (text[richI+1] == 'f')
					richI += 2;
			}
			else
				srcI++;
			richI++;
		}

		return richI;
	}
	int DynamicText::RichTextIndexToSourceIndex(int in) const
	{
		int srcI = 0;
		int richI = 0;

		while (richI < in)
		{
			if (text[richI] == '\033')
			{
				// color
				if (text[richI+1] == 'c' && text[richI+2] == 'e')
					richI += 2;
				else if (text[richI+1] == 'c' && text[richI+2] == '#')
					richI += 8;
				else if (text[richI+1] == 'c' && text[richI+2] == '$')
					richI += 10;

				// highlight
				if (text[richI+1] == 'h' && text[richI+2] == 'e')
					richI += 2;
				else if (text[richI+1] == 'h' && text[richI+2] == '#')
					richI += 8;
				else if (text[richI+1] == 'h' && text[richI+2] == '$')
					richI += 10;

				// any font case
				else if (text[richI+1] == 'f')
					richI += 2;
			}
			else
				srcI++;
			richI++;
		}

		return srcI;
	}

	int DynamicText::GetTouchedCharIndex(std::vector<DT_TextureAtlas*> fonts)
	{
		UpdateTFSandBackground(fonts);
		Vec2i char_pos{0, 0};
		int desiredLine = -std::floor((pos.y - context.cursor.position.y) / (atlas->maxHeight * fontSize)) - 1;
		int i = 0;
		for (;i < text.size() && desiredLine > 0; ++i)
		{
			if (text[i] == '\n') desiredLine--;
		}

		std::vector<DT_TextureAtlas*> font_stack = {atlas};

		// 99999 is just a very high number
		float best = 99999; int index = i;
		for (; i < text.size() && text[i] != '\n'; ++i)
		{
			char c = text[i];
			if (c == ' ')
				char_pos.x += 20.0f*fontSize;
			else if (c == '\033')
			{
				if (text[i+1] == 'c' && text[i+2] == '#')
					i += 8;
				else if (text[i+1] == 'c' && text[i+2] == '$')
					i += 10;
				else if (text[i+1] == 'c' && text[i+2] == 'e')
					i += 2;

				if (text[i+1] == 'h' && text[i+2] == '#')
					i += 8;
				else if (text[i+1] == 'h' && text[i+2] == '$')
					i += 10;
				else if (text[i+1] == 'h' && text[i+2] == 'e')
					i += 2;

				else if (text[i+1] == 'f')
				{
					if (text[i+2] != 'e')
					{
						if (text[i+2] < 48 || text[i+2] > 57 || text[i+2]-48 < 0 || text[i+2]-48 >= fonts.size())
						{
							Debug::Error("DynamicText::GetTouchedCharIndex(): '\\033f" + std::to_string(text[i+2]-48) + "' invalid font formatting parameter");
							continue;
						}
						font_stack.push_back(fonts[text[i+2]-48]);
						i += 2;
					}
					else
					{
						if (font_stack.size() <= 1)
						{
							Debug::Error("DynamicText::GetTouchedCharIndex(): \\033fe operator without start.");
							continue;
						}
						font_stack.pop_back();
						i += 2;
					}
				}
			}
			else
				char_pos.x += font_stack[font_stack.size()-1]->GetChar(c).advance*fontSize;

			const float d = std::abs(pos.x + char_pos.x - font_stack[font_stack.size()-1]->GetCharSize(c).x - context.cursor.position.x);
			if (d < best)
			{
				best = d;
				index = i;
			}
		}

		return RichTextIndexToSourceIndex(index);
	}
	Vec2 DynamicText::GetPositionOnIndex(int character_index, bool left_side, std::vector<DT_TextureAtlas*> fonts) const
	{
		character_index = SourceIndexToRichTextIndex(character_index);

		std::vector<DT_TextureAtlas*> font_stack = {atlas};

		Vec2i char_pos{ 0, 0 };
		for (int i = 0; i < text.size(); ++i)
		{
			const char c = text[i];
			if (i == character_index && left_side) return char_pos + pos;
			if (i-1 == character_index && !left_side) return char_pos + pos;
			if (c == ' ')
				char_pos.x += 20.0f*fontSize;
			else if (c == '\n')
				char_pos = Vec2i{ 0.0f, char_pos.y + atlas->maxHeight*fontSize };
			else if (c == '\033')
			{
				if (text[i+1] == 'c' && text[i+2] == '#')
					i += 8;
				if (text[i+1] == 'c' && text[i+2] == '$')
					i += 10;
				else if (text[i+1] == 'c' && text[i+2] == 'e')
					i += 2;

				if (text[i+1] == 'h' && text[i+2] == '#')
					i += 8;
				else if (text[i+1] == 'h' && text[i+2] == '$')
					i += 10;
				else if (text[i+1] == 'h' && text[i+2] == 'e')
					i += 2;

				else if (text[i+1] == 'f')
				{
					if (text[i+2] != 'e')
					{
						if (text[i+2] < 48 || text[i+2] > 57 || text[i+2]-48 < 0 || text[i+2]-48 >= fonts.size())
						{
							Debug::Error("DynamicText::GetPositionOnIndex(): '\\033f" + std::to_string(text[i+2]-48) + "' invalid font formatting parameter");
							continue;
						}
						font_stack.push_back(fonts[text[i+2]-48]);
						i += 2;
					}
					else
					{
						if (font_stack.size() <= 1)
						{
							Debug::Error("DynamicText::GetPositionOnIndex(): \\033fe operator without start.");
							continue;
						}
						font_stack.pop_back();
						i += 2;
					}
				}
			}
			else
				char_pos.x += font_stack[font_stack.size()-1]->GetChar(c).advance*fontSize;
		}

		return char_pos + pos;
	}

	// DYNAMIC TEXT TEXTURE ATLAS
	DT_TextureAtlas::DT_TextureAtlas(const std::string& source, unsigned int quality, bool pixelated)
		:maxHeight(quality)
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			Debug::FatalError("Failed to load FreeType.");
		}

		FT_Face face;
		if (FT_New_Face(ft, source.c_str(), 0, &face))
		{
			Debug::FatalError("Failed to load font at '" + source + "'.");
		}

		FT_Set_Pixel_Sizes(face, 0, quality);

		FT_UInt index;
		int c = FT_Get_First_Char(face, &index);

		while (index && c < 256) {
			FT_Load_Char(face, c, FT_LOAD_RENDER);

			Image img(face->glyph->bitmap.buffer, { face->glyph->bitmap.width, face->glyph->bitmap.rows});
			img.FlipXAxis();
			atlas.insert(std::pair{ c,
				DT_Character{Texture(img, !pixelated, 0, true), {face->glyph->bitmap_left, face->glyph->bitmap_top}, static_cast<unsigned int>(face->glyph->advance.x)>>6}
			});
			c = FT_Get_Next_Char(face, c, &index);
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	DT_Character DT_TextureAtlas::GetChar(char character)
	{
		return atlas[character];
	}
	Vec2i DT_TextureAtlas::GetCharSize(char character)
	{
		return atlas[character].texture.size;
	}

	//------------------<TEXT INPUT>-------------------------
	TextInput::TextInput(DT_TextureAtlas& font)
		:TextInput(*DefaultWindow, font){
	}
	TextInput::TextInput(Window& window, DT_TextureAtlas& font)
		:RectangleInput(window), render(window, font, {0, 0}),
		textCursor({0, 0}, {5, font.maxHeight}, Color::Red, Shapes::Rectangle({0, 0}), window)
	{
		backColor = Color{0.1f, 0.1f, 0.1f};

		window.key_tracker.Track({
			FunctionalKey_Escape,
			FunctionalKey_Enter,
			FunctionalKey_Backspace,
			FunctionalKey_Delete,
			FunctionalKey_Up,
			FunctionalKey_Down,
			FunctionalKey_Right,
			FunctionalKey_Left,
			FunctionalKey_Left_Control, 'X', 'C', 'V'
		});
	}

	void TextInput::Update(bool render_afterwards)
	{
		auto clear_selection_cache = [this]{select_start = -1; select_end = -1; back_text_cursor_start = -1;};
		// on click on text => activate
		if (Rect_OnMouseClick())
		{
			active = true;
			// if render.GetTouchedCharIndex() returns -1, set to 0
			cursor_pos = std::max(0, render.GetTouchedCharIndex(rich_text_font_list));
			text_cursor_tick_time = 0;

			if (select_start != -1 && select_end != -1)
				clear_selection_cache();

			select_start = cursor_pos;
			back_text_cursor_start = cursor_pos;
		}
		else if (Rect_OnMouseDrag() && select_start != -1)
		{
			if (const int new_pos = render.GetTouchedCharIndex(rich_text_font_list);
				new_pos > select_start)
			{
				select_end = new_pos;
				select_start = back_text_cursor_start;
			}
			else
			{
				select_end = back_text_cursor_start;
				select_start = new_pos;
			}
		}
		else if (Rect_OnMouseRelease() && select_start == select_end)
			clear_selection_cache();

		// on click outside of text => deactivate
		if (!Rect_OnMouseEnter() && context.cursor.left_button.clicked)
			active = false;

		if (!active)
		{
			if (render_afterwards) Render();
			return;
		}

		if (context.pressed_character != 0)
		{
			if (select_start != -1)
			{
				text.erase(text.begin()+select_start, text.begin()+select_end);
				text.insert(text.begin() + select_start, context.pressed_character);
				cursor_pos = static_cast<int>(std::clamp(select_start + 1, 0, static_cast<int>(text.size())));
				clear_selection_cache();
			}
			else
			{
				text.insert(text.begin() + cursor_pos, context.pressed_character);
				cursor_pos = static_cast<int>(std::clamp(cursor_pos + 1, 0, static_cast<int>(text.size())));
			}
			text_cursor_tick_time = 0;
		}
		else if (context.key_tracker[FunctionalKey_Enter].clicked)
		{
			if (select_start != -1)
			{
				text.erase(text.begin()+select_start, text.begin()+select_end);
				text.insert(text.begin() + select_start, '\n');
				cursor_pos = static_cast<int>(std::clamp(select_start + 1, 0, static_cast<int>(text.size())));
				clear_selection_cache();
			}
			else
			{
				text.insert(text.begin() + cursor_pos, '\n');
				cursor_pos = static_cast<int>(std::clamp(cursor_pos + 1, 0, static_cast<int>(text.size())));
			}
				text_cursor_tick_time = 0;
		}
		else if (context.key_tracker[FunctionalKey_Backspace].clicked && text.length() > 0 && (cursor_pos > 0||select_start != -1)) {
			if (select_start != -1)
			{
				cursor_pos = select_start;
				text.erase(text.begin() + select_start, text.begin() + select_end);
				text_cursor_tick_time = 0;
				clear_selection_cache();
			}
			else
			{
				cursor_pos = static_cast<int>(std::clamp(cursor_pos - 1, 0, static_cast<int>(text.size())));
				text.erase(text.begin() + cursor_pos);
				text_cursor_tick_time = 0;
			}
		}
		else if (context.key_tracker[FunctionalKey_Delete].clicked && text.length() > 0) {
			if (select_start != -1)
			{
				cursor_pos = select_start;
				text.erase(text.begin() + select_start, text.begin() + select_end);
				text_cursor_tick_time = 0;
				clear_selection_cache();
			}
			else if (cursor_pos < text.size())
			{
				text.erase(text.begin() + cursor_pos);
				text_cursor_tick_time = 0;
			}
		}
		else if (context.key_tracker[FunctionalKey_Up].clicked)
		{
			for (;cursor_pos > 0 && text[cursor_pos] != '\n'; --cursor_pos){}
			cursor_pos = std::clamp(cursor_pos-1, 0, static_cast<int>(text.size()));
			for (;cursor_pos >= 0 && text[cursor_pos] != '\n'; --cursor_pos){}
			cursor_pos++;
			text_cursor_tick_time = 0;
			clear_selection_cache();
		}
		else if (context.key_tracker[FunctionalKey_Down].clicked)
		{
			for (;cursor_pos < text.size() && text[cursor_pos] != '\n'; ++cursor_pos){}
			cursor_pos = std::clamp(cursor_pos+1, 0, static_cast<int>(text.size()));
			text_cursor_tick_time = 0;
			clear_selection_cache();
		}
		else if (context.key_tracker[FunctionalKey_Right].clicked)
		{
			cursor_pos = static_cast<int>(std::clamp(cursor_pos + 1, 0, static_cast<int>(text.size())));
			text_cursor_tick_time = 0;
			clear_selection_cache();
		}
		else if (context.key_tracker[FunctionalKey_Left].clicked)
		{
			cursor_pos = static_cast<int>(std::clamp(cursor_pos - 1, 0, static_cast<int>(text.size())));
			text_cursor_tick_time = 0;
			clear_selection_cache();
		}
		else if (context.key_tracker[FunctionalKey_Left_Control].pressed)
		{
			if (context.key_tracker['C'].clicked && select_start != -1)
				context.SetClipboard(text.substr(select_start, select_end-select_start));
			else if (context.key_tracker['X'].clicked && select_start != -1)
			{
				context.SetClipboard(text.substr(select_start, select_end-select_start));
				cursor_pos = select_start;
				text.erase(text.begin() + select_start, text.begin() + select_end);
				text_cursor_tick_time = 0;
				clear_selection_cache();
			}
			else if (context.key_tracker['V'].clicked)
			{
				if (select_start != -1)
				{
					cursor_pos = select_start;
					text.erase(text.begin() + select_start, text.begin() + select_end);
					text_cursor_tick_time = 0;
					clear_selection_cache();
				}
				text.insert(cursor_pos, context.GetClipboard());
			}
		}

		if (render_afterwards) Render();
	}
	void TextInput::Render()
	{
		if (preprocessText != nullptr)	render.text = preprocessText(text);
		else							render.text = text;
		render.pos = pos;
		
		render.Render(rich_text_font_list, active?Vec2{select_start, select_end}:Vec2{-1, -1});
		text_cursor_tick_time += DeltaTime;
		if (active && static_cast<int>(std::floor(text_cursor_tick_time / textCursorTickRate)) % 2 == 0 &&
			cursor_pos >= 0 && cursor_pos <= render.text.size())
		{
			textCursor.pos = render.GetPositionOnIndex(cursor_pos, true, rich_text_font_list) + Vec2{ -(render.atlas->maxHeight*fontSize)/15.0f, 10.0f };
			textCursor.scale = Vec2{5, render.atlas->maxHeight} * fontSize;
			textCursor.Render();
		}
	}

	bool TextInput::Rect_OnMouseEnter(Vec2 anchor)
	{
		return render.Rect_OnMouseEnter();
	}

	//------------------<GRAPH>------------------------------
	Graph::Graph(Vec2 pos, Vec2 desSize, std::vector<float>* data_source, DT_TextureAtlas& font)
		:RectangleInput(*DefaultWindow),
		lines({0, 0}, {0, 0}, Color::White, {}, UniformFlag_RenderAsLine),
		background({0, 0}, {0, 0}, {0.1f, 0.1f, 0.1f}, Shapes::Rectangle({0, 1})),
		info_line({0, 0}, {1, 1}, { 1.0f, 1.0f, 1.0f, 0.5f }, Shapes::LineStrip({{0, 0}, {1, 0}}), UniformFlag_RenderAsLine),
		text(font, {0, 0}),
		data_source(data_source),
		position(pos),
		desired_size(desSize)
	{
		lines.UniformFlags |= UniformFlag_RenderAsLine;

		text.fontSize = 0.2f;
		text.backgroundAddSize = { 0, 0 };

		Update();
	}
	Graph::Graph(Window& window, Vec2 pos, Vec2 desSize, std::vector<float>* data_source, DT_TextureAtlas& font)
		:RectangleInput(window),
		lines({0, 0}, {0, 0}, Color::White, {}, window, UniformFlag_RenderAsLine),
		background({0, 0}, {0, 0}, {0.1f, 0.1f, 0.1f}, Shapes::Rectangle({0, 1}), window),
		info_line({0, 0}, {1, 1}, { 1.0f, 1.0f, 1.0f, 0.5f }, Shapes::LineStrip({{0, 0}, {1, 0}}), UniformFlag_RenderAsLine),
		text(window, font, {0, 0}),
		data_source(data_source),
		position(pos),
		desired_size(desSize)
	{
		text.fontSize = 0.2f;
		text.backgroundAddSize = { 0, 0 };

		Update();
	}

	void Graph::Update()
	{
		std::vector<Vertex> points; points.reserve(data_source->size()+1);
		std::vector<unsigned int> indices; indices.reserve(data_source->size() + 1);
		indices.push_back(0); points.push_back({ { 0, 0 }, Color::White });
		biggest = 0;
		for (int i = 0; i < data_source->size(); i++)
		{
			indices.push_back(i+1);
			points.push_back(
				{ { (-i-1) / static_cast<float>(data_source->size()), data_source->at(i)},
				Color::White});

			if (data_source->at(i) > biggest) biggest = data_source->at(i);
		}
		lines.Construct(points, indices);
	}

	void Graph::Render()
	{
		const float PixPerUnit = desired_size.y / biggest;
		text.text = std::to_string(static_cast<int>(biggest)); text.UpdateTextFieldSize();
		const int xOffset = anyText?text.textFieldSize.x:0;

		finalSize = (
			desired_size +
			Vec2{
				static_cast<float>(text.textFieldSize.x)  + textLeftOffset,
				text.atlas->maxHeight * fontSize
			}*anyText +
			background_padding
		)*scale;

		background.scale = finalSize;
		background.pos = position + background_padding * (anyText ? Vec2{ -0.5f, 0.5f } : Vec2{ 0.0f, 0.5f }) * scale + finalSize * Vec2{-anchor.x, anchor.y};
		background.color = backgroundColor;

		lines.scale = (Vec2{desired_size.x, PixPerUnit})* scale;
		lines.pos = Vec2{ position.x + xOffset + textLeftOffset * scale, position.y }+finalSize * Vec2{ -anchor.x, anchor.y };
		lines.color = lineColor;
		
		text.fontSize = fontSize*scale;
		text.textColor = textColor;

		info_line.color = infoLineColor;
		info_line.scale = Vec2{ desired_size.x*scale, 1.0f };

		background.Render();
		if (infoLineYDelta != 0)
		{
			for (float y = 0; y <= biggest; y += infoLineYDelta)
			{
				info_line.pos = Vec2{ position.x + xOffset + textLeftOffset * scale, position.y - y * PixPerUnit * scale } + finalSize * Vec2{ -anchor.x, anchor.y };
				info_line.Render();
				if (anyText)
				{
					text.text = std::to_string(static_cast<int>(y));
					text.UpdateTextFieldSize();
					text.pos = Vec2{ position.x + xOffset - (text.textFieldSize.x / 2) * scale, position.y - (y * PixPerUnit) * scale } + finalSize * Vec2{ -anchor.x, anchor.y } - Vec2{0, text.textFieldSize.y};
					text.Render();
				}
			}
		}
		lines.Render();
	}

	bool Graph::Rect_OnMouseEnter(Vec2 anchor_)
	{
		// pos if anchor was on default (0.5, 0.5)
		Vec2 r_pos = position + background_padding * (anyText ? Vec2{ -0.5f, 0.5f } : Vec2{ 0.0f, 0.5f }) * scale + finalSize * Vec2{ 0.5f, -0.5f };

		return CompareMouseToThis(r_pos, finalSize, 90); // can be problem
	}

	//------------------<LIBRARY FUNCTIONS>------------------
	GLFWwindow* Init(bool debug_build)
	{
		gl_debug_build = debug_build;

		glfwSetErrorCallback(glfw_error_callback);
		// Initialize glfw
		if (!glfwInit())
			Debug::FatalError("GLFW failed to initialize!");

		// Create window, and context with it
		if (gl_debug_build) glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		MainWindow = glfwCreateWindow(100, 100, "MAIN", nullptr, nullptr);
		if (!MainWindow)
		{
			Debug::FatalError("Failed to create initialization window!");
			glfwTerminate();
			return nullptr;
		}
		glfwMakeContextCurrent(MainWindow);

		// Initialize glad (OpenGL)
		gladLoadGL();
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			Debug::FatalError("Failed to initialize GLAD");
			return nullptr;
		}

		if (debug_build)
		{
			int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
			if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
				glDebugMessageCallback(glDebugOutput, nullptr);
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			}
		}

		// Set up OpenGL
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Set up stb_image and stb_image_write
		stbi_set_flip_vertically_on_load(true);
		stbi_flip_vertically_on_write(true);

		// Compile DefaultShader
		DefaultShader = Shader();
		DefaultShader.Compile(std::string(src_defVertex), std::string(src_defFragment));

		dtS.Compile(std::string(src_DT_Vertex), std::string(src_DT_Fragment));

		return MainWindow;
	}

	void ManualUpdate()
	{
		DeltaTime = GetLibTime() - lastTick;
		lastTick = GetLibTime();

		Framerate = 1 / DeltaTime;
	}

	void Terminate()
	{
		glfwTerminate();
	}

	double GetLibTime()
	{
		return glfwGetTime();
	}

	std::string GetDefShaderCode(DefShaderType_ which)
	{
		switch (which)
		{
		case DefShaderType_defVertex:
			return std::string(src_defVertex);
		case DefShaderType_defFragment:
			return std::string(src_defFragment);
		case DefShaderType_dtVertex:
			return std::string(src_DT_Vertex);
		case DefShaderType_dtFragment:
			return std::string(src_DT_Fragment);
		}
		Debug::Error("Invalid GetDefShaderCode() 'which' parameter.");
		return "";
	}

	bool RectButton(Object& obj, Color defaultColor, Color hoverColor, Vec2 anchor, bool ChangeMouseShape)
	{
		if (obj.Rect_OnMouseEnter(anchor))
		{
			if (ChangeMouseShape) obj.GetContext().cursor.SetLooks(CursorShape_PointingHand);
			obj.color = hoverColor;
			return obj.Rect_OnMouseClick(anchor);
		}
		obj.color = defaultColor;
		return false;
	}
	bool RectButton(DynamicText& obj, Color defaultColor, Color hoverColor, bool ChangeMouseShape)
	{
		if (obj.Rect_OnMouseEnter())
		{
			if (ChangeMouseShape) obj.GetContext().cursor.SetLooks(CursorShape_PointingHand);
			obj.backColor = hoverColor;
			return obj.Rect_OnMouseClick();
		}
		obj.backColor = defaultColor;
		return false;
	}
	bool RectButton(Graph& obj, Color defaultColor, Color hoverColor, bool ChangeMouseShape)
	{
		if (obj.Rect_OnMouseEnter())
		{
			if (ChangeMouseShape) obj.GetContext().cursor.SetLooks(CursorShape_PointingHand);
			obj.backgroundColor = hoverColor;
			return obj.Rect_OnMouseClick();
		}
		obj.backgroundColor = defaultColor;
		return false;
	}

	void SetEdges(Vec2 min, Vec2 max)
	{
		if (min.x == -1 || min.y == -1 || max.x == -1 || max.y == -1)
		LG_EdgeMin = min;
		LG_EdgeMax = max;
	}

	//------------------<LibGui::Debug FUNCTIONS>------------------
	namespace Debug
	{
		bool LogRepeated = false;
		bool ErrorCrash = false;
		DebugLevel_ LogFilter = DebugLevel_None;

		static std::string lastLog = "";
		void Happy(const std::string& message)
		{
			if (LogFilter >= DebugLevel_Happy) return;

			if ("HAPPY$" + message == lastLog && !LogRepeated)
				return;
			lastLog = "HAPPY$" + message;

			std::cout << "\033[32m[HAPPY]:)\033[0m " << message << '\n';
		}
		void Log(const std::string& message)
		{
			if (LogFilter >= DebugLevel_Log) return;

			if ("LOG$" + message == lastLog && !LogRepeated)
				return;
			lastLog = "LOG$" + message;

			std::cout << "[LOG]:    " << message << '\n';
		}
		void Warn(const std::string& message)
		{
			if (LogFilter >= DebugLevel_Warning) return;

			if ("WARN$" + message == lastLog && !LogRepeated)
				return;
			lastLog = "WARN$" + message;

			std::cout << "\033[33m[WARNING]:\033[0m" << message << "\n";
		}
		void Error(const std::string& message) {
			if (LogFilter < DebugLevel_Error)
			{
				if ("ERROR$" + message == lastLog && !LogRepeated)
					return;
				lastLog = "ERROR$" + message;

				std::cout << "\033[31m[ERROR]:\033[0m  " << message << "\n";
			}
			if (!ErrorCrash)
				return;

			// If glfw was initialized, terminate it
			if (GetLibTime() != 0)
			{
				std::cout << "...Terminating LibGui\n";
				Terminate();
			}
			std::cout << "...Closing program.\n";
			throw "[LibGui::Debug::Error() was called]:" + message;
		}
		void FatalError(const std::string& message)
		{
			std::cout << "\033[35m[FATAL]:\033[0m  " << message << "\n(to stop crash, do LibGui::Debug::ErrorCrash = false)";

			// If glfw was initialized, terminate it
			if (GetLibTime() != 0)
			{
				std::cout << "...Terminating LibGui\n";
				Terminate();
			}

			std::cout << "...Closing program.\n";
			throw "[LibGui::Debug::FatalError() was called]:" + message;
		}
	}
}