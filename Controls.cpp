
#include "Controls.h"
#include "DrawManager.h"
#include "Menu.h"
#include <iostream>

#define white Color(255, 255, 255)
#define mixed Color(90, 90, 90)
#define lighter_gray Color(48, 48, 48)
#define light_gray Color(40, 40, 40)
#define gray Color(28, 28, 28)
#define dark_gray Color(21, 21, 19)
#define darker_gray Color(19, 19, 19)
#define black Color(0, 0, 0)

#pragma region Base Control
void CControl::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void CControl::SetSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}

void CControl::GetSize(int &w, int &h)
{
	w = m_iWidth;
	h = m_iHeight;
}

bool CControl::Flag(int f)
{
	if (m_Flags & f)
		return true;
	else
		return false;
}

POINT CControl::GetAbsolutePos()
{
	POINT p;
	RECT client = parent->GetClientArea();
	if (parent)
	{
		p.x = m_x + client.left;
		p.y = m_y + client.top + 29;
	}
	
	return p;
}

void CControl::SetFileId(std::string fid)
{
	FileIdentifier = fid;
}
#pragma endregion Implementations of the Base control functions

#pragma region CheckBox
CCheckBox::CCheckBox()
{
	Checked = false;
	bIsSub = false;

	m_Flags = UIFlags::UI_Clickable | UIFlags::UI_Drawable | UIFlags::UI_SaveFile;
	m_iHeight = 9;

	FileControlType = UIControlTypes::UIC_CheckBox;
}

void CCheckBox::SetState(bool s)
{
	Checked = s;
}

bool CCheckBox::GetState()
{
	return Checked;
}

bool CCheckBox::GetIsSub()
{
	return bIsSub;
}

void CCheckBox::SetAsSub(bool t)
{
	bIsSub = t;
}

void CCheckBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();


	Color grad;
	bool bSetRed = false;
	bool bSetGreen = false;
	bool bSetBlue = false;
	if (game::globals.forecolor.r() >= 15)
		bSetRed = true;
	if (game::globals.forecolor.g() >= 15)
		bSetGreen = true;
	if (game::globals.forecolor.b() >= 15)
		bSetBlue = true;

	float red = bSetRed ? game::globals.forecolor.r() - 15 : game::globals.forecolor.r();
	float green = bSetGreen ? game::globals.forecolor.g() - 15 : game::globals.forecolor.g();
	float blue = bSetBlue ? game::globals.forecolor.b() - 15 : game::globals.forecolor.b();

	grad = Color(red, green, blue, 245);
	
//	Color unchecked = Color(miscconfig.cCheckboxUnchecked[0], miscconfig.cCheckboxUnchecked[1], miscconfig.cCheckboxUnchecked[2]);
	/*
	Color unchecked_grad;
	bool unchecked_bSetRed = false;
	bool unchecked_bSetGreen = false;
	bool unchecked_bSetBlue = false;
	if (unchecked.r() >= 15)
		unchecked_bSetRed = true;
	if (unchecked.g() >= 15)
		unchecked_bSetGreen = true;
	if (unchecked.b() >= 15)
		unchecked_bSetBlue = true;

	float unchecked_red = unchecked_bSetRed ? unchecked.r() - 15 : unchecked.r();
	float unchecked_green = unchecked_bSetGreen ? unchecked.g() - 15 : unchecked.g();
	float unchecked_blue = unchecked_bSetBlue ? unchecked.b() - 15 : unchecked.b();


	draw.gradient_verticle(a.x, a.y, 9, 9, Color(55, 55, 55, 255), Color(40, 40, 40, 245));
	draw.outline(a.x, a.y, 9, 9, Color(2, 2, 2, 255));

	if (Checked)
	{
		draw.gradient_verticle(a.x, a.y, 9, 9, Color(miscconfig.cMenuForecolor), Color(12, 12, 12, 255));
		draw.outline(a.x, a.y, 9, 9, Color(2, 2, 2, 255));
	}*/
}

void CCheckBox::OnUpdate() { m_iHeight = 9; }

void CCheckBox::OnClick()
{
	if (!should_animate)
		Checked = !Checked;
}
#pragma endregion Implementations of the Check Box functions

#pragma region Label
CLabel::CLabel()
{
	m_Flags = UIFlags::UI_Drawable;
	FileControlType = UIC_Label;
	Text = "Default";
	FileIdentifier = "Default";
	m_iHeight = 10;
}

void CLabel::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	draw.text(a.x, a.y - 1, Text.c_str(), draw.fonts.menu, Color(205, 205, 205, 255));
}

void CLabel::SetText(std::string text)
{
	Text = text;
}

void CLabel::OnUpdate(){}
void CLabel::OnClick(){}
#pragma endregion Implementations of the Label functions

#pragma region GroupBox
CGroupBox::CGroupBox()
{
	Items = 1;
	last_y = 0;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
	FileControlType = UIControlTypes::UIC_GroupBox;
}
void CGroupBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = draw.get_text_size(Text.c_str(), draw.fonts.menu);
	draw.rect(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 1));
	draw.text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Text.c_str(), draw.fonts.menu, Color(244, 244, 244, 255));
	if (group_tabs.size())
	{
		draw.line(a.x + 1, a.y + 8, a.x + m_iWidth, a.y + 8, Color(48, 48, 48, 255));
		draw.line(a.x + 1, a.y + 38, a.x + m_iWidth, a.y + 38, Color(48, 48, 48, 255));
		draw.rect(a.x + 1, a.y + 9, m_iWidth - 1, 29, Color(21, 21, 19, 255));
		draw.rect(a.x + 1, a.y + 36, m_iWidth - 1, 2, Color(62, 62, 62, 255));

		for (int i = 0; i < group_tabs.size(); i++)
		{
			RECT text_size = draw.get_text_size(group_tabs[i].name.c_str(), draw.fonts.menu);

			int width = m_iWidth - 1;

			int tab_length = (width / group_tabs.size());

			int text_position[] = {
				(a.x + (tab_length * (i + 1)) - (tab_length / 2)),
				a.y + 23 - (text_size.bottom / 2)
			};

			RECT tab_area = {
				(a.x + 1) + (tab_length * i),
				a.y + 9,
				tab_length,
				29
			};

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				if (GUI.IsMouseInRegion(tab_area))
				{
					selected_tab = group_tabs[i].id;
				}
			}

			if (selected_tab == group_tabs[i].id)
			{
				draw.rect(tab_area.left, tab_area.top, tab_area.right, tab_area.bottom, game::globals.forecolor);
				draw.rect(tab_area.left, tab_area.top, tab_area.right, tab_area.bottom - 2, Color(39, 39, 39, 255));
				draw.text(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), draw.fonts.menu, Color(205, 205, 205, 245));
			}
			else if (selected_tab != group_tabs[i].id)
				draw.text(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), draw.fonts.menu, Color(92, 92, 92, 255));
		}
	}

	//Top Left
	draw.line(a.x, a.y, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 2, a.y, Color(48, 48, 48, 255));
	draw.line(a.x - 1, a.y - 1, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 1, a.y - 1, Color(0, 0, 0, 255));

	//Top Right
	draw.line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	draw.line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y - 1, a.x + m_iWidth + 1, a.y - 1, Color(0, 0, 0, 255));

	//Left
	draw.line(a.x, a.y, a.x, a.y + m_iHeight, Color(49, 49, 49, 255));
	draw.line(a.x - 1, a.y, a.x - 1, a.y + m_iHeight, Color(0, 0, 0, 255));

	//Bottom
	draw.line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	draw.line(a.x - 1, a.y + m_iHeight + 1, a.x + m_iWidth + 2, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	//Right
	draw.line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight + 1, Color(48, 48, 48, 255));
	draw.line(a.x + m_iWidth + 1, a.y, a.x + m_iWidth + 1, a.y + m_iHeight + 1, Color(0, 0, 0, 255));
}

void CGroupBox::SetText(std::string text)
{
	Text = text;
}

void CGroupBox::PlaceLabledControl(int g_tab, std::string Label, CTab *Tab, CControl* control) {
	bool has_tabs = group_tabs.size() ? 1 : 0;

	if (has_tabs) {
		bool has_reset = false;

		for (int i = 0; i < reset_tabs.size(); i++) {
			if (reset_tabs[i] == g_tab)
				has_reset = true;
		}

		if (!has_reset) {
			initialized = false;
			reset_tabs.push_back(g_tab);
		}
	}

	if (!initialized) {
		Items = 0;
		last_y = has_tabs ? m_y + 48 : m_y + 8;
		initialized = true;
	}

	bool add_label_y = true;
	bool is_checkbox = control->FileControlType == UIControlTypes::UIC_CheckBox;
	bool is_label = control->FileControlType == UIControlTypes::UIC_Label;
	bool is_keybind = control->FileControlType == UIControlTypes::UIC_KeyBind;
	bool is_color = control->FileControlType == UIControlTypes::UIC_ColorSelector;

	int x = m_x + 38;
	int y = last_y;
	int control_width, control_height;
	control->GetSize(control_width, control_height);

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	label->parent_group = this;
	label->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(label);

	if (is_checkbox || is_label || is_color) add_label_y = false;

	if (Label != "" && add_label_y && !is_keybind) {
		RECT label_size = draw.get_text_size(Label.c_str(), draw.fonts.menu);
		last_y += 14;
		y = last_y;
	}

	if (!is_keybind)
		last_control_height = control_height + 7;

	if (is_keybind || (is_color && Label == "")) {
		y -= last_control_height;
		x = m_x + m_iWidth - 36;
	}
	if (is_color && Label != "")
		x = m_x + m_iWidth - 36;
	if (is_checkbox)
		x -= 24;

	control->SetPosition(x, is_checkbox ? y + 1 : y);
	control->SetSize(m_iWidth - (38 * 2), control_height);
	control->parent_group = this;
	control->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(control);

	if (!is_keybind) {
		if (!is_color || is_color && Label != "")
			last_y += control_height + 7;
	}
}

void CGroupBox::AddTab(CGroupTab t)
{
	group_tabs.push_back(t);

	if (selected_tab == 0)
		selected_tab++;
}
void CGroupBox::OnUpdate(){}
void CGroupBox::OnClick(){}

//New subs and shit for the legitbot yw simple now give me money thanks
CGroupBox2::CGroupBox2()
{
	Items = 1;
	last_y = 0;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
	FileControlType = UIControlTypes::UIC_GroupBox;
}
void CGroupBox2::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = draw.get_text_size(Text.c_str(), draw.fonts.Legitbot);
	draw.rect(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 1));
	draw.text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Text.c_str(), draw.fonts.Legitbot, Color(244, 244, 244, 255));
	if (group_tabs.size())
	{
		draw.line(a.x + 1, a.y + 8, a.x + m_iWidth, a.y + 8, Color(48, 48, 48, 255));
		draw.line(a.x + 1, a.y + 38, a.x + m_iWidth, a.y + 38, Color(48, 48, 48, 255));
		draw.rect(a.x + 1, a.y + 9, m_iWidth - 1, 29, Color(21, 21, 19, 255));
		draw.rect(a.x + 1, a.y + 36, m_iWidth - 1, 2, Color(62, 62, 62, 255));

		for (int i = 0; i < group_tabs.size(); i++)
		{
			RECT text_size = draw.get_text_size(group_tabs[i].name.c_str(), draw.fonts.Legitbot);

			int width = m_iWidth - 1;

			int tab_length = (width / group_tabs.size());

			int text_position[] = {
				(a.x + (tab_length * (i + 1)) - (tab_length / 2)),
				a.y + 23 - (text_size.bottom / 2)
			};

			RECT tab_area = {
				(a.x + 1) + (tab_length * i),
				a.y + 9,
				tab_length,
				29
			};

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				if (GUI.IsMouseInRegion(tab_area))
				{
					selected_tab = group_tabs[i].id;
				}
			}

			if (selected_tab == group_tabs[i].id)
			{
				draw.text(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), draw.fonts.Legitbot, Color(255, 255, 255, 255));
			}
			else if (selected_tab != group_tabs[i].id)
				draw.text(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), draw.fonts.Legitbot, Color(92, 92, 92, 255));
		}
	}

	//Top Left
	draw.line(a.x, a.y, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 2, a.y, Color(48, 48, 48, 255));
	draw.line(a.x - 1, a.y - 1, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 1, a.y - 1, Color(0, 0, 0, 255));

	//Top Right
	draw.line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	draw.line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y - 1, a.x + m_iWidth + 1, a.y - 1, Color(0, 0, 0, 255));

	//Left
	draw.line(a.x, a.y, a.x, a.y + m_iHeight, Color(49, 49, 49, 255));
	draw.line(a.x - 1, a.y, a.x - 1, a.y + m_iHeight, Color(0, 0, 0, 255));

	//Bottom
	draw.line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	draw.line(a.x - 1, a.y + m_iHeight + 1, a.x + m_iWidth + 2, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	//Right
	draw.line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight + 1, Color(48, 48, 48, 255));
	draw.line(a.x + m_iWidth + 1, a.y, a.x + m_iWidth + 1, a.y + m_iHeight + 1, Color(0, 0, 0, 255));
}
void CGroupBox2::SetText(std::string text)
{
	Text = text;
}

void CGroupBox2::PlaceLabledControl(int g_tab, std::string Label, CTab *Tab, CControl* control) {
	bool has_tabs = group_tabs.size() ? 1 : 0;

	if (has_tabs) {
		bool has_reset = false;

		for (int i = 0; i < reset_tabs.size(); i++) {
			if (reset_tabs[i] == g_tab)
				has_reset = true;
		}

		if (!has_reset) {
			initialized = false;
			reset_tabs.push_back(g_tab);
		}
	}

	if (!initialized) {
		Items = 0;
		last_y = has_tabs ? m_y + 48 : m_y + 8;
		initialized = true;
	}

	bool add_label_y = true;
	bool is_checkbox = control->FileControlType == UIControlTypes::UIC_CheckBox;
	bool is_label = control->FileControlType == UIControlTypes::UIC_Label;
	bool is_keybind = control->FileControlType == UIControlTypes::UIC_KeyBind;
	bool is_color = control->FileControlType == UIControlTypes::UIC_ColorSelector;

	int x = m_x + 38;
	int y = last_y;
	int control_width, control_height;
	control->GetSize(control_width, control_height);

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	label->parent_group = this;
	label->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(label);

	if (is_checkbox || is_label || is_color) add_label_y = false;

	if (Label != "" && add_label_y && !is_keybind) {
		RECT label_size = draw.get_text_size(Label.c_str(), draw.fonts.menu);
		last_y += 14;
		y = last_y;
	}

	if (!is_keybind)
		last_control_height = control_height + 7;

	if (is_keybind || (is_color && Label == "")) {
		y -= last_control_height;
		x = m_x + m_iWidth - 36;
	}
	if (is_color && Label != "")
		x = m_x + m_iWidth - 36;
	if (is_checkbox)
		x -= 24;

	control->SetPosition(x, is_checkbox ? y + 1 : y);
	control->SetSize(m_iWidth - (38 * 2), control_height);
	control->parent_group = this;
	control->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(control);

	if (!is_keybind) {
		if (!is_color || is_color && Label != "")
			last_y += control_height + 7;
	}
}

void CGroupBox2::AddTab(CGroupTab t)
{
	group_tabs.push_back(t);

	if (selected_tab == 0)
		selected_tab++;
}
void CGroupBox2::OnUpdate() {}
void CGroupBox2::OnClick() {}
#pragma endregion Implementations of the Group Box functions

#pragma region Sliders
CSlider::CSlider()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	Format = FORMAT_INT;
	m_iHeight = 10;
	FileControlType = UIControlTypes::UIC_Slider;
}

void CSlider::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	draw.gradient_verticle(a.x, a.y, m_iWidth, 7, Color(62, 62, 62, 255), Color(55, 55, 55, 245));

	float Ratio = ( Value - Min ) / ( Max - Min );
	float Location = Ratio*m_iWidth;

	Color grad;
	bool bSetRed = false;
	bool bSetGreen = false;
	bool bSetBlue = false;
	if (game::globals.forecolor.r() >= 15)
		bSetRed = true;
	if (game::globals.forecolor.g() >= 15)
		bSetGreen = true;
	if (game::globals.forecolor.b() >= 15)
		bSetBlue = true;

	float red = bSetRed ? game::globals.forecolor.r() - 15 : game::globals.forecolor.r();
	float green = bSetGreen ? game::globals.forecolor.g() - 15 : game::globals.forecolor.g();
	float blue = bSetBlue ? game::globals.forecolor.b() - 15 : game::globals.forecolor.b();

	grad = Color(red, green, blue, 245);

	draw.gradient_verticle(a.x, a.y, Location, 7, game::globals.forecolor, grad);

	draw.outline(a.x, a.y, m_iWidth, 7, Color(2, 2, 2, 255));

	char buffer[24];
	const char* format;
	if (Format == FORMAT_DECDIG2)
		sprintf_s(buffer, "%.2f%s", Value, extension );
	else if (Format == FORMAT_DECDIG1)
		sprintf_s(buffer, "%.1f%s", Value, extension );
	else if (Format == FORMAT_INT)
		sprintf_s(buffer, "%1.0f%s", Value, extension);

	RECT txtSize = draw.get_text_size(buffer, draw.fonts.menu_bold);
	draw.text(a.x + Location - (txtSize.right / 2), a.y + 7 - (txtSize.bottom / 2), buffer, draw.fonts.menu_bold, Color(180, 180, 180, 245));
}

void CSlider::OnUpdate(){
	POINT a = GetAbsolutePos();
	m_iHeight = 15;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x;//-1
			if ( NewX < 0 )
				NewX = 0;
			if ( NewX > m_iWidth )
				NewX = m_iWidth;
			Ratio = NewX / float( m_iWidth );
			Value = Min + ( Max - Min )*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSlider::OnClick(){
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSlider::GetValue()
{
	return Value;
}

void CSlider::SetValue(float v)
{
	Value = v;
}

void CSlider::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}

void CSlider::SetFormat(SliderFormat type)
{
	Format = type;
}
#pragma endregion Implementations of the Slider functions

#pragma region ItemSelector
CItemSelector::CItemSelector()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_ItemSelector;
	m_iHeight = 16;
	Type = ITEMSELECTOR_INT;
}

void CItemSelector::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	//Minus

	RECT rMinus = { a.x, a.y, 16, m_iHeight };

	draw.outline(a.x - 1, a.y - 1, 18, m_iHeight + 2, Color(0, 0, 0, 255));
	draw.rect(a.x, a.y, 16, m_iHeight, Color(39, 39, 39, 255));

	if (GUI.IsMouseInRegion(rMinus) && GetAsyncKeyState(VK_LBUTTON))
		draw.gradient_verticle(a.x, a.y, 16, m_iHeight, Color(55, 55, 55, 255), Color(40, 40, 40, 245));

	RECT txtSizeMinus = draw.get_text_size("-", draw.fonts.menu);
	draw.text(a.x + (rMinus.right / 2) - (txtSizeMinus.right / 2),
		a.y + (m_iHeight / 2) - (txtSizeMinus.bottom / 2),
		XorStr("-"),
		draw.fonts.menu,
		Color(205, 205, 205, 245));

	//Main

	draw.outline(a.x + 19, a.y - 1, m_iWidth - 42, m_iHeight + 2, Color(0, 0, 0, 255));
	draw.gradient_verticle(a.x + 20, a.y, m_iWidth - 40, m_iHeight, Color(55, 55, 55, 255), Color(40, 40, 40, 245));

	if (Type == ITEMSELECTOR_INT)
	{
		char buffer[24];
		sprintf_s(buffer, "%1.0f", Value);

		RECT txtValueSize = draw.get_text_size(buffer, draw.fonts.menu);
		draw.text(a.x + (m_iWidth / 2) - (txtValueSize.right / 2),
			a.y + (m_iHeight / 2) - (txtValueSize.bottom / 2),
			buffer,
			draw.fonts.menu,
			Color(245, 245, 245, 245));
	}
	else if (Type == ITEMSELECTOR_STRING)
	{
		std::string text = Items[Value];
		RECT txtValueSize = draw.get_text_size(text.c_str(), draw.fonts.menu);
		draw.text(a.x + (m_iWidth / 2) - (txtValueSize.right / 2),
			a.y + (m_iHeight / 2) - (txtValueSize.bottom / 2),
			text.c_str(),
			draw.fonts.menu,
			Color(245, 245, 245, 245));
	}

	//Plus

	RECT rPlus = { a.x + (m_iWidth - 16), a.y, 16, m_iHeight };

	draw.outline(a.x + (m_iWidth - 16) - 1, a.y - 1, 18, m_iHeight + 2, Color(0, 0, 0, 255));
	draw.rect(a.x + (m_iWidth - 16), a.y, 16, m_iHeight, Color(39, 39, 39, 255));

	if (GUI.IsMouseInRegion(rPlus) && GetAsyncKeyState(VK_LBUTTON))
		draw.gradient_verticle(a.x + (m_iWidth - 16), a.y, 16, m_iHeight, Color(55, 55, 55, 255), Color(40, 40, 40, 245));

	RECT txtSizePlus = draw.get_text_size("+", draw.fonts.menu);
	draw.text(a.x + (m_iWidth - 8) - (txtSizePlus.right / 2),
		a.y + (m_iHeight / 2) - (txtSizePlus.bottom / 2),
		"+",
		draw.fonts.menu,
		Color(245, 245, 245, 245));

}

void CItemSelector::Manager()
{
	if (UpdateType == UPDATE_LEFT)
	{
		if (Value - 1 < Min)
			return;
		else
			Value -= 1;
	}
	else if (UpdateType == UPDATE_RIGHT)
	{
		if (Value + 1 > Max)
			return;
		else
			Value += 1;
	}

	if (!IsHolding)
	{
		UpdateType = UPDATE_NONE;
	}
}

void CItemSelector::OnUpdate()
{
	Manager();

	m_iHeight = 16;

	POINT a = GetAbsolutePos();

	RECT rMinus = { a.x, a.y, 16, m_iHeight };
	RECT rPlus = { a.x + (m_iWidth - 16), a.y, 16, m_iHeight };

	if (GUI.IsMouseInRegion(rMinus) && GetAsyncKeyState(VK_LBUTTON))
	{
		preTicks += 1;
		if (preTicks > 125)
		{
			UpdateType = UPDATE_LEFT;
			IsHolding = true;
		}
	}
	else if (GUI.IsMouseInRegion(rPlus) && GetAsyncKeyState(VK_LBUTTON))
	{
		preTicks += 1;
		if (preTicks > 125)
		{
			UpdateType = UPDATE_RIGHT;
			IsHolding = true;
		}
	}
	else
		preTicks = 0;

	IsHolding = false;
}

void CItemSelector::OnClick()
{
	POINT a = GetAbsolutePos();

	RECT rMinus = { a.x, a.y, 16, m_iHeight };
	RECT rPlus = { a.x + (m_iWidth - 16), a.y, 16, m_iHeight };

	if (GUI.IsMouseInRegion(rMinus))
		UpdateType = UPDATE_LEFT;
	else if (GUI.IsMouseInRegion(rPlus))
		UpdateType = UPDATE_RIGHT;
}

float CItemSelector::GetInt()
{
	return Value;
}

void CItemSelector::SetValue(float value)
{
	Value = value;
}

void CItemSelector::SetMin(float min)
{
	Min = min;
}

void CItemSelector::SetMax(float max)
{
	Max = max;
}

void CItemSelector::AddItem(std::string name)
{
	Items.push_back(name);
}

void CItemSelector::SetType(ItemSelector_Type type)
{
	Type = type;
}

#pragma endregion Implementations of the Item Selector functions

#pragma region KeyBinders

char* KeyStrings[254] = { "[ _ ]", "[M1]", "[M2]", "[BRK]", "[M3]", "[M4]", "[M5]",
"[ _ ]", "[BSPC]", "[TAB]", "[ _ ]", "[ _ ]", "[ _ ]", "[ENTER]", "[ _ ]", "[ _ ]", "[SHI]",
"[CTRL]", "[ALT]","[PAU]","[CAPS]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ESC]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[SPACE]","[PGUP]", "[PGDOWN]", "[END]", "[HOME]", "[LEFT]",
"[UP]", "[RIGHT]", "[DOWN]", "[ _ ]", "[PRNT]", "[ _ ]", "[PRTSCR]", "[INS]","[DEL]", "[ _ ]", "[0]", "[1]",
"[2]", "[3]", "[4]", "[5]", "[6]", "[7]", "[8]", "[9]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[A]", "[B]", "[C]", "[D]", "[E]", "[F]", "[G]", "[H]", "[I]", "[J]", "[K]", "[L]", "[M]", "[N]", "[O]", "[P]", "[Q]", "[R]", "[S]", "[T]", "[U]",
"[V]", "[W]", "[X]","[Y]", "[Z]", "[LFTWIN]", "[RGHTWIN]", "[ _ ]", "[ _ ]", "[ _ ]", "[NUM0]", "[NUM1]",
"[NUM2]", "[NUM3]", "[NUM4]", "[NUM5]", "[NUM6]","[NUM7]", "[NUM8]", "[NUM9]", "[*]", "[+]", "[_]", "[-]", "[.]", "[/]", "[F1]", "[F2]", "[F3]",
"[F4]", "[F5]", "[F6]", "[F7]", "[F8]", "[F9]", "[F10]", "[F11]", "[F12]","[F13]", "[F14]", "[F15]", "[F16]", "[F17]", "[F18]", "[F19]", "[F20]", "[F21]",
"[F22]", "[F23]", "[F24]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]",
"[NUM LOCK]", "[SCROLL LOCK[", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[LSHFT]", "[RSHFT]", "[LCTRL]",
"[RCTRL]", "[LMENU]", "[RMENU]", "[ _ ]","[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[NTRK]", "[PTRK]", "[STOP]", "[PLAY]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[;]", "[+]", "[,]", "[-]", "[.]", "[/?]", "[~]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]","[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[{]", "[\\|]", "[}]", "['\"]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]" };

CKeyBind::CKeyBind()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	m_iHeight = 12;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

void CKeyBind::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	if (this->Getting_New_Key)
	{
		draw.text(a.x, a.y, "[ _ ]", draw.fonts.menu, Color(110, 110, 110, 255));
	}
	else
	{
		if (key == -1)
			draw.text(a.x, a.y, "[ _ ]", draw.fonts.menu, Color(110, 110, 110, 255));
		else
		{
			char* NameOfKey = KeyStrings[key];
			draw.text(a.x, a.y, NameOfKey, draw.fonts.menu, Color(110, 110, 110, 255));
		}
	}
	
}

void CKeyBind::OnUpdate(){
	m_iHeight = 13;
	RECT text_area;
	if (key == -1)
		text_area = draw.get_text_size("[ _ ]", draw.fonts.menu);
	else
		text_area = draw.get_text_size(text, draw.fonts.menu);
	m_iWidth = text_area.right;
	POINT a = GetAbsolutePos();
	if (Getting_New_Key)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE)
				{
					Getting_New_Key = false;
					key = -1;
					text = "[ _ ]";
					return;
				}

				key = i;
				Getting_New_Key = false;
				text = KeyStrings[i];
				return;
			}
		}
	}
}

void CKeyBind::OnClick(){
	POINT a = GetAbsolutePos();
	if (!Getting_New_Key)
		Getting_New_Key = true;
}

int CKeyBind::GetKey()
{
	return key;
}

void CKeyBind::SetKey(int k)
{
	key = k;
	text = KeyStrings[k];
}

#pragma endregion Implementations of the KeyBind Control functions

#pragma region Button
CButton::CButton()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable;
	FileControlType == UIControlTypes::UIC_Button;
	Text = "Default";
	m_iHeight = 25;
	CallBack = nullptr;
	FileIdentifier = "Default";
}

void CButton::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	if (hover)
		draw.gradient_verticle(a.x, a.y, m_iWidth, m_iHeight, Color(55, 55, 55, 255), Color(55, 55, 55, 255));
	else
		draw.gradient_verticle(a.x, a.y, m_iWidth, m_iHeight, Color(45, 45, 45, 255), Color(45, 45, 45, 255));

	draw.outline(a.x, a.y, m_iWidth, m_iHeight, Color(2, 2, 2, 255));

	RECT TextSize = draw.get_text_size(Text.c_str(), draw.fonts.menu_bold);
	int TextX = a.x + (m_iWidth / 2) - (TextSize.right / 2);
	int TextY = a.y + (m_iHeight / 2) - (TextSize.bottom / 2);

	draw.text(TextX, TextY, Text.c_str(), draw.fonts.menu_bold, Color(180, 180, 180, 200));
}

void CButton::SetText(std::string text)
{
	Text = text;
}

void CButton::SetCallback(CButton::ButtonCallback_t callback)
{
	CallBack = callback;
}

void CButton::OnUpdate()
{
	m_iHeight = 25;
}

void CButton::OnClick()
{
	if (CallBack)
		CallBack();
}
#pragma endregion Implementations of the Button functions

#pragma region ComboBox
CComboBox::CComboBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	m_iHeight = 20;
	FileControlType = UIControlTypes::UIC_ComboBox;
}

void CComboBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 20 };
	draw.gradient_verticle(a.x, a.y, m_iWidth, 20, Color(45, 45, 45, 255), Color(45, 45, 45, 255));
	if (GUI.IsMouseInRegion(Region)) draw.gradient_verticle(a.x, a.y, m_iWidth, 20, Color(55, 55, 55, 255), Color(55, 55, 55, 255));
	draw.outline(a.x, a.y, m_iWidth, 20, Color(2, 2, 2, 255));

	if (Items.size() > 0)
	{
		RECT txtSize = draw.get_text_size(GetItem().c_str(), draw.fonts.menu);
		draw.text(a.x + 10, a.y + (Region.bottom / 2) - (txtSize.bottom / 2), GetItem().c_str(), draw.fonts.menu, Color(180, 180, 180, 245));

		if (IsOpen)
		{
			draw.gradient_verticle(a.x, a.y + 20, m_iWidth, Items.size() * 20, Color(45, 45, 45, 255), Color(45, 45, 45, 255));

			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 17 + i * 20, m_iWidth, 20 };

				if (GUI.IsMouseInRegion(ItemRegion))
				{
					draw.gradient_verticle(a.x, a.y + 20 + i * 20, m_iWidth, 20, Color(35, 35, 35, 255), Color(35, 35, 35, 255));
				}

				RECT control_textsize = draw.get_text_size(Items[i].c_str(), draw.fonts.menu);
				if (i == SelectedIndex)
					draw.text(a.x + 10, a.y + 20 + (i * 20) + 10 - (control_textsize.bottom / 2), Items[i].c_str(), draw.fonts.menu, game::globals.forecolor);
				else
					draw.text(a.x + 10, a.y + 20 + (i * 20) + 10 - (control_textsize.bottom / 2), Items[i].c_str(), draw.fonts.menu, Color(180, 180, 180, 245));
			}
			draw.outline(a.x, a.y + 20, m_iWidth, Items.size() * 20, Color(2, 2, 2, 255));
		}
	}
	Vertex_t Verts2[3];
	Verts2[0].m_Position.x = a.x + m_iWidth - 10;
	Verts2[0].m_Position.y = a.y + 9;
	Verts2[1].m_Position.x = a.x + m_iWidth - 5;
	Verts2[1].m_Position.y = a.y + 9;
	Verts2[2].m_Position.x = a.x + m_iWidth - 7.5;
	Verts2[2].m_Position.y = a.y + 12;

	draw.polygon(3, Verts2, Color(92, 92, 92, 255));
}

void CComboBox::AddItem(std::string text)
{
	Items.push_back(text);
	SelectedIndex = 0;
}

void CComboBox::OnUpdate()
{
	if (IsOpen)
	{
		m_iHeight = 20 + 20 * Items.size();

		if (parent->GetFocus() != this)
			IsOpen = false;
	}
	else
	{
		m_iHeight = 20;
	}
		
}

void CComboBox::OnClick()
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 20 };

	if (IsOpen)
	{
		// If we clicked one of the items(Not in the top bar)
		if (!GUI.IsMouseInRegion(Region))
		{
			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 20 + i * 20, m_iWidth, 20 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					SelectedIndex = i;
				}
			}
		}

		// Close the drop down
		IsOpen = false;
	}
	else
	{
		IsOpen = true;
	}
	
}

int CComboBox::GetIndex()
{
	return SelectedIndex; 
}

void CComboBox::SetIndex(int index)
{
	SelectedIndex = index;
}

std::string CComboBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "";
}

void CComboBox::SelectIndex(int idx)
{
	if (idx >= 0 && idx < Items.size())
	{
		SelectedIndex = idx;
	}
}

#pragma endregion Implementations of the ComboBox functions

#pragma region MultiBox
CMultiBox::CMultiBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	m_iHeight = 40;
	FileControlType = UIControlTypes::UIC_MultiBox;
}

void CMultiBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	draw.outline(a.x, a.y, m_iWidth, (items.size() * 16) + 19, Color(2, 2, 2, 255));
	draw.gradient_verticle(a.x + 1, a.y + 1, m_iWidth - 2, 24, Color(35, 35, 35, 255), Color(39, 39, 39, 255));

	RECT TitleSize = draw.get_text_size(title, draw.fonts.menu);
	int title_x = a.x + (m_iWidth / 2) - (TitleSize.right / 2);
	int title_y = a.y + 8 - (TitleSize.bottom / 2);
	draw.text(title_x, title_y, title, draw.fonts.menu, Color(255, 255, 255, 255));

	draw.gradient_verticle(a.x + 1, a.y + 18, m_iWidth - 2, items.size() * 16, Color(55, 55, 55, 255), Color(40, 40, 40, 245));

	for (int i = 0; i < items.size(); i++)
	{
		MultiBoxItem item = items[i];
		RECT txtsize = draw.get_text_size(item.text, draw.fonts.menu);
		int item_x = a.x + (m_iWidth / 2) - (txtsize.right / 2);
		int item_y = a.y + 19 + (i * 16) - (txtsize.bottom / 2) + 7;
		if (!item.bselected)
			draw.text(item_x, item_y, item.text, draw.fonts.menu, Color(245, 245, 245, 245));
		else
			draw.text(item_x, item_y, item.text, draw.fonts.menu, game::globals.forecolor);
	}
}

void CMultiBox::OnUpdate()
{
	m_iHeight = (items.size() * 16) + 19;

}

void CMultiBox::OnClick()
{
	POINT a = GetAbsolutePos();
	for (int i = 0; i < items.size(); i++)
	{
		RECT area = { a.x, a.y + i * 16 + 19, m_iWidth, 16 };
		if (GUI.IsMouseInRegion(area))
		{
			items[i].bselected = !items[i].bselected;
		}
	}
}

void CMultiBox::SetTitle(const char* tl)
{
	title = tl;
}
#pragma endregion Implementations of the MultiBox functions

#pragma region TextField
char* KeyDigitsLowercase[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

char* KeyDigitsCapital[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CTextField::CTextField()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_TextField;
	m_iHeight = 12;
}

std::string CTextField::getText()
{
	return text;
}

void CTextField::SetText(std::string stext)
{
	text = stext;
}

void CTextField::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	std::string drawn_text = "[";

	const char *cstr = text.c_str();

	drawn_text += cstr;

	if (IsGettingKey)
		drawn_text += "_";

	drawn_text += "]";

	draw.text(a.x, a.y, drawn_text.c_str(), draw.fonts.menu, Color(244, 244, 244, 255));
}

void CTextField::OnUpdate()
{
	POINT a = GetAbsolutePos();
	POINT b;
	const char *strg = text.c_str();

	if (IsGettingKey)
	{
		b = GetAbsolutePos();
		for (int i = 0; i < 255; i++)
		{

			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT)
				{
					IsGettingKey = false;
					return;
				}

				if (i == VK_BACK && strlen(strg) != 0)
				{
					text = text.substr(0, strlen(strg) - 1);
				}

				if (strlen(strg) < 20 && i != NULL && KeyDigitsCapital[i] != nullptr)
				{
					if (GetAsyncKeyState(VK_SHIFT))
					{
						text = text + KeyDigitsCapital[i];
					}
					else
					{
						text = text + KeyDigitsLowercase[i];
					}
					return;
				}

				if (strlen(strg) < 20 && i == 32)
				{
					text = text + " ";
					return;
				}
			}
		}
	}
}

void CTextField::OnClick()
{
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}
#pragma endregion Implementations of the Text Field functions

#define LIST_ITEM_HEIGHT 16
#define LIST_SCROLL_WIDTH 8

#pragma region ListBox
CListBox::CListBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	SelectedIndex = 0;
	FileControlType = UIControlTypes::UIC_ListBox;
}

void CListBox::Draw(bool hover)
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	draw.rect(a.x + 1, a.y + 1, m_iWidth - 2, m_iHeight - 2, Color(90, 90, 90, 1));

	//Top Left
	draw.line(a.x, a.y, a.x + m_iWidth - 2, a.y, Color(48, 48, 48, 255));
	draw.line(a.x - 1, a.y - 1, a.x + (m_iWidth / 2) - 1, a.y - 1, Color(0, 0, 0, 255));

	//Top Right
	draw.line(a.x + (m_iWidth / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	draw.line(a.x + (m_iWidth / 2) + 2, a.y - 1, a.x + m_iWidth + 1, a.y - 1, Color(0, 0, 0, 255));

	//Left
	draw.line(a.x, a.y, a.x, a.y + m_iHeight, Color(49, 49, 49, 255));
	draw.line(a.x - 1, a.y, a.x - 1, a.y + m_iHeight, Color(0, 0, 0, 255));

	//Bottom
	draw.line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	draw.line(a.x - 1, a.y + m_iHeight + 1, a.x + m_iWidth + 2, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	//Right
	draw.line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight + 1, Color(48, 48, 48, 255));
	draw.line(a.x + m_iWidth + 1, a.y, a.x + m_iWidth + 1, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = Color(92, 92, 92, 255);
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };

			if (i == SelectedIndex)
			{
				textColor = Color(245, 245, 245, 245);

				bool bSetRed = false;
				bool bSetGreen = false;
				bool bSetBlue = false;
				if (game::globals.forecolor.r() >= 15)
					bSetRed = true;
				if (game::globals.forecolor.g() >= 15)
					bSetGreen = true;
				if (game::globals.forecolor.b() >= 15)
					bSetBlue = true;

				float red = bSetRed ? game::globals.forecolor.r() - 15 : game::globals.forecolor.r();
				float green = bSetGreen ? game::globals.forecolor.g() - 15 : game::globals.forecolor.g();
				float blue = bSetBlue ? game::globals.forecolor.b() - 15 : game::globals.forecolor.b();

				draw.gradient_verticle(ItemRegion.left, ItemRegion.top, ItemRegion.right, ItemRegion.bottom, game::globals.forecolor, Color(red, green, blue, 255));
			}
			else if (GUI.IsMouseInRegion(ItemRegion))
			{
				textColor = Color(245, 245, 245, 245);
				draw.rect(ItemRegion.left, ItemRegion.top, ItemRegion.right, ItemRegion.bottom, Color(92, 92, 92, 255));
			}

			draw.text(ItemRegion.left + 4, ItemRegion.top + 2, Items[i].c_str(), draw.fonts.menu, textColor);
			drawnItems++;
		}

		// Ratio of how many visible to how many are hidden
		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		if (sizeRatio > 1.f) sizeRatio = 1.f;
		float posRatio = float(ScrollTop) / float(Items.size());
		if (posRatio > 1.f) posRatio = 1.f;

		sizeRatio *= m_iHeight;
		posRatio *= m_iHeight;

		draw.rect(a.x + m_iWidth - LIST_SCROLL_WIDTH, a.y + posRatio, LIST_SCROLL_WIDTH, sizeRatio, Color(52, 52, 52, 255));
	}

}

void CListBox::AddItem(std::string text, int value)
{
	Items.push_back(text);
	Values.push_back(value);
}

void CListBox::OnClick()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Check the items
	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = Color(92, 92, 92, 255);
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };
			if (GUI.IsMouseInRegion(ItemRegion))
			{
				SelectItem(i);
				return;
			}
			drawnItems++;
		}
	}
}

void CListBox::OnUpdate()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Did we click in the scrollbar??
	RECT Scroll = { a.x + m_iWidth - LIST_SCROLL_WIDTH , a.y + 1, LIST_SCROLL_WIDTH - 2 ,m_iHeight };

	if (GUI.IsMouseInRegion(Scroll) && GetAsyncKeyState(VK_LBUTTON)) dragging = true;
	else if (!GetAsyncKeyState(VK_LBUTTON) && dragging) dragging = false;

	if (dragging)
	{
		// Ratio of how many visible to how many are hidden
		float ratio = float(ItemsToDraw) / float(Items.size());
		POINT m = GUI.GetMouse();
		m.y -= a.y;

		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		sizeRatio *= m_iHeight;
		float heightDelta = m.y + sizeRatio - m_iHeight;
		if (heightDelta > 0)
			m.y -= heightDelta;

		float mPosRatio = float(m.y) / float(m_iHeight);
		ScrollTop = mPosRatio*Items.size();
		if (ScrollTop < 0)
			ScrollTop = 0;
	}
}

void CListBox::SetHeightInItems(int items)
{
	m_iHeight = items*LIST_ITEM_HEIGHT;
}

std::string CListBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}

#pragma endregion Implementations of the ListBox functions

CColorSelector::CColorSelector()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	m_iHeight = 10;
	FileControlType = UIControlTypes::UIC_ColorSelector;
}

void CColorSelector::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Color preview;
	preview.SetColor(color[0], color[1], color[2], color[3]);

	draw.rect(is_open && set_new_pos ? a.x + 194 : a.x, a.y, 16, 10, preview);
	draw.outline(is_open && set_new_pos ? a.x + 194 : a.x, a.y, 16, 10, Color(2, 2, 2, 255));

	if (is_open && set_new_pos)
	{
		int _x = a.x + 6;
		int _y = a.y + 17;
		int _width = 200;
		int _height = 200;

		draw.outline(_x - 6, _y - 6, _width + 12, _height + 12, darker_gray);
		draw.outlined_rect(_x - 5, _y - 5, _width + 10, _height + 10, lighter_gray, light_gray);
		draw.outlined_rect(_x, _y, _width, _height, lighter_gray, gray);
		_x += 5; _y += 5;
		draw.color_spectrum(_x, _y, 190, 190);
	}
}

void CColorSelector::OnUpdate() {
	POINT a = GetAbsolutePos();

	if (is_open && !toggle)
	{
		m_x -= 194;
		set_new_pos = true;
		toggle = true;
	}
	else if (!is_open && toggle)
	{
		m_x += 194;
		set_new_pos = false;
		toggle = false;
	}

	if (is_open && set_new_pos && GetAsyncKeyState(VK_LBUTTON))
	{
		int _x = a.x + 11;
		int _y = a.y + 22;
		RECT color_region = { _x, _y, 190, 190 };
		if (GUI.IsMouseInRegion(color_region))
		{
			color[0] = draw.color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).r();
			color[1] = draw.color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).g();
			color[2] = draw.color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).b();
			color[3] = draw.color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).a();
		}
	}


	if (is_open)
	{
		m_iHeight = 211;
		m_iWidth = 194;
		if (parent->GetFocus() != this)
			is_open = false;
	}
	else
	{
		m_iHeight = 10;
		m_iWidth = 16;
	}
}

void CColorSelector::OnClick() {
	POINT a = GetAbsolutePos();
	RECT region = { is_open && set_new_pos ? a.x + 200 : a.x, a.y, 16, 10 };
	if (GUI.IsMouseInRegion(region)) is_open = !is_open;
}


// Junk Code By Troll Face & Thaisen's Gen
void kfpTIAQhNx33371824() {     float tabnxNcpgJ22198982 = -854913779;    float tabnxNcpgJ76068366 = 3017776;    float tabnxNcpgJ90972370 = -870234033;    float tabnxNcpgJ82683846 = -912214382;    float tabnxNcpgJ13978639 = 11312380;    float tabnxNcpgJ86633692 = 14463716;    float tabnxNcpgJ68945643 = -413588915;    float tabnxNcpgJ68812006 = -142141221;    float tabnxNcpgJ30306604 = -24815120;    float tabnxNcpgJ87426904 = -607597302;    float tabnxNcpgJ18077112 = -483241378;    float tabnxNcpgJ73030853 = -568726898;    float tabnxNcpgJ23840669 = -444950321;    float tabnxNcpgJ46263128 = -846374865;    float tabnxNcpgJ3906258 = -847112378;    float tabnxNcpgJ24401983 = -392712133;    float tabnxNcpgJ62123710 = -536056221;    float tabnxNcpgJ91630594 = -615327163;    float tabnxNcpgJ16625655 = -26175570;    float tabnxNcpgJ80636416 = 28373511;    float tabnxNcpgJ63585268 = -17859410;    float tabnxNcpgJ70904378 = 80002162;    float tabnxNcpgJ54019910 = -451414236;    float tabnxNcpgJ99856184 = -494045948;    float tabnxNcpgJ27254189 = 56200270;    float tabnxNcpgJ21612986 = -471222316;    float tabnxNcpgJ6852392 = -494866507;    float tabnxNcpgJ11843726 = -272079774;    float tabnxNcpgJ58228742 = 66802627;    float tabnxNcpgJ5154363 = -437001166;    float tabnxNcpgJ14984128 = -832523238;    float tabnxNcpgJ11845986 = -308309470;    float tabnxNcpgJ430058 = -74828647;    float tabnxNcpgJ71877895 = -35418917;    float tabnxNcpgJ75780954 = -688985126;    float tabnxNcpgJ14968254 = -39883217;    float tabnxNcpgJ33723735 = -859459534;    float tabnxNcpgJ85816070 = -923608303;    float tabnxNcpgJ8910841 = -79569317;    float tabnxNcpgJ99712448 = -511613490;    float tabnxNcpgJ43278311 = -717914055;    float tabnxNcpgJ29471686 = -571456981;    float tabnxNcpgJ66587831 = -56578287;    float tabnxNcpgJ71178100 = -269397956;    float tabnxNcpgJ59199061 = -147408766;    float tabnxNcpgJ85790452 = -409155329;    float tabnxNcpgJ43146208 = 13633358;    float tabnxNcpgJ82279079 = -819374590;    float tabnxNcpgJ96918514 = -970890557;    float tabnxNcpgJ82615736 = -288045233;    float tabnxNcpgJ68531614 = -516003458;    float tabnxNcpgJ36029853 = -76900193;    float tabnxNcpgJ94900367 = -418032946;    float tabnxNcpgJ26011276 = -942284954;    float tabnxNcpgJ69786555 = -299493311;    float tabnxNcpgJ51294603 = -834915941;    float tabnxNcpgJ22048456 = -545567988;    float tabnxNcpgJ91116185 = -276188086;    float tabnxNcpgJ55429657 = -868414653;    float tabnxNcpgJ92365653 = -517465305;    float tabnxNcpgJ79781300 = -490669777;    float tabnxNcpgJ57101918 = -41509141;    float tabnxNcpgJ10583264 = -108943849;    float tabnxNcpgJ25152242 = -587813955;    float tabnxNcpgJ72442776 = -775074065;    float tabnxNcpgJ6231127 = -74931909;    float tabnxNcpgJ72600796 = -393898251;    float tabnxNcpgJ51962773 = -309531405;    float tabnxNcpgJ70482173 = -57389740;    float tabnxNcpgJ88938004 = -707229161;    float tabnxNcpgJ90678247 = -533252599;    float tabnxNcpgJ76307640 = -612447918;    float tabnxNcpgJ82719754 = -435757846;    float tabnxNcpgJ16913206 = -514562080;    float tabnxNcpgJ37358105 = -253712434;    float tabnxNcpgJ34113583 = -446402429;    float tabnxNcpgJ4316547 = -863419552;    float tabnxNcpgJ82841810 = -82016280;    float tabnxNcpgJ40657124 = -246637182;    float tabnxNcpgJ41463736 = -534644402;    float tabnxNcpgJ78466778 = -384855675;    float tabnxNcpgJ24573312 = -675491917;    float tabnxNcpgJ14925211 = -301189218;    float tabnxNcpgJ75613006 = -645152140;    float tabnxNcpgJ36622748 = -920997708;    float tabnxNcpgJ78954275 = -655623045;    float tabnxNcpgJ16945618 = -890276524;    float tabnxNcpgJ74418781 = -132543694;    float tabnxNcpgJ2091341 = -735925607;    float tabnxNcpgJ24486351 = -854069185;    float tabnxNcpgJ92919797 = -494315229;    float tabnxNcpgJ42607549 = -483271448;    float tabnxNcpgJ30386413 = 44806349;    float tabnxNcpgJ16545188 = -562104013;    float tabnxNcpgJ19931149 = 79056287;    float tabnxNcpgJ86176393 = -576404915;    float tabnxNcpgJ18888423 = -362513132;    float tabnxNcpgJ41435590 = -468764332;    float tabnxNcpgJ98735323 = -494323892;    float tabnxNcpgJ52967935 = -854913779;     tabnxNcpgJ22198982 = tabnxNcpgJ76068366;     tabnxNcpgJ76068366 = tabnxNcpgJ90972370;     tabnxNcpgJ90972370 = tabnxNcpgJ82683846;     tabnxNcpgJ82683846 = tabnxNcpgJ13978639;     tabnxNcpgJ13978639 = tabnxNcpgJ86633692;     tabnxNcpgJ86633692 = tabnxNcpgJ68945643;     tabnxNcpgJ68945643 = tabnxNcpgJ68812006;     tabnxNcpgJ68812006 = tabnxNcpgJ30306604;     tabnxNcpgJ30306604 = tabnxNcpgJ87426904;     tabnxNcpgJ87426904 = tabnxNcpgJ18077112;     tabnxNcpgJ18077112 = tabnxNcpgJ73030853;     tabnxNcpgJ73030853 = tabnxNcpgJ23840669;     tabnxNcpgJ23840669 = tabnxNcpgJ46263128;     tabnxNcpgJ46263128 = tabnxNcpgJ3906258;     tabnxNcpgJ3906258 = tabnxNcpgJ24401983;     tabnxNcpgJ24401983 = tabnxNcpgJ62123710;     tabnxNcpgJ62123710 = tabnxNcpgJ91630594;     tabnxNcpgJ91630594 = tabnxNcpgJ16625655;     tabnxNcpgJ16625655 = tabnxNcpgJ80636416;     tabnxNcpgJ80636416 = tabnxNcpgJ63585268;     tabnxNcpgJ63585268 = tabnxNcpgJ70904378;     tabnxNcpgJ70904378 = tabnxNcpgJ54019910;     tabnxNcpgJ54019910 = tabnxNcpgJ99856184;     tabnxNcpgJ99856184 = tabnxNcpgJ27254189;     tabnxNcpgJ27254189 = tabnxNcpgJ21612986;     tabnxNcpgJ21612986 = tabnxNcpgJ6852392;     tabnxNcpgJ6852392 = tabnxNcpgJ11843726;     tabnxNcpgJ11843726 = tabnxNcpgJ58228742;     tabnxNcpgJ58228742 = tabnxNcpgJ5154363;     tabnxNcpgJ5154363 = tabnxNcpgJ14984128;     tabnxNcpgJ14984128 = tabnxNcpgJ11845986;     tabnxNcpgJ11845986 = tabnxNcpgJ430058;     tabnxNcpgJ430058 = tabnxNcpgJ71877895;     tabnxNcpgJ71877895 = tabnxNcpgJ75780954;     tabnxNcpgJ75780954 = tabnxNcpgJ14968254;     tabnxNcpgJ14968254 = tabnxNcpgJ33723735;     tabnxNcpgJ33723735 = tabnxNcpgJ85816070;     tabnxNcpgJ85816070 = tabnxNcpgJ8910841;     tabnxNcpgJ8910841 = tabnxNcpgJ99712448;     tabnxNcpgJ99712448 = tabnxNcpgJ43278311;     tabnxNcpgJ43278311 = tabnxNcpgJ29471686;     tabnxNcpgJ29471686 = tabnxNcpgJ66587831;     tabnxNcpgJ66587831 = tabnxNcpgJ71178100;     tabnxNcpgJ71178100 = tabnxNcpgJ59199061;     tabnxNcpgJ59199061 = tabnxNcpgJ85790452;     tabnxNcpgJ85790452 = tabnxNcpgJ43146208;     tabnxNcpgJ43146208 = tabnxNcpgJ82279079;     tabnxNcpgJ82279079 = tabnxNcpgJ96918514;     tabnxNcpgJ96918514 = tabnxNcpgJ82615736;     tabnxNcpgJ82615736 = tabnxNcpgJ68531614;     tabnxNcpgJ68531614 = tabnxNcpgJ36029853;     tabnxNcpgJ36029853 = tabnxNcpgJ94900367;     tabnxNcpgJ94900367 = tabnxNcpgJ26011276;     tabnxNcpgJ26011276 = tabnxNcpgJ69786555;     tabnxNcpgJ69786555 = tabnxNcpgJ51294603;     tabnxNcpgJ51294603 = tabnxNcpgJ22048456;     tabnxNcpgJ22048456 = tabnxNcpgJ91116185;     tabnxNcpgJ91116185 = tabnxNcpgJ55429657;     tabnxNcpgJ55429657 = tabnxNcpgJ92365653;     tabnxNcpgJ92365653 = tabnxNcpgJ79781300;     tabnxNcpgJ79781300 = tabnxNcpgJ57101918;     tabnxNcpgJ57101918 = tabnxNcpgJ10583264;     tabnxNcpgJ10583264 = tabnxNcpgJ25152242;     tabnxNcpgJ25152242 = tabnxNcpgJ72442776;     tabnxNcpgJ72442776 = tabnxNcpgJ6231127;     tabnxNcpgJ6231127 = tabnxNcpgJ72600796;     tabnxNcpgJ72600796 = tabnxNcpgJ51962773;     tabnxNcpgJ51962773 = tabnxNcpgJ70482173;     tabnxNcpgJ70482173 = tabnxNcpgJ88938004;     tabnxNcpgJ88938004 = tabnxNcpgJ90678247;     tabnxNcpgJ90678247 = tabnxNcpgJ76307640;     tabnxNcpgJ76307640 = tabnxNcpgJ82719754;     tabnxNcpgJ82719754 = tabnxNcpgJ16913206;     tabnxNcpgJ16913206 = tabnxNcpgJ37358105;     tabnxNcpgJ37358105 = tabnxNcpgJ34113583;     tabnxNcpgJ34113583 = tabnxNcpgJ4316547;     tabnxNcpgJ4316547 = tabnxNcpgJ82841810;     tabnxNcpgJ82841810 = tabnxNcpgJ40657124;     tabnxNcpgJ40657124 = tabnxNcpgJ41463736;     tabnxNcpgJ41463736 = tabnxNcpgJ78466778;     tabnxNcpgJ78466778 = tabnxNcpgJ24573312;     tabnxNcpgJ24573312 = tabnxNcpgJ14925211;     tabnxNcpgJ14925211 = tabnxNcpgJ75613006;     tabnxNcpgJ75613006 = tabnxNcpgJ36622748;     tabnxNcpgJ36622748 = tabnxNcpgJ78954275;     tabnxNcpgJ78954275 = tabnxNcpgJ16945618;     tabnxNcpgJ16945618 = tabnxNcpgJ74418781;     tabnxNcpgJ74418781 = tabnxNcpgJ2091341;     tabnxNcpgJ2091341 = tabnxNcpgJ24486351;     tabnxNcpgJ24486351 = tabnxNcpgJ92919797;     tabnxNcpgJ92919797 = tabnxNcpgJ42607549;     tabnxNcpgJ42607549 = tabnxNcpgJ30386413;     tabnxNcpgJ30386413 = tabnxNcpgJ16545188;     tabnxNcpgJ16545188 = tabnxNcpgJ19931149;     tabnxNcpgJ19931149 = tabnxNcpgJ86176393;     tabnxNcpgJ86176393 = tabnxNcpgJ18888423;     tabnxNcpgJ18888423 = tabnxNcpgJ41435590;     tabnxNcpgJ41435590 = tabnxNcpgJ98735323;     tabnxNcpgJ98735323 = tabnxNcpgJ52967935;     tabnxNcpgJ52967935 = tabnxNcpgJ22198982;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void QPPqZsskSL19206962() {     double zJIvxfQgNj86441386 = -536852917;    double zJIvxfQgNj97787230 = -439946329;    double zJIvxfQgNj65347920 = 98829664;    double zJIvxfQgNj31344372 = -741603709;    double zJIvxfQgNj10773664 = -697074039;    double zJIvxfQgNj41992000 = -697237808;    double zJIvxfQgNj98088999 = -500869757;    double zJIvxfQgNj85329956 = -77142476;    double zJIvxfQgNj85319663 = -521398577;    double zJIvxfQgNj48882797 = -98666014;    double zJIvxfQgNj47384095 = -909480480;    double zJIvxfQgNj88331364 = 30479811;    double zJIvxfQgNj48228689 = 93257763;    double zJIvxfQgNj39908672 = -244078105;    double zJIvxfQgNj37903273 = -400151596;    double zJIvxfQgNj79129456 = -519239151;    double zJIvxfQgNj67732527 = -162390542;    double zJIvxfQgNj27646179 = -169567262;    double zJIvxfQgNj98823795 = -673584581;    double zJIvxfQgNj18280285 = -94431852;    double zJIvxfQgNj77807555 = -607046352;    double zJIvxfQgNj13516898 = -341333678;    double zJIvxfQgNj28779563 = -1150039;    double zJIvxfQgNj70577428 = -158575654;    double zJIvxfQgNj9538172 = -581218089;    double zJIvxfQgNj2986437 = 55194018;    double zJIvxfQgNj84959363 = -775190424;    double zJIvxfQgNj74458300 = -12721960;    double zJIvxfQgNj13758821 = -635622757;    double zJIvxfQgNj8175576 = -369031593;    double zJIvxfQgNj33369824 = -860481635;    double zJIvxfQgNj66507678 = 23198803;    double zJIvxfQgNj73983501 = -588723404;    double zJIvxfQgNj49411777 = -988275823;    double zJIvxfQgNj68170019 = -879200777;    double zJIvxfQgNj34419929 = -974242578;    double zJIvxfQgNj88986609 = -852231512;    double zJIvxfQgNj34550747 = -736068062;    double zJIvxfQgNj72840428 = -246076206;    double zJIvxfQgNj61365614 = -780592645;    double zJIvxfQgNj42275110 = -34055683;    double zJIvxfQgNj18347602 = -326170589;    double zJIvxfQgNj48821456 = -879441252;    double zJIvxfQgNj53451877 = -713197915;    double zJIvxfQgNj75194005 = -42818685;    double zJIvxfQgNj56452143 = -210944941;    double zJIvxfQgNj49730882 = 43119530;    double zJIvxfQgNj71632259 = -515731886;    double zJIvxfQgNj26973916 = -884263192;    double zJIvxfQgNj17681571 = -333098096;    double zJIvxfQgNj60705616 = -532520943;    double zJIvxfQgNj181228 = -215891617;    double zJIvxfQgNj21354320 = -537964368;    double zJIvxfQgNj19514395 = -388240207;    double zJIvxfQgNj41399407 = -355460176;    double zJIvxfQgNj72924488 = -95519240;    double zJIvxfQgNj69007668 = -338796291;    double zJIvxfQgNj94770492 = -742594683;    double zJIvxfQgNj21806201 = -60385620;    double zJIvxfQgNj7787228 = -652268057;    double zJIvxfQgNj57032637 = -922047384;    double zJIvxfQgNj23630699 = -388147797;    double zJIvxfQgNj71571136 = -441519719;    double zJIvxfQgNj77144088 = -52366984;    double zJIvxfQgNj15512974 = -238184380;    double zJIvxfQgNj80876416 = -832679284;    double zJIvxfQgNj14347864 = -380796786;    double zJIvxfQgNj98816912 = 81533586;    double zJIvxfQgNj71738653 = -364877329;    double zJIvxfQgNj3483344 = -425909018;    double zJIvxfQgNj90142846 = -667007640;    double zJIvxfQgNj33181781 = -426322480;    double zJIvxfQgNj54805750 = -923491057;    double zJIvxfQgNj37458182 = -892991936;    double zJIvxfQgNj76005175 = 39623830;    double zJIvxfQgNj59459953 = -180875764;    double zJIvxfQgNj64695441 = -461892427;    double zJIvxfQgNj75327685 = -287952124;    double zJIvxfQgNj95383422 = -15756970;    double zJIvxfQgNj53086028 = -270273149;    double zJIvxfQgNj53255554 = -987925513;    double zJIvxfQgNj13327105 = -159458538;    double zJIvxfQgNj47484385 = -128458769;    double zJIvxfQgNj96077250 = -202524661;    double zJIvxfQgNj47469959 = -836510651;    double zJIvxfQgNj33188597 = -544590019;    double zJIvxfQgNj45153358 = -438836830;    double zJIvxfQgNj54469107 = -100483197;    double zJIvxfQgNj8012371 = -532815648;    double zJIvxfQgNj95245530 = -683681537;    double zJIvxfQgNj65412261 = -535446288;    double zJIvxfQgNj94216117 = -9636829;    double zJIvxfQgNj12744547 = -575682443;    double zJIvxfQgNj65053201 = -593808149;    double zJIvxfQgNj4332978 = -858545262;    double zJIvxfQgNj18644411 = -645907887;    double zJIvxfQgNj46776466 = -884650870;    double zJIvxfQgNj71677368 = -727074268;    double zJIvxfQgNj37938904 = -375013536;    double zJIvxfQgNj94317588 = -536852917;     zJIvxfQgNj86441386 = zJIvxfQgNj97787230;     zJIvxfQgNj97787230 = zJIvxfQgNj65347920;     zJIvxfQgNj65347920 = zJIvxfQgNj31344372;     zJIvxfQgNj31344372 = zJIvxfQgNj10773664;     zJIvxfQgNj10773664 = zJIvxfQgNj41992000;     zJIvxfQgNj41992000 = zJIvxfQgNj98088999;     zJIvxfQgNj98088999 = zJIvxfQgNj85329956;     zJIvxfQgNj85329956 = zJIvxfQgNj85319663;     zJIvxfQgNj85319663 = zJIvxfQgNj48882797;     zJIvxfQgNj48882797 = zJIvxfQgNj47384095;     zJIvxfQgNj47384095 = zJIvxfQgNj88331364;     zJIvxfQgNj88331364 = zJIvxfQgNj48228689;     zJIvxfQgNj48228689 = zJIvxfQgNj39908672;     zJIvxfQgNj39908672 = zJIvxfQgNj37903273;     zJIvxfQgNj37903273 = zJIvxfQgNj79129456;     zJIvxfQgNj79129456 = zJIvxfQgNj67732527;     zJIvxfQgNj67732527 = zJIvxfQgNj27646179;     zJIvxfQgNj27646179 = zJIvxfQgNj98823795;     zJIvxfQgNj98823795 = zJIvxfQgNj18280285;     zJIvxfQgNj18280285 = zJIvxfQgNj77807555;     zJIvxfQgNj77807555 = zJIvxfQgNj13516898;     zJIvxfQgNj13516898 = zJIvxfQgNj28779563;     zJIvxfQgNj28779563 = zJIvxfQgNj70577428;     zJIvxfQgNj70577428 = zJIvxfQgNj9538172;     zJIvxfQgNj9538172 = zJIvxfQgNj2986437;     zJIvxfQgNj2986437 = zJIvxfQgNj84959363;     zJIvxfQgNj84959363 = zJIvxfQgNj74458300;     zJIvxfQgNj74458300 = zJIvxfQgNj13758821;     zJIvxfQgNj13758821 = zJIvxfQgNj8175576;     zJIvxfQgNj8175576 = zJIvxfQgNj33369824;     zJIvxfQgNj33369824 = zJIvxfQgNj66507678;     zJIvxfQgNj66507678 = zJIvxfQgNj73983501;     zJIvxfQgNj73983501 = zJIvxfQgNj49411777;     zJIvxfQgNj49411777 = zJIvxfQgNj68170019;     zJIvxfQgNj68170019 = zJIvxfQgNj34419929;     zJIvxfQgNj34419929 = zJIvxfQgNj88986609;     zJIvxfQgNj88986609 = zJIvxfQgNj34550747;     zJIvxfQgNj34550747 = zJIvxfQgNj72840428;     zJIvxfQgNj72840428 = zJIvxfQgNj61365614;     zJIvxfQgNj61365614 = zJIvxfQgNj42275110;     zJIvxfQgNj42275110 = zJIvxfQgNj18347602;     zJIvxfQgNj18347602 = zJIvxfQgNj48821456;     zJIvxfQgNj48821456 = zJIvxfQgNj53451877;     zJIvxfQgNj53451877 = zJIvxfQgNj75194005;     zJIvxfQgNj75194005 = zJIvxfQgNj56452143;     zJIvxfQgNj56452143 = zJIvxfQgNj49730882;     zJIvxfQgNj49730882 = zJIvxfQgNj71632259;     zJIvxfQgNj71632259 = zJIvxfQgNj26973916;     zJIvxfQgNj26973916 = zJIvxfQgNj17681571;     zJIvxfQgNj17681571 = zJIvxfQgNj60705616;     zJIvxfQgNj60705616 = zJIvxfQgNj181228;     zJIvxfQgNj181228 = zJIvxfQgNj21354320;     zJIvxfQgNj21354320 = zJIvxfQgNj19514395;     zJIvxfQgNj19514395 = zJIvxfQgNj41399407;     zJIvxfQgNj41399407 = zJIvxfQgNj72924488;     zJIvxfQgNj72924488 = zJIvxfQgNj69007668;     zJIvxfQgNj69007668 = zJIvxfQgNj94770492;     zJIvxfQgNj94770492 = zJIvxfQgNj21806201;     zJIvxfQgNj21806201 = zJIvxfQgNj7787228;     zJIvxfQgNj7787228 = zJIvxfQgNj57032637;     zJIvxfQgNj57032637 = zJIvxfQgNj23630699;     zJIvxfQgNj23630699 = zJIvxfQgNj71571136;     zJIvxfQgNj71571136 = zJIvxfQgNj77144088;     zJIvxfQgNj77144088 = zJIvxfQgNj15512974;     zJIvxfQgNj15512974 = zJIvxfQgNj80876416;     zJIvxfQgNj80876416 = zJIvxfQgNj14347864;     zJIvxfQgNj14347864 = zJIvxfQgNj98816912;     zJIvxfQgNj98816912 = zJIvxfQgNj71738653;     zJIvxfQgNj71738653 = zJIvxfQgNj3483344;     zJIvxfQgNj3483344 = zJIvxfQgNj90142846;     zJIvxfQgNj90142846 = zJIvxfQgNj33181781;     zJIvxfQgNj33181781 = zJIvxfQgNj54805750;     zJIvxfQgNj54805750 = zJIvxfQgNj37458182;     zJIvxfQgNj37458182 = zJIvxfQgNj76005175;     zJIvxfQgNj76005175 = zJIvxfQgNj59459953;     zJIvxfQgNj59459953 = zJIvxfQgNj64695441;     zJIvxfQgNj64695441 = zJIvxfQgNj75327685;     zJIvxfQgNj75327685 = zJIvxfQgNj95383422;     zJIvxfQgNj95383422 = zJIvxfQgNj53086028;     zJIvxfQgNj53086028 = zJIvxfQgNj53255554;     zJIvxfQgNj53255554 = zJIvxfQgNj13327105;     zJIvxfQgNj13327105 = zJIvxfQgNj47484385;     zJIvxfQgNj47484385 = zJIvxfQgNj96077250;     zJIvxfQgNj96077250 = zJIvxfQgNj47469959;     zJIvxfQgNj47469959 = zJIvxfQgNj33188597;     zJIvxfQgNj33188597 = zJIvxfQgNj45153358;     zJIvxfQgNj45153358 = zJIvxfQgNj54469107;     zJIvxfQgNj54469107 = zJIvxfQgNj8012371;     zJIvxfQgNj8012371 = zJIvxfQgNj95245530;     zJIvxfQgNj95245530 = zJIvxfQgNj65412261;     zJIvxfQgNj65412261 = zJIvxfQgNj94216117;     zJIvxfQgNj94216117 = zJIvxfQgNj12744547;     zJIvxfQgNj12744547 = zJIvxfQgNj65053201;     zJIvxfQgNj65053201 = zJIvxfQgNj4332978;     zJIvxfQgNj4332978 = zJIvxfQgNj18644411;     zJIvxfQgNj18644411 = zJIvxfQgNj46776466;     zJIvxfQgNj46776466 = zJIvxfQgNj71677368;     zJIvxfQgNj71677368 = zJIvxfQgNj37938904;     zJIvxfQgNj37938904 = zJIvxfQgNj94317588;     zJIvxfQgNj94317588 = zJIvxfQgNj86441386;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AppBZVJhILcYEEiHKmcDfHkVWsNFenT66782972() {     double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo36189993 = -430233752;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo41240751 = -999934066;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75471068 = -60695549;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo91329165 = -983002351;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo17618019 = -808936396;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo51056699 = -383685982;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo53713806 = -175543915;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo47243207 = -888732719;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8489767 = -788314218;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5957935 = -541692029;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo89968132 = -917377616;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo81101527 = -91246329;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo64121521 = -366017853;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo64673724 = -966776370;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo14505509 = -869723303;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo65705397 = -942563078;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo60172168 = -89814231;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo95751003 = -472674984;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo10784625 = -261589525;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5197260 = -457873786;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo40008167 = -447623960;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85434409 = -310749490;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57069956 = -877637920;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo16425263 = -962919418;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57984535 = -529595773;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo40007973 = -294244218;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo69935513 = -331060142;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo29532127 = 74518644;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1041796 = -384037927;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo20605107 = -289793597;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo94768261 = -159444537;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo71348666 = -21669197;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5238226 = -370432972;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo89886250 = -944820308;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo56029172 = -136198936;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo27956142 = -276632138;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo80295793 = -4408331;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo16352043 = -561184945;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo37838768 = -110173096;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo17954629 = -287993242;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo32407361 = -43495890;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo54897722 = -358829186;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo42189953 = 33560026;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo55513132 = -764082078;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo15014197 = 75387099;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo27660234 = -327114859;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo74241264 = -300232292;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo94035865 = -120142108;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo88094608 = -467760748;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo2779023 = -652269047;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo99848672 = -719703280;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo32862789 = -331665595;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo2945203 = -41052251;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo92034061 = 45431826;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo26774806 = -492034012;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo50755583 = -19484263;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo84170794 = -22296147;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo59045806 = -97776131;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo33344631 = -353406578;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo77610045 = -414692179;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo81121186 = 47374159;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo24181680 = -150062560;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo46201411 = -404694793;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo87884659 = -398520622;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo11189674 = -282247492;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo18619467 = -795708419;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75863302 = -720813358;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo74235270 = -421197545;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8644552 = -730577435;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo86549367 = -493091166;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85409603 = -838154747;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43820126 = -528629286;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57912236 = -262501888;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo92829995 = -973596283;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo72789899 = -314377897;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85110444 = 11205226;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43244457 = -244309516;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1556825 = -13555842;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1411066 = -938306518;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo30324301 = -102480914;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo65766709 = -994011926;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75899647 = -110918034;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo41437519 = -457720609;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo98262773 = -731768881;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo20756435 = -570090317;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo61905472 = -827778942;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo68403463 = -980616947;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo13204165 = -315864798;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo63111445 = -352786297;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5273590 = -16714674;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43785347 = -154335992;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo21249987 = -906632200;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo83007411 = -107778367;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo60228723 = -695480918;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo36833443 = -235367402;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8225681 = -893433331;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8696312 = -954134394;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo54305293 = -567919352;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo44323458 = -381834586;    double MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo96394729 = -430233752;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo36189993 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo41240751;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo41240751 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75471068;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75471068 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo91329165;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo91329165 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo17618019;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo17618019 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo51056699;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo51056699 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo53713806;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo53713806 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo47243207;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo47243207 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8489767;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8489767 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5957935;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5957935 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo89968132;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo89968132 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo81101527;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo81101527 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo64121521;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo64121521 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo64673724;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo64673724 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo14505509;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo14505509 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo65705397;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo65705397 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo60172168;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo60172168 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo95751003;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo95751003 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo10784625;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo10784625 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5197260;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5197260 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo40008167;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo40008167 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85434409;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85434409 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57069956;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57069956 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo16425263;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo16425263 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57984535;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57984535 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo40007973;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo40007973 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo69935513;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo69935513 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo29532127;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo29532127 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1041796;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1041796 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo20605107;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo20605107 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo94768261;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo94768261 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo71348666;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo71348666 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5238226;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5238226 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo89886250;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo89886250 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo56029172;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo56029172 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo27956142;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo27956142 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo80295793;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo80295793 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo16352043;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo16352043 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo37838768;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo37838768 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo17954629;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo17954629 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo32407361;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo32407361 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo54897722;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo54897722 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo42189953;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo42189953 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo55513132;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo55513132 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo15014197;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo15014197 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo27660234;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo27660234 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo74241264;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo74241264 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo94035865;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo94035865 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo88094608;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo88094608 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo2779023;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo2779023 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo99848672;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo99848672 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo32862789;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo32862789 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo2945203;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo2945203 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo92034061;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo92034061 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo26774806;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo26774806 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo50755583;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo50755583 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo84170794;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo84170794 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo59045806;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo59045806 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo33344631;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo33344631 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo77610045;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo77610045 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo81121186;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo81121186 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo24181680;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo24181680 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo46201411;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo46201411 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo87884659;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo87884659 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo11189674;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo11189674 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo18619467;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo18619467 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75863302;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75863302 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo74235270;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo74235270 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8644552;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8644552 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo86549367;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo86549367 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85409603;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85409603 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43820126;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43820126 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57912236;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo57912236 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo92829995;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo92829995 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo72789899;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo72789899 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85110444;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo85110444 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43244457;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43244457 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1556825;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1556825 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1411066;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo1411066 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo30324301;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo30324301 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo65766709;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo65766709 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75899647;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo75899647 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo41437519;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo41437519 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo98262773;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo98262773 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo20756435;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo20756435 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo61905472;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo61905472 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo68403463;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo68403463 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo13204165;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo13204165 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo63111445;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo63111445 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5273590;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo5273590 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43785347;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo43785347 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo21249987;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo21249987 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo83007411;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo83007411 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo60228723;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo60228723 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo36833443;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo36833443 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8225681;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8225681 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8696312;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo8696312 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo54305293;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo54305293 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo44323458;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo44323458 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo96394729;     MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo96394729 = MjiFqFOCVtknbmcDNiBVqnemtEUegHDCVsQo36189993;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BTPDzyRzsCBIRFyEpIlSiiLezeEeasn94819331() {     double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT96955025 = -852187366;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT70183966 = -231693786;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT53559111 = -554041697;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT74918748 = 99605777;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT69805279 = -370376828;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT91453730 = -476765865;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT3850812 = -320938032;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT95392251 = -280352182;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT43646836 = -316846325;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26459094 = -786275647;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT49010323 = -279643847;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT89980960 = -457554982;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT46000868 = -631107720;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT94628051 = -356271568;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT85644117 = -330019953;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT71542873 = -878517836;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9692694 = -999573742;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT83401276 = -598833716;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT94263337 = -471152436;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT75455743 = -672167303;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT42405172 = -581660940;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT1504289 = -895115197;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT66008976 = -569856670;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT45496113 = -765185745;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT25879491 = -602399111;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT22544673 = -84814254;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT40606797 = 12633664;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT31291777 = -608429159;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT19750175 = -532735822;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT59746023 = -73453806;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72129018 = 91268765;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT84674755 = -534434303;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT68057474 = 78491828;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT79655544 = -253164616;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT34366019 = -152109226;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26836786 = -818419588;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT48203583 = 91875801;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT33337173 = -201882655;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT97532266 = -98726611;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT1315855 = -29172750;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT81891653 = -309263841;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9420499 = -330295633;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT71540473 = -446880130;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT11147170 = -914069126;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT70968777 = -192288054;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT35360999 = -193165261;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT36191343 = -927345836;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT803697 = -716451369;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58771493 = -16991786;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT80577760 = -566324035;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT23922261 = -897767460;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT47601916 = 70396354;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58370011 = -564811304;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT55668970 = -702844655;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT52414160 = -245373753;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT95450737 = -957072170;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT4174990 = -661837117;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT8062998 = -788855953;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT49039258 = -297995112;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT47260607 = -185562575;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50846934 = -389399529;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72559035 = -712508874;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT75642077 = -747616361;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT83900813 = -143392519;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT54330075 = -777544412;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT64335568 = -745209545;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT21923487 = -436046811;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT66345323 = -277943105;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT60262032 = -104162342;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58807331 = -511600365;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT23339290 = -870393637;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT76355521 = -697691087;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT85869009 = -400107105;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT92947482 = -341979686;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT93564090 = -262903463;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT32984673 = -151365307;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT29963815 = -348235068;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT54861806 = -655787545;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT74527335 = -472897691;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT90518491 = -309233851;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT86353329 = -157468419;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT39803101 = -270914968;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72520284 = -491437373;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT39172415 = -966411787;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT35823762 = -175686346;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT24527103 = -979127589;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26304744 = -969622999;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT12388505 = -218663517;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT27241385 = 92209137;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT38915282 = -195037057;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT22661796 = -56582472;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT40140586 = -119268247;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT84297915 = -903887543;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50271660 = -913164037;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50468921 = -639773222;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9332618 = -596754967;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT33778422 = -582679273;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT87639660 = -203487611;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT56817095 = -36524714;    double GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT6633210 = -852187366;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT96955025 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT70183966;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT70183966 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT53559111;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT53559111 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT74918748;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT74918748 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT69805279;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT69805279 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT91453730;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT91453730 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT3850812;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT3850812 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT95392251;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT95392251 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT43646836;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT43646836 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26459094;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26459094 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT49010323;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT49010323 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT89980960;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT89980960 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT46000868;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT46000868 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT94628051;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT94628051 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT85644117;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT85644117 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT71542873;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT71542873 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9692694;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9692694 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT83401276;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT83401276 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT94263337;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT94263337 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT75455743;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT75455743 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT42405172;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT42405172 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT1504289;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT1504289 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT66008976;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT66008976 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT45496113;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT45496113 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT25879491;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT25879491 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT22544673;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT22544673 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT40606797;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT40606797 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT31291777;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT31291777 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT19750175;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT19750175 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT59746023;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT59746023 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72129018;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72129018 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT84674755;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT84674755 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT68057474;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT68057474 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT79655544;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT79655544 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT34366019;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT34366019 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26836786;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26836786 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT48203583;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT48203583 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT33337173;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT33337173 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT97532266;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT97532266 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT1315855;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT1315855 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT81891653;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT81891653 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9420499;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9420499 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT71540473;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT71540473 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT11147170;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT11147170 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT70968777;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT70968777 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT35360999;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT35360999 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT36191343;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT36191343 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT803697;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT803697 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58771493;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58771493 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT80577760;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT80577760 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT23922261;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT23922261 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT47601916;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT47601916 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58370011;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58370011 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT55668970;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT55668970 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT52414160;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT52414160 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT95450737;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT95450737 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT4174990;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT4174990 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT8062998;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT8062998 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT49039258;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT49039258 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT47260607;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT47260607 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50846934;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50846934 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72559035;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72559035 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT75642077;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT75642077 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT83900813;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT83900813 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT54330075;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT54330075 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT64335568;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT64335568 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT21923487;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT21923487 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT66345323;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT66345323 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT60262032;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT60262032 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58807331;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT58807331 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT23339290;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT23339290 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT76355521;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT76355521 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT85869009;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT85869009 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT92947482;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT92947482 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT93564090;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT93564090 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT32984673;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT32984673 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT29963815;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT29963815 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT54861806;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT54861806 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT74527335;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT74527335 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT90518491;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT90518491 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT86353329;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT86353329 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT39803101;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT39803101 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72520284;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT72520284 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT39172415;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT39172415 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT35823762;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT35823762 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT24527103;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT24527103 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26304744;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT26304744 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT12388505;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT12388505 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT27241385;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT27241385 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT38915282;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT38915282 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT22661796;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT22661796 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT40140586;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT40140586 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT84297915;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT84297915 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50271660;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50271660 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50468921;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT50468921 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9332618;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT9332618 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT33778422;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT33778422 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT87639660;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT87639660 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT56817095;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT56817095 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT6633210;     GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT6633210 = GpuDVeLlkUvsGxdLwpHaURJDTIlDUumYAPvT96955025;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LOHtWGsFIadUGdjDjWEIlSkZfjtIHRx62222837() {     double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT22254619 = -680388220;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24427420 = -725497018;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT95573888 = -238849384;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71364837 = 82824144;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT74547374 = -294573728;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT30433323 = -6930664;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11877748 = -326142881;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50623787 = -831914807;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT60026465 = -170693093;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT28697053 = 97020862;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90191384 = -899374420;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT12583922 = -695221217;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21498484 = -365843557;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT25285736 = -114556406;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT43760320 = -349604421;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT68403596 = -411456637;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT59230044 = -101973270;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT94291890 = 51364003;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT97619989 = -982134194;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT23950771 = 28075795;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11384739 = -768889600;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT92448907 = -802836494;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT41301013 = 16598923;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24424127 = 62452010;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT14199185 = 22088908;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT10957277 = -95013416;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21079089 = -138188103;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT27726528 = -630571769;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4468787 = -753409244;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT69443397 = -598038218;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT64319222 = -593130681;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT44901684 = -566049759;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4111135 = -626931614;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT39959249 = -473496842;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT29252450 = -476233193;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT16984777 = -125407498;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT78640925 = -46797406;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT69024580 = -955187459;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT62148801 = -49085278;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT92278440 = 85478602;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT85780005 = -893776776;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT60275809 = -901008892;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71790976 = -833269756;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT85881768 = -16688206;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT10924952 = -6711449;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT37528404 = -932336720;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT6494052 = -451753641;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT65228796 = -654995394;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT25214102 = -566249830;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT53892333 = -676377089;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT80484494 = -780110091;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT93402827 = -796035103;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT20622021 = -124578809;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT13562303 = -691532488;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50838099 = -509122632;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT29805711 = -877551727;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT83126407 = -642095941;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71149761 = -201301395;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT57165653 = -939264765;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT63590097 = -99560313;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT9354234 = -868742562;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT84151219 = -695571113;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT46155000 = 21494437;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90583067 = -572654875;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT64377831 = -309848458;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT45289700 = -233324662;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT8472788 = 31710396;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT81539234 = -892346716;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT96033285 = -638323214;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT26775543 = -124196924;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT89762671 = -264659232;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90205463 = -146785811;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT32143089 = -899550720;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT5341549 = -967612796;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT38170766 = -78147430;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT51108929 = -867880708;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT20657931 = -969566738;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT55419245 = -966712872;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT13499176 = -930836541;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT76670781 = -45574372;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4463226 = -643259776;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT55850292 = -483192710;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT2512426 = 35678061;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50576454 = 22967845;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT88958903 = -817928128;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT70916395 = -797095578;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24279664 = -341470950;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90548832 = -935399127;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT89121150 = -964374211;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT99446739 = -598681466;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT33858370 = -483311557;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT7491164 = -845496012;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11858928 = 84077305;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT98558704 = -949524965;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT82924206 = -45778837;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT1628786 = -98205663;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT14120810 = -822503330;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT81207909 = -160614881;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21503937 = -706839749;    double XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT65635252 = -680388220;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT22254619 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24427420;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24427420 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT95573888;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT95573888 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71364837;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71364837 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT74547374;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT74547374 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT30433323;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT30433323 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11877748;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11877748 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50623787;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50623787 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT60026465;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT60026465 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT28697053;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT28697053 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90191384;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90191384 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT12583922;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT12583922 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21498484;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21498484 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT25285736;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT25285736 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT43760320;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT43760320 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT68403596;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT68403596 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT59230044;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT59230044 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT94291890;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT94291890 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT97619989;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT97619989 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT23950771;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT23950771 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11384739;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11384739 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT92448907;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT92448907 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT41301013;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT41301013 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24424127;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24424127 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT14199185;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT14199185 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT10957277;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT10957277 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21079089;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21079089 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT27726528;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT27726528 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4468787;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4468787 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT69443397;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT69443397 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT64319222;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT64319222 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT44901684;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT44901684 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4111135;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4111135 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT39959249;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT39959249 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT29252450;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT29252450 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT16984777;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT16984777 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT78640925;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT78640925 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT69024580;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT69024580 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT62148801;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT62148801 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT92278440;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT92278440 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT85780005;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT85780005 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT60275809;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT60275809 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71790976;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71790976 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT85881768;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT85881768 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT10924952;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT10924952 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT37528404;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT37528404 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT6494052;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT6494052 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT65228796;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT65228796 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT25214102;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT25214102 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT53892333;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT53892333 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT80484494;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT80484494 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT93402827;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT93402827 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT20622021;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT20622021 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT13562303;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT13562303 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50838099;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50838099 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT29805711;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT29805711 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT83126407;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT83126407 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71149761;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT71149761 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT57165653;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT57165653 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT63590097;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT63590097 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT9354234;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT9354234 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT84151219;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT84151219 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT46155000;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT46155000 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90583067;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90583067 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT64377831;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT64377831 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT45289700;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT45289700 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT8472788;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT8472788 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT81539234;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT81539234 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT96033285;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT96033285 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT26775543;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT26775543 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT89762671;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT89762671 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90205463;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90205463 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT32143089;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT32143089 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT5341549;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT5341549 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT38170766;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT38170766 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT51108929;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT51108929 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT20657931;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT20657931 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT55419245;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT55419245 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT13499176;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT13499176 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT76670781;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT76670781 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4463226;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT4463226 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT55850292;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT55850292 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT2512426;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT2512426 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50576454;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT50576454 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT88958903;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT88958903 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT70916395;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT70916395 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24279664;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT24279664 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90548832;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT90548832 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT89121150;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT89121150 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT99446739;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT99446739 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT33858370;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT33858370 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT7491164;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT7491164 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11858928;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT11858928 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT98558704;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT98558704 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT82924206;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT82924206 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT1628786;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT1628786 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT14120810;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT14120810 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT81207909;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT81207909 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21503937;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT21503937 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT65635252;     XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT65635252 = XAVhIIGdocLqNBnFtyKYoaVqtehOnMkyrcGT22254619;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void SMWGtNuxbgsWJqnnDGWwwlgGSpIQQdR29626342() {     double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh47554212 = -508589078;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh78670873 = -119300263;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh37588666 = 76342929;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh67810926 = 66042517;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79289468 = -218770645;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh69412914 = -637095464;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh19904683 = -331347731;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5855322 = -283477462;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh76406094 = -24539863;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh30935013 = -119682629;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh31372446 = -419104994;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh35186884 = -932887471;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh96996099 = -100579395;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh55943420 = -972841247;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh1876522 = -369188926;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh65264320 = 55604561;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh8767394 = -304372798;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5182505 = -398438286;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh976642 = -393115949;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh72445798 = -371681108;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh80364304 = -956118270;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh83393527 = -710557790;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh16593050 = -496945485;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh3352142 = -209910235;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh2518879 = -453423097;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh99369882 = -105212570;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh1551380 = -289009870;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24161278 = -652714392;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh89187398 = -974082636;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79140772 = -22622631;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh56509426 = -177530126;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5128613 = -597665208;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh40164796 = -232355050;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh262954 = -693829068;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24138882 = -800357153;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh7132768 = -532395370;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh9078267 = -185470612;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh4711988 = -608492270;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh26765336 = 556080;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh83241026 = -899870046;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh89668357 = -378289710;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh11131120 = -371722110;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh72041479 = -119659378;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh60616366 = -219307287;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh50881126 = -921134842;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39695809 = -571508142;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh76796758 = 23838554;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh29653897 = -593539416;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh91656710 = -15507825;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh27206904 = -786430154;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh37046728 = -662452721;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39203739 = -562466552;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh82874028 = -784346319;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh71455635 = -680220320;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh49262037 = -772871500;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh64160684 = -798031288;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh62077823 = -622354778;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh34236525 = -713746836;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh65292047 = -480534387;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79919585 = -13558075;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh67861534 = -248085594;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh95743404 = -678633340;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh16667923 = -309394826;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh97265322 = 98082768;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74425586 = -942152503;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh26243833 = -821439787;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh95022088 = -600532422;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh96733145 = -406750327;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh31804539 = -72484094;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh94743753 = -836793557;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh56186054 = -758924828;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh4055407 = -695880529;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh78417169 = -298994367;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh17735616 = -493245903;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh82777441 = -993391398;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh69233185 = -484396160;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh11352048 = -490898413;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh55976684 = -177638199;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh52471015 = -288775393;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh62823070 = -881914956;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh22573125 = -29051124;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh71897483 = -695470455;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh32504568 = -537206567;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh61980494 = -87652483;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh42094045 = -360169910;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh17305688 = -615063575;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh22254585 = -813318889;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh68709160 = -552134730;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh51000917 = -920957569;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh59978197 = 97674134;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh45054944 = -910040592;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74841742 = -471723776;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39419940 = -27957883;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh46845750 = -985885845;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh15379492 = -551784453;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh93924952 = -699656371;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh94463196 = 37672715;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74776157 = -117742146;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh86190779 = -277154785;    double CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24637294 = -508589078;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh47554212 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh78670873;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh78670873 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh37588666;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh37588666 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh67810926;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh67810926 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79289468;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79289468 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh69412914;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh69412914 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh19904683;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh19904683 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5855322;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5855322 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh76406094;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh76406094 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh30935013;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh30935013 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh31372446;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh31372446 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh35186884;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh35186884 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh96996099;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh96996099 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh55943420;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh55943420 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh1876522;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh1876522 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh65264320;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh65264320 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh8767394;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh8767394 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5182505;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5182505 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh976642;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh976642 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh72445798;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh72445798 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh80364304;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh80364304 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh83393527;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh83393527 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh16593050;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh16593050 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh3352142;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh3352142 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh2518879;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh2518879 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh99369882;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh99369882 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh1551380;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh1551380 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24161278;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24161278 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh89187398;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh89187398 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79140772;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79140772 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh56509426;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh56509426 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5128613;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh5128613 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh40164796;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh40164796 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh262954;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh262954 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24138882;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24138882 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh7132768;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh7132768 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh9078267;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh9078267 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh4711988;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh4711988 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh26765336;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh26765336 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh83241026;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh83241026 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh89668357;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh89668357 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh11131120;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh11131120 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh72041479;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh72041479 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh60616366;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh60616366 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh50881126;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh50881126 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39695809;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39695809 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh76796758;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh76796758 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh29653897;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh29653897 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh91656710;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh91656710 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh27206904;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh27206904 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh37046728;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh37046728 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39203739;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39203739 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh82874028;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh82874028 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh71455635;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh71455635 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh49262037;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh49262037 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh64160684;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh64160684 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh62077823;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh62077823 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh34236525;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh34236525 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh65292047;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh65292047 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79919585;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh79919585 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh67861534;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh67861534 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh95743404;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh95743404 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh16667923;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh16667923 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh97265322;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh97265322 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74425586;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74425586 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh26243833;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh26243833 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh95022088;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh95022088 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh96733145;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh96733145 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh31804539;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh31804539 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh94743753;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh94743753 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh56186054;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh56186054 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh4055407;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh4055407 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh78417169;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh78417169 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh17735616;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh17735616 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh82777441;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh82777441 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh69233185;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh69233185 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh11352048;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh11352048 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh55976684;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh55976684 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh52471015;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh52471015 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh62823070;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh62823070 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh22573125;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh22573125 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh71897483;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh71897483 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh32504568;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh32504568 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh61980494;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh61980494 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh42094045;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh42094045 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh17305688;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh17305688 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh22254585;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh22254585 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh68709160;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh68709160 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh51000917;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh51000917 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh59978197;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh59978197 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh45054944;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh45054944 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74841742;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74841742 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39419940;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh39419940 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh46845750;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh46845750 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh15379492;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh15379492 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh93924952;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh93924952 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh94463196;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh94463196 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74776157;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh74776157 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh86190779;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh86190779 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24637294;     CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh24637294 = CoedlQYXgrDrYmZpCzGhacSqgfQnEFdgxOqh47554212;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pyxryuZFUJGiHXaELyHkFiJgXNKBcyn6200299() {     int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj87097828 = -875598036;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj19990112 = 54581175;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78650689 = -25118907;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj16647576 = -649993233;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78616561 = -184265141;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55049456 = -954103584;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj14938140 = -772699305;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj10301294 = -293997026;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj77113742 = -171558572;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95468986 = -193253114;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96780338 = -925206652;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj69517437 = -87477416;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38918507 = -666692529;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91785594 = -592541063;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj32696711 = 48865582;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj23211381 = -345662347;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj49950370 = -728203005;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj71338569 = -578212765;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6424368 = -847011150;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj90566254 = -191237260;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43648802 = -154917284;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj72376600 = -87116956;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj57425140 = -720486235;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj92088871 = -726572056;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91011883 = -84679985;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40012419 = -161943912;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj67575248 = -644955626;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj74359722 = -813257447;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj76266477 = -910663849;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj97836892 = -560379628;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj58732156 = -243114660;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj23836270 = -749816948;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78408794 = -576187191;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38029831 = 27152232;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40824868 = -149868287;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6025387 = -478801185;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj16793141 = -567002451;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj21654651 = -75441167;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96483058 = -763139109;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj42888554 = -326839928;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95589591 = -531958298;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj54437930 = -648700936;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj64454768 = -639129403;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj77816766 = -976485215;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj35827732 = -787653304;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj2417913 = -334458026;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96759692 = 99237076;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj39586280 = -351250397;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj58490122 = -312197514;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj80575954 = 55220145;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj32468121 = -666674084;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95519210 = -97352991;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj27717903 = -216031746;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj70289519 = -256883443;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62551497 = -53362419;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj14721228 = -688481081;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62564972 = -224932591;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj86561818 = -298546852;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj25635693 = -465313248;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38604143 = 77678770;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj87474208 = -209147958;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40578418 = -959441859;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj34034817 = -383333178;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj79276849 = -611178945;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj36736831 = -950138454;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj72944068 = -75389704;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91108642 = -511290225;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj888677 = -593844762;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj50960726 = -342672777;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj26671325 = -472333233;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6418240 = -778659896;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj28295720 = -552761839;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj74855511 = -815073657;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj63535814 = -420171222;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj94976662 = -659278962;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj89210871 = -506216349;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj7921833 = -447987553;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj79608373 = -744001020;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj56261139 = -938918752;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj88593970 = -750221959;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43252726 = -161180988;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj27988968 = -193705230;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj15869600 = -401059934;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95690522 = -865883995;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj65368772 = -893705544;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj63212945 = -45761669;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96118367 = -433785202;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj8119276 = -219303748;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj75478334 = -919485350;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj26103641 = -461387207;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43460414 = -153868595;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj30231323 = -168455600;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96018958 = -610127919;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj57878915 = -740817880;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55414345 = -17691970;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55011174 = -572516440;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj20403113 = -165367759;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj85177918 = 72049541;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj41079936 = 73653238;    int GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62883663 = -875598036;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj87097828 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj19990112;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj19990112 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78650689;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78650689 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj16647576;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj16647576 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78616561;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78616561 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55049456;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55049456 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj14938140;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj14938140 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj10301294;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj10301294 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj77113742;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj77113742 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95468986;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95468986 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96780338;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96780338 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj69517437;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj69517437 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38918507;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38918507 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91785594;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91785594 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj32696711;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj32696711 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj23211381;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj23211381 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj49950370;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj49950370 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj71338569;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj71338569 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6424368;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6424368 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj90566254;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj90566254 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43648802;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43648802 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj72376600;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj72376600 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj57425140;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj57425140 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj92088871;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj92088871 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91011883;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91011883 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40012419;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40012419 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj67575248;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj67575248 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj74359722;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj74359722 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj76266477;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj76266477 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj97836892;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj97836892 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj58732156;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj58732156 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj23836270;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj23836270 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78408794;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj78408794 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38029831;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38029831 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40824868;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40824868 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6025387;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6025387 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj16793141;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj16793141 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj21654651;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj21654651 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96483058;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96483058 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj42888554;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj42888554 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95589591;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95589591 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj54437930;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj54437930 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj64454768;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj64454768 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj77816766;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj77816766 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj35827732;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj35827732 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj2417913;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj2417913 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96759692;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96759692 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj39586280;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj39586280 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj58490122;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj58490122 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj80575954;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj80575954 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj32468121;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj32468121 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95519210;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95519210 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj27717903;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj27717903 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj70289519;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj70289519 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62551497;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62551497 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj14721228;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj14721228 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62564972;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62564972 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj86561818;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj86561818 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj25635693;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj25635693 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38604143;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj38604143 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj87474208;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj87474208 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40578418;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj40578418 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj34034817;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj34034817 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj79276849;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj79276849 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj36736831;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj36736831 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj72944068;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj72944068 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91108642;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj91108642 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj888677;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj888677 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj50960726;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj50960726 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj26671325;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj26671325 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6418240;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj6418240 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj28295720;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj28295720 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj74855511;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj74855511 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj63535814;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj63535814 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj94976662;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj94976662 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj89210871;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj89210871 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj7921833;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj7921833 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj79608373;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj79608373 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj56261139;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj56261139 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj88593970;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj88593970 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43252726;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43252726 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj27988968;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj27988968 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj15869600;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj15869600 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95690522;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj95690522 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj65368772;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj65368772 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj63212945;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj63212945 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96118367;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96118367 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj8119276;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj8119276 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj75478334;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj75478334 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj26103641;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj26103641 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43460414;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj43460414 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj30231323;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj30231323 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96018958;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj96018958 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj57878915;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj57878915 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55414345;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55414345 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55011174;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj55011174 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj20403113;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj20403113 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj85177918;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj85177918 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj41079936;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj41079936 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62883663;     GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj62883663 = GyQRAgdGhGVmhYPCMTCTHQFgHpRTljYdVWzj87097828;}
// Junk Finished
