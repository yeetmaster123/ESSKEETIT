#pragma once

#include "Hooks.h"

class Fonts
{
public:
	DWORD menu;
	DWORD font_icons;
	DWORD menu_bold;
	DWORD esp;
	DWORD esp_extra;
	DWORD esp_small;
	DWORD indicator;
	DWORD aaindicator;
	DWORD Legitbot;
	DWORD in_game_logging_font;
	DWORD visuals_grenade_pred_font;
	DWORD in_game_logging_font_1;
	DWORD in_game_logging_font_2;
	DWORD in_game_logging_font_3;
	DWORD in_game_logging_font_4;
};

class DrawManager
{
public:
	Fonts fonts;
	void rect(int x, int y, int w, int h, Color color)
	{
		m_pSurface->DrawSetColor(color);
		m_pSurface->DrawFilledRect(x, y, x + w, y + h);
	}

	void outlined_rect(int x, int y, int w, int h, Color color_out, Color color_in)
	{
		m_pSurface->DrawSetColor(color_in);
		m_pSurface->DrawFilledRect(x, y, x + w, y + h);

		m_pSurface->DrawSetColor(color_out);
		m_pSurface->DrawOutlinedRect(x, y, x + w, y + h);
	}

	void outline(int x, int y, int w, int h, Color color)
	{
		m_pSurface->DrawSetColor(color);
		m_pSurface->DrawOutlinedRect(x, y, x + w, y + h);
	}

	void line(int x, int y, int x2, int y2, Color color)
	{
		m_pSurface->DrawSetColor(color);
		m_pSurface->DrawLine(x, y, x2, y2);
	}

	void polyline(int *x, int *y, int count, Color color)
	{
		m_pSurface->DrawSetColor(color);
		m_pSurface->DrawPolyLine(x, y, count);
	}
	void textf(int x, int y, Color color, DWORD font, const char* fmt, ...)
	{

		if (!fmt) return; //if the passed string is null return
		if (strlen(fmt) < 2) return;

		//Set up va_list and buffer to hold the params 
		va_list va_alist;
		char logBuf[256] = { 0 };

		//Do sprintf with the parameters
		va_start(va_alist, fmt);
		_vsnprintf_s(logBuf + strlen(logBuf), 256 - strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
		va_end(va_alist);

		text(x, y, logBuf, font, color);

	}
	void polygon(int count, Vertex_t* Vertexs, Color color)
	{
		static int Texture = m_pSurface->CreateNewTextureID(true);
		unsigned char buffer[4] = { 255, 255, 255, 255 };

		m_pSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
		m_pSurface->DrawSetColor(color);
		m_pSurface->DrawSetTexture(Texture);

		m_pSurface->DrawTexturedPolygon(count, Vertexs);
	}
	unsigned int CreateF(std::string font_name, int size, int weight, int blur, int scanlines, int flags)
	{
		auto font = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(font, font_name.c_str(), size, weight, blur, scanlines, flags);

		return font;
	}
	void polygon_outlined(int count, Vertex_t* Vertexs, Color color, Color colorLine)
	{
		static int x[128];
		static int y[128];

		polygon(count, Vertexs, color);

		for (int i = 0; i < count; i++)
		{
			x[i] = Vertexs[i].m_Position.x;
			y[i] = Vertexs[i].m_Position.y;
		}

		polyline(x, y, count, colorLine);
	}

	void gradient_verticle(int x, int y, int w, int h, Color c1, Color c2)
	{
		rect(x, y, w, h, c1);
		BYTE first = c2.r();
		BYTE second = c2.g();
		BYTE third = c2.b();
		for (int i = 0; i < h; i++)
		{
			float fi = i, fh = h;
			float a = fi / fh;
			DWORD ia = a * 255;
			rect(x, y + i, w, 1, Color(first, second, third, ia));
		}
	}
	RECT GetViewport()
	{
		RECT Viewport = { 0, 0, 0, 0 };
		int w, h;
		m_pEngine->GetScreenSize(w, h);
		Viewport.right = w; Viewport.bottom = h;
		return Viewport;
	}
	bool WorldToScreen(Vector &in, Vector &out)
	{
		const matrix3x4& worldToScreen = m_pEngine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

		float w = worldToScreen[3][0] * in[0] + worldToScreen[3][1] * in[1] + worldToScreen[3][2] * in[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
		out.z = 0; //Screen doesn't have a 3rd dimension.

		if (w > 0.001) //If the object is within view.
		{
			RECT ScreenSize = GetViewport();
			float fl1DBw = 1 / w; //Divide 1 by the angle.
			out.x = (ScreenSize.right / 2) + (0.5f * ((worldToScreen[0][0] * in[0] + worldToScreen[0][1] * in[1] + worldToScreen[0][2] * in[2] + worldToScreen[0][3]) * fl1DBw) * ScreenSize.right + 0.5f); //Get the X dimension and push it in to the Vector.
			out.y = (ScreenSize.bottom / 2) - (0.5f * ((worldToScreen[1][0] * in[0] + worldToScreen[1][1] * in[1] + worldToScreen[1][2] * in[2] + worldToScreen[1][3]) * fl1DBw) * ScreenSize.bottom + 0.5f); //Get the Y dimension and push it in to the Vector.
			return true;
		}

		return false;
	}
	void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
	{
		Color colColor(0, 0, 0);
		flRainbow += flSpeed;
		if (flRainbow > 1.f) flRainbow = 0.f;
		for (int i = 0; i < width; i++)
		{
			float hue = (1.f / (float)width) * i;
			hue -= flRainbow;
			if (hue < 0.f) hue += 1.f;
			Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
			rect(x + i, y, 1, height, colRainbow);
		}
	}
	void textold(int x, int y, const wchar_t* _Input, int font, Color color)
	{
		m_pSurface->DrawSetTextFont(font);
		m_pSurface->DrawSetTextColor(color);
		m_pSurface->DrawSetTextPos(x, y);

		m_pSurface->DrawPrintText(_Input, wcslen(_Input));
	}
	void gradient_horizontal(int x, int y, int w, int h, Color c1, Color c2)
	{
		rect(x, y, w, h, c1);
		BYTE first = c2.r();
		BYTE second = c2.g();
		BYTE third = c2.b();
		for (int i = 0; i < w; i++)
		{
			float fi = i, fw = w;
			float a = fi / fw;
			DWORD ia = a * 255;
			rect(x + i, y, 1, h, Color(first, second, third, ia));
		}
	}
	void text(int x, int y, const char* _Input, int font, Color color)
	{
		int apple = 0;
		char Buffer[2048] = { '\0' };
		va_list Args;
		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);
		size_t Size = strlen(Buffer) + 1;
		wchar_t* WideBuffer = new wchar_t[Size];
		mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);

		m_pSurface->DrawSetTextColor(color);
		m_pSurface->DrawSetTextFont(font);
		m_pSurface->DrawSetTextPos(x, y);
		m_pSurface->DrawPrintText(WideBuffer, wcslen(WideBuffer));
	}

	Vector2D GetTextSize(unsigned int Font, std::string Input)
	{
		/* char -> wchar */
		size_t size = Input.size() + 1;
		auto wide_buffer = std::make_unique<wchar_t[]>(size);
		mbstowcs_s(0, wide_buffer.get(), size, Input.c_str(), size - 1);

		int width, height;
		m_pSurface->GetTextSize(Font, wide_buffer.get(), width, height);

		return Vector2D(width, height);
	}
	void DrawF(int X, int Y, unsigned int Font, bool center_width, bool center_height, Color Color, std::string Input)
	{
		/* char -> wchar */
		size_t size = Input.size() + 1;
		auto wide_buffer = std::make_unique<wchar_t[]>(size);
		mbstowcs_s(0, wide_buffer.get(), size, Input.c_str(), size - 1);

		/* check center */
		int width = 0, height = 0;
		m_pSurface->GetTextSize(Font, wide_buffer.get(), width, height);
		if (!center_width)
			width = 0;
		if (!center_height)
			height = 0;

		/* call and draw*/
		m_pSurface->DrawSetTextColor(Color);
		m_pSurface->DrawSetTextFont(Font);
		m_pSurface->DrawSetTextPos(X - (width * .5), Y - (height * .5));
		m_pSurface->DrawPrintTextxd(wide_buffer.get(), wcslen(wide_buffer.get()), 0);
	}


	RECT get_text_size(const char* _Input, int font)
	{
		int apple = 0;
		char Buffer[2048] = { '\0' };
		va_list Args;
		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);
		size_t Size = strlen(Buffer) + 1;
		wchar_t* WideBuffer = new wchar_t[Size];
		mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);
		int Width = 0, Height = 0;

		m_pSurface->GetTextSize(font, WideBuffer, Width, Height);

		RECT outcome = { 0, 0, Width, Height };
		return outcome;
	}

	void color_spectrum(int x, int y, int w, int h)
	{
		static int GradientTexture = 0;
		static std::unique_ptr<Color[]> Gradient = nullptr;
		if (!Gradient)
		{
			Gradient = std::make_unique<Color[]>(w * h);

			for (int i = 0; i < w; i++)
			{
				int div = w / 6;
				int phase = i / div;
				float t = (i % div) / (float)div;
				int r, g, b;

				switch (phase)
				{
				case(0):
					r = 255;
					g = 255 * t;
					b = 0;
					break;
				case(1):
					r = 255 * (1.f - t);
					g = 255;
					b = 0;
					break;
				case(2):
					r = 0;
					g = 255;
					b = 255 * t;
					break;
				case(3):
					r = 0;
					g = 255 * (1.f - t);
					b = 255;
					break;
				case(4):
					r = 255 * t;
					g = 0;
					b = 255;
					break;
				case(5):
					r = 255;
					g = 0;
					b = 255 * (1.f - t);
					break;
				}

				for (int k = 0; k < h; k++)
				{
					float sat = k / (float)h;
					int _r = r + sat * (128 - r);
					int _g = g + sat * (128 - g);
					int _b = b + sat * (128 - b);

					*reinterpret_cast<Color*>(Gradient.get() + i + k * w) = Color(_r, _g, _b);
				}
			}

			GradientTexture = m_pSurface->CreateNewTextureID(true);
			m_pSurface->DrawSetTextureRGBA(GradientTexture, (unsigned char*)Gradient.get(), w, h);
		}
		m_pSurface->DrawSetColor(Color(255, 255, 255, 255));
		m_pSurface->DrawSetTexture(GradientTexture);
		m_pSurface->DrawTexturedRect(x, y, x + w, y + h);
	}

	Color color_spectrum_pen(int x, int y, int w, int h, Vector stx)
	{
		int div = w / 6;
		int phase = stx.x / div;
		float t = ((int)stx.x % div) / (float)div;
		int r, g, b;

		switch (phase)
		{
		case(0):
			r = 255;
			g = 255 * t;
			b = 0;
			break;
		case(1):
			r = 255 * (1.f - t);
			g = 255;
			b = 0;
			break;
		case(2):
			r = 0;
			g = 255;
			b = 255 * t;
			break;
		case(3):
			r = 0;
			g = 255 * (1.f - t);
			b = 255;
			break;
		case(4):
			r = 255 * t;
			g = 0;
			b = 255;
			break;
		case(5):
			r = 255;
			g = 0;
			b = 255 * (1.f - t);
			break;
		}

		float sat = stx.y / h;
		return Color(r + sat * (128 - r), g + sat * (128 - g), b + sat * (128 - b), 255);
	}

	void filled_circle(int x, int y, float points, float radius, Color color)
	{
		std::vector<Vertex_t> vertices;
		float step = (float)M_PI * 2.0f / points;

		for (float a = 0; a < (M_PI * 2.0f); a += step)
			vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + x, radius * sinf(a) + y)));

		polygon((int)points, vertices.data(), color);
	}

	void DrawLine(int x1, int y1, int x2, int y2, Color color)
	{
		m_pSurface->DrawSetColor(color);
		m_pSurface->DrawLine(x1, y1, x2, y2);
	}


	void DrawFilled3DBox(Vector origin, int width, int height, Color outline, Color filling)
	{
		float difw = float(width / 2);
		float difh = float(height / 2);
		Vector boxVectors[8] =
		{
			Vector(origin.x - difw, origin.y - difh, origin.z - difw),
			Vector(origin.x - difw, origin.y - difh, origin.z + difw),
			Vector(origin.x + difw, origin.y - difh, origin.z + difw),
			Vector(origin.x + difw, origin.y - difh, origin.z - difw),
			Vector(origin.x - difw, origin.y + difh, origin.z - difw),
			Vector(origin.x - difw, origin.y + difh, origin.z + difw),
			Vector(origin.x + difw, origin.y + difh, origin.z + difw),
			Vector(origin.x + difw, origin.y + difh, origin.z - difw),
		};

		static Vector vec0, vec1, vec2, vec3,
			vec4, vec5, vec6, vec7;

		if (WorldToScreen(boxVectors[0], vec0) &&
			WorldToScreen(boxVectors[1], vec1) &&
			WorldToScreen(boxVectors[2], vec2) &&
			WorldToScreen(boxVectors[3], vec3) &&
			WorldToScreen(boxVectors[4], vec4) &&
			WorldToScreen(boxVectors[5], vec5) &&
			WorldToScreen(boxVectors[6], vec6) &&
			WorldToScreen(boxVectors[7], vec7))
		{
			Vector2D lines[12][2];
			lines[0][0] = vec0;
			lines[0][1] = vec1;
			lines[1][0] = vec1;
			lines[1][1] = vec2;
			lines[2][0] = vec2;
			lines[2][1] = vec3;
			lines[3][0] = vec3;
			lines[3][1] = vec0;

			// top of box
			lines[4][0] = vec4;
			lines[4][1] = vec5;
			lines[5][0] = vec5;
			lines[5][1] = vec6;
			lines[6][0] = vec6;
			lines[6][1] = vec7;
			lines[7][0] = vec7;
			lines[7][1] = vec4;

			lines[8][0] = vec0;
			lines[8][1] = vec4;

			lines[9][0] = vec1;
			lines[9][1] = vec5;

			lines[10][0] = vec2;
			lines[10][1] = vec6;

			lines[11][0] = vec3;
			lines[11][1] = vec7;

			for (int i = 0; i < 12; i++)
				DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, outline);
		}
	}
	
};
extern DrawManager draw;