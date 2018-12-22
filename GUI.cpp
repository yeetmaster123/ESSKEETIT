
#include "GUI.h"

#include "DrawManager.h"
#include "MetaInfo.h"
#include "Menu.h"
#include <algorithm>
#include "tinyxml2.h"
#include "Controls.h"

#include <sstream>

CGUI GUI;

CGUI::CGUI()
{

}

// Draws all windows 
void CGUI::Draw()
{
	bool ShouldDrawCursor = false;

	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			ShouldDrawCursor = true;
			DrawWindow(window, 1);
		}
	}

	if (ShouldDrawCursor)
	{
		draw.rect(Mouse.x + 1, Mouse.y, 1, 17, Color(3, 6, 26, 255));
		for (int i = 0; i < 11; i++)
			draw.rect(Mouse.x + 2 + i, Mouse.y + 1 + i, 1, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 8, Mouse.y + 12, 5, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 8, Mouse.y + 13, 1, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 9, Mouse.y + 14, 1, 2, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 10, Mouse.y + 16, 1, 2, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 8, Mouse.y + 18, 2, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 7, Mouse.y + 16, 1, 2, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 6, Mouse.y + 14, 1, 2, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 5, Mouse.y + 13, 1, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 4, Mouse.y + 14, 1, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 3, Mouse.y + 15, 1, 1, Color(3, 6, 26, 255));
		draw.rect(Mouse.x + 2, Mouse.y + 16, 1, 1, Color(3, 6, 26, 255));
		for (int i = 0; i < 4; i++)
			draw.rect(Mouse.x + 2 + i, Mouse.y + 2 + i, 1, 14 - (i * 2), Color(167, 244, 66, 255));
		draw.rect(Mouse.x + 6, Mouse.y + 6, 1, 8, Color(167, 244, 66, 255));
		draw.rect(Mouse.x + 7, Mouse.y + 7, 1, 9, Color(167, 244, 66, 255));
		for (int i = 0; i < 4; i++)
			draw.rect(Mouse.x + 8 + i, Mouse.y + 8 + i, 1, 4 - i, Color(167, 244, 66, 255));
		draw.rect(Mouse.x + 8, Mouse.y + 14, 1, 4, Color(167, 244, 66, 255));
		draw.rect(Mouse.x + 9, Mouse.y + 16, 1, 2, Color(167, 244, 66, 255));
	}
}

// Handle all input etc
void CGUI::Update()
{
	//Key Array
	std::copy(keys, keys + 255, oldKeys);
	for (int x = 0; x < 255; x++)
	{
		//oldKeys[x] = oldKeys[x] & keys[x];
		keys[x] = (GetAsyncKeyState(x));
	}

	// Mouse Location
	POINT mp; GetCursorPos(&mp);
	Mouse.x = mp.x; Mouse.y = mp.y;

	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	m_pEngine->GetScreenSize(w, h);
	Viewport.right = w; Viewport.bottom = h;
	RECT Screen = Viewport;

	// Window Binds
	for (auto& bind : WindowBinds)
	{
		if (false)
		{
			bind.second->Toggle();
		}
	}

	// Stop dragging
	if (IsDraggingWindow && !GetKeyState(VK_LBUTTON))
	{
		IsDraggingWindow = false;
		DraggingWindow = nullptr;
	}

	// If we are in the proccess of dragging a window
	if (IsDraggingWindow && GetKeyState(VK_LBUTTON) && !GetKeyPress(VK_LBUTTON))
	{
		if (DraggingWindow)
		{
			DraggingWindow->m_x = Mouse.x - DragOffsetX;
			DraggingWindow->m_y = Mouse.y - DragOffsetY;
		}
	}

	// Process some windows
	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			// Used to tell the widget processing that there could be a click
			bool bCheckWidgetClicks = false;

			// If the user clicks inside the window
			if (GetKeyPress(VK_LBUTTON))
			{
				if (IsMouseInRegion(window->m_x, window->m_y, window->m_x + window->m_iWidth, window->m_y + window->m_iHeight))
				{
					// Is it inside the client area?
					if (IsMouseInRegion(window->GetClientArea()))
					{
						// User is selecting a new tab
						if (!IsMouseInRegion(window->GetTabArea()))
							bCheckWidgetClicks = true;
					}
				}
				else
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}

			if (IsMouseInRegion(window->GetDragArea()))
			{
				// Must be in the around the title or side of the window
				// So we assume the user is trying to drag the window
				IsDraggingWindow = true;
				DraggingWindow = window;
				DragOffsetX = Mouse.x - window->m_x;
				DragOffsetY = Mouse.y - window->m_y;

				// Loose focus on the control
				window->IsFocusingControl = false;
				window->FocusedControl = nullptr;
			}

			// Controls 
			if (window->SelectedTab != nullptr)
			{
				// Focused widget
				bool SkipWidget = false;
				CControl* SkipMe = nullptr;

				// this window is focusing on a widget??
				if (window->IsFocusingControl)
				{
					if (window->FocusedControl != nullptr)
					{
						CControl* control = window->FocusedControl;
						CGroupBox* group;
						if (control->FileControlType != UIControlTypes::UIC_GroupBox) group = control->parent_group ? (CGroupBox*)control->parent_group : nullptr;

						if (group != nullptr && control->FileControlType != UIControlTypes::UIC_GroupBox)
						{
							if ((group->group_tabs.size() > 0 && control->g_tab == group->selected_tab) || group->group_tabs.size() == 0)
							{
								// We've processed it once, skip it later
								SkipWidget = true;
								SkipMe = window->FocusedControl;

								POINT cAbs = window->FocusedControl->GetAbsolutePos();
								RECT controlRect = { cAbs.x, cAbs.y, window->FocusedControl->m_iWidth, window->FocusedControl->m_iHeight };
								window->FocusedControl->OnUpdate();

								if (window->FocusedControl->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
								{
									window->FocusedControl->OnClick();

									bCheckWidgetClicks = false;
								}
							}
						}
						else if (control->FileControlType == UIControlTypes::UIC_GroupBox || control->FileControlType != UIControlTypes::UIC_GroupBox && !control->parent_group)
						{
							// We've processed it once, skip it later
							SkipWidget = true;
							SkipMe = window->FocusedControl;

							POINT cAbs = window->FocusedControl->GetAbsolutePos();
							RECT controlRect = { cAbs.x, cAbs.y, window->FocusedControl->m_iWidth, window->FocusedControl->m_iHeight };
							window->FocusedControl->OnUpdate();

							if (window->FocusedControl->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
							{
								window->FocusedControl->OnClick();

								// If it gets clicked we loose focus
								window->IsFocusingControl = false;
								window->FocusedControl = nullptr;
								bCheckWidgetClicks = false;
							}
						}
					}
				}

				// Itterate over the rest of the control
				for (auto control : window->SelectedTab->Controls)
				{
					if (control != nullptr)
					{
						CGroupBox* group;
						if (control->FileControlType != UIControlTypes::UIC_GroupBox) group = control->parent_group ? (CGroupBox*)control->parent_group : nullptr;

						if (group != nullptr && control->FileControlType != UIControlTypes::UIC_GroupBox)
						{
							if (group->group_tabs.size() > 0 && control->g_tab == group->selected_tab || group->group_tabs.size() == 0)
							{
								if (SkipWidget && SkipMe == control)
									continue;

								POINT cAbs = control->GetAbsolutePos();
								RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
								control->OnUpdate();

								if (control->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
								{
									control->OnClick();
									bCheckWidgetClicks = false;

									// Change of focus
									if (control->Flag(UIFlags::UI_Focusable))
									{
										window->IsFocusingControl = true;
										window->FocusedControl = control;
									}
									else
									{
										window->IsFocusingControl = false;
										window->FocusedControl = nullptr;
									}
								}
							}
						}
						else if (control->FileControlType == UIControlTypes::UIC_GroupBox || control->FileControlType != UIControlTypes::UIC_GroupBox && !control->parent_group)
						{
							if (SkipWidget && SkipMe == control)
								continue;

							POINT cAbs = control->GetAbsolutePos();
							RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
							control->OnUpdate();

							if (control->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
							{
								control->OnClick();
								bCheckWidgetClicks = false;

								// Change of focus
								if (control->Flag(UIFlags::UI_Focusable))
								{
									window->IsFocusingControl = true;
									window->FocusedControl = control;
								}
								else
								{
									window->IsFocusingControl = false;
									window->FocusedControl = nullptr;
								}
							}
						}
					}
				}
				// We must have clicked whitespace
				if (bCheckWidgetClicks)
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}
		}
	}
}

// Returns 
bool CGUI::GetKeyPress(unsigned int key)
{
	if (keys[key] == true && oldKeys[key] == false)
		return true;
	else
		return false;
}

bool CGUI::GetKeyState(unsigned int key)
{
	return keys[key];
}

bool CGUI::IsMouseInRegion(int x, int y, int x2, int y2)
{
	if (Mouse.x > x && Mouse.y > y && Mouse.x < x2 && Mouse.y < y2)
		return true;
	else
		return false;
}

bool CGUI::IsMouseInRegion(RECT region)
{
	return IsMouseInRegion(region.left, region.top, region.left + region.right, region.top + region.bottom);
}

POINT CGUI::GetMouse()
{
	return Mouse;
}

bool CGUI::DrawWindow(CWindow* window, int menu)
{
	if (menu == 1)
	{
		int _x = window->m_x + 8;
		int _tab_x = window->m_x + 8 - 90;
		int _y = window->m_y + 29 * 2;
		int _width = window->m_iWidth - 16;
		int _height = window->m_iHeight - 236;

		//Inner
		draw.rect(_tab_x, _y, _width + 90, _height, Color(28, 28, 28, 255));
		draw.rect(_tab_x, _y - 6, _width + 90, 6, Color(40, 40, 40, 255));
		draw.rect(_tab_x, _y + _height, _width + 90, 6, Color(40, 40, 40, 255));
		draw.rect(_tab_x - 6, _y - 6, 6, _height + 12, Color(40, 40, 40, 255));
		draw.rect(_x + _width, _y - 6, 6, _height + 12, Color(40, 40, 40, 255));

		//Tab
		draw.rect(_tab_x, _y, 90, _height, Color(21, 21, 19, 255));

		draw.outline(_tab_x, _y, _width + 90, _height, Color(48, 48, 48, 255));
		draw.outline(_x - 6 - 90, _y - 6, _width + 90 + 12, _height + 12, Color(48, 48, 48, 255));
		draw.outline(_tab_x, _y, 90, _height, Color(48, 48, 48, 255));

		static float rainbow;
		rainbow += 0.005f;
		if (rainbow > 1.f)
		{
			rainbow = 0.f;
		}

		int W, H;
		m_pEngine->GetScreenSize(W, H);

		draw.DrawRectRainbow(_tab_x + 1, _y + 1, _width + 90 - 2, 1, 0.001f, rainbow);

		// we are using the dimensions of the picture we got from Photoshop

		int tabcount = window->Tabs.size();
		if (tabcount) // If there are some tabs
		{
			bool isOut = true;

			for (int i = 0; i < tabcount; i++)
			{
				CTab *tab = window->Tabs[i];

				float xAxis;
				float yAxis;
				float yWinPos = _y;
				float yWinHeight = _height;

				float intercept = (yWinHeight - 40) / (tabcount + 1);
				int factor = i;

				yAxis = yWinPos + 16 + (factor * intercept) - 10 + 20;

				RECT TabDrawArea = { _tab_x + 1, yAxis - 5, 89, intercept };

				RECT TextSize;
				TextSize = draw.get_text_size(tab->Title.c_str(), draw.fonts.font_icons);

				RECT ClickTabArea = { xAxis,
					yAxis,
					TextSize.right,
					TextSize.bottom };

				if (GetAsyncKeyState(VK_LBUTTON))
				{
					if (IsMouseInRegion(TabDrawArea))
					{
						window->SelectedTab = window->Tabs[i];
						window->IsFocusingControl = false;
						window->FocusedControl = nullptr;
					}
				}

				xAxis = _x - (45 + TextSize.right / 2);
				if (IsMouseInRegion(TabDrawArea) && window->SelectedTab != tab) {
					draw.text(TabDrawArea.left + (TabDrawArea.right / 2) - (TextSize.right / 2), TabDrawArea.top + (TabDrawArea.bottom / 2) - (TextSize.bottom / 2), tab->Title.c_str(), draw.fonts.font_icons, Color(190, 190, 190, 255));
				}
				else if (window->SelectedTab == tab) {
					draw.rect(TabDrawArea.left, TabDrawArea.top, TabDrawArea.right, TabDrawArea.bottom, Color(28, 28, 28, 255));
					draw.line(TabDrawArea.left, TabDrawArea.top, TabDrawArea.left + TabDrawArea.right, TabDrawArea.top, Color(48, 48, 48, 255));
					draw.line(TabDrawArea.left, TabDrawArea.top + TabDrawArea.bottom, TabDrawArea.left + TabDrawArea.right, TabDrawArea.top + TabDrawArea.bottom, Color(48, 48, 48, 255));
					draw.text(TabDrawArea.left + (TabDrawArea.right / 2) - (TextSize.right / 2), TabDrawArea.top + (TabDrawArea.bottom / 2) - (TextSize.bottom / 2), tab->Title.c_str(), draw.fonts.font_icons, Color(200, 200, 200, 255));
				}
				else {
					draw.text(TabDrawArea.left + (TabDrawArea.right / 2) - (TextSize.right / 2), TabDrawArea.top + (TabDrawArea.bottom / 2) - (TextSize.bottom / 2), tab->Title.c_str(), draw.fonts.font_icons, Color(92, 92, 92, 255));
				}

				int width = _width;
			}
		}
	}

	// Controls 
	if (window->SelectedTab != nullptr)
	{
		// Focused widget
		bool SkipWidget = false;
		CControl* SkipMe = nullptr;

		// this window is focusing on a widget??
		if (window->IsFocusingControl)
		{
			if (window->FocusedControl != nullptr)
			{
				CControl* control = window->FocusedControl;
				CGroupBox* group;
				if (control->FileControlType != UIControlTypes::UIC_GroupBox) group = control->parent_group ? (CGroupBox*)control->parent_group : nullptr;

				if (group != nullptr && control->FileControlType != UIControlTypes::UIC_GroupBox)
				{
					if (group->group_tabs.size() > 0 && control->g_tab == group->selected_tab || group->group_tabs.size() == 0)
					{
						SkipWidget = true;
						SkipMe = window->FocusedControl;
					}
				}
				else if (control->FileControlType == UIControlTypes::UIC_GroupBox || control->FileControlType != UIControlTypes::UIC_GroupBox && !control->parent_group)
				{

					SkipWidget = true;
					SkipMe = window->FocusedControl;
				}
			}
		}


		// Itterate over all the other controls
		for (auto control : window->SelectedTab->Controls)
		{
			if (SkipWidget && SkipMe == control)
				continue;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				CGroupBox* group;
				if (control->FileControlType != UIControlTypes::UIC_GroupBox) group = control->parent_group ? (CGroupBox*)control->parent_group : nullptr;

				if (group != nullptr && control->FileControlType != UIControlTypes::UIC_GroupBox)
				{
					if (group->group_tabs.size() > 0 && control->g_tab == group->selected_tab || group->group_tabs.size() == 0)
					{
						POINT cAbs = control->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
						bool hover = false;
						if (IsMouseInRegion(controlRect))
						{
							hover = true;
						}
						control->Draw(hover);
					}
				}
				else if (control->FileControlType == UIControlTypes::UIC_GroupBox || control->FileControlType != UIControlTypes::UIC_GroupBox && !control->parent_group)
				{

					POINT cAbs = control->GetAbsolutePos();
					RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
					bool hover = false;
					if (IsMouseInRegion(controlRect))
					{
						hover = true;
					}
					control->Draw(hover);
				}
			}
		}

		// Draw the skipped widget last
		if (SkipWidget)
		{
			auto control = window->FocusedControl;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				CControl* control = window->FocusedControl;
				CGroupBox* group;
				if (control->FileControlType != UIControlTypes::UIC_GroupBox) group = control->parent_group ? (CGroupBox*)control->parent_group : nullptr;

				if (group != nullptr && control->FileControlType != UIControlTypes::UIC_GroupBox)
				{
					if (group->group_tabs.size() > 0 && control->g_tab == group->selected_tab || group->group_tabs.size() == 0)
					{
						POINT cAbs = control->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
						bool hover = false;
						if (IsMouseInRegion(controlRect))
						{
							hover = true;
						}
						control->Draw(hover);
					}
				}
				else if (control->FileControlType == UIControlTypes::UIC_GroupBox || control->FileControlType != UIControlTypes::UIC_GroupBox && !control->parent_group)
				{

					POINT cAbs = control->GetAbsolutePos();
					RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
					bool hover = false;
					if (IsMouseInRegion(controlRect))
					{
						hover = true;
					}
					control->Draw(hover);
				}
			}
		}

	}


	return true;
}

void CGUI::RegisterWindow(CWindow* window)
{
	Windows.push_back(window);

	// Resorting to put groupboxes at the start
	for (auto tab : window->Tabs)
	{
		for (auto control : tab->Controls)
		{
			if (control->Flag(UIFlags::UI_RenderFirst))
			{
				CControl * c = control;
				tab->Controls.erase(std::remove(tab->Controls.begin(), tab->Controls.end(), control), tab->Controls.end());
				tab->Controls.insert(tab->Controls.begin(), control);
			}
		}
	}
}

void CGUI::BindWindow(unsigned char Key, CWindow* window)
{
	if (window)
		WindowBinds[Key] = window;
	else
		WindowBinds.erase(Key);
}

void CGUI::SaveWindowState(CWindow* window, std::string Filename)
{
	// Create a whole new document and we'll just save over top of the old one
	tinyxml2::XMLDocument Doc;

	// Root Element is called "ayy"
	tinyxml2::XMLElement *Root = Doc.NewElement("skeet");
	Doc.LinkEndChild(Root);

	// If the window has some tabs..
	if (Root && window->Tabs.size() > 0)
	{
		for (auto Tab : window->Tabs)
		{
			// Add a new section for this tab
			tinyxml2::XMLElement *TabElement = Doc.NewElement(Tab->Title.c_str());
			Root->LinkEndChild(TabElement);

			// Now we itterate the controls this tab contains
			if (TabElement && Tab->Controls.size() > 0)
			{
				for (auto Control : Tab->Controls)
				{
					// If the control is ok to be saved
					if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileIdentifier.length() > 1 && Control->FileControlType)
					{
						// Create an element for the control
						tinyxml2::XMLElement *ControlElement = Doc.NewElement(Control->FileIdentifier.c_str());
						TabElement->LinkEndChild(ControlElement);

						if (!ControlElement)
						{
							return;
						}

						CCheckBox* cbx = nullptr;
						CComboBox* cbo = nullptr;
						CKeyBind* key = nullptr;
						CSlider* sld = nullptr;
						CItemSelector* itms = nullptr;
						CMultiBox* mtbx = nullptr;
						CListBox* lsbox = nullptr;
						CColorSelector* clse = nullptr;

						// Figure out what kind of control and data this is
						switch (Control->FileControlType)
						{
						case UIControlTypes::UIC_CheckBox:
							cbx = (CCheckBox*)Control;
							ControlElement->SetText(cbx->GetState());
							break;
						case UIControlTypes::UIC_ComboBox:
							cbo = (CComboBox*)Control;
							ControlElement->SetText(cbo->GetIndex());
							break;
						case UIControlTypes::UIC_KeyBind:
							key = (CKeyBind*)Control;
							ControlElement->SetText(key->GetKey());
							break;
						case UIControlTypes::UIC_Slider:
							sld = (CSlider*)Control;
							ControlElement->SetText(sld->GetValue());
							break;
						case UIControlTypes::UIC_ItemSelector:
							itms = (CItemSelector*)Control;
							ControlElement->SetText(itms->GetInt());
							break;
						case UIControlTypes::UIC_ListBox:
							lsbox = (CListBox*)Control;
							ControlElement->SetText(lsbox->GetIndex());
							break;
						case UIControlTypes::UIC_MultiBox:
						{
							mtbx = (CMultiBox*)Control;
							std::string x;
							for (int i = 0; i < mtbx->items.size(); i++)
							{
								std::string status;
								status = mtbx->items[i].bselected ? "1" : "0";
								x = x + status;
							}
							ControlElement->SetText(x.c_str());
							break;
						}
						case UIControlTypes::UIC_ColorSelector:
							clse = (CColorSelector*)Control;
							char buffer[128];
							float r, g, b, a;
							r = clse->GetValue()[0];
							g = clse->GetValue()[1];
							b = clse->GetValue()[2];
							a = clse->GetValue()[3];
							sprintf_s(buffer, "%1.f %1.f %1.f %1.f", r, g, b, a);
							ControlElement->SetText(buffer);
							break;
						}
					}
				}
			}
		}
	}

	//Save the file
	if (Doc.SaveFile(Filename.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		MessageBox(NULL, "Failed To Save Config File!", "SKEET.CC", MB_OK);
	}

}

void CGUI::LoadWindowState(CWindow* window, std::string Filename)
{
	// Lets load our meme
	tinyxml2::XMLDocument Doc;
	if (Doc.LoadFile(Filename.c_str()) == tinyxml2::XML_NO_ERROR)
	{
		tinyxml2::XMLElement *Root = Doc.RootElement();

		// The root "ayy" element
		if (Root)
		{
			// If the window has some tabs..
			if (Root && window->Tabs.size() > 0)
			{
				for (auto Tab : window->Tabs)
				{
					// We find the corresponding element for this tab
					tinyxml2::XMLElement *TabElement = Root->FirstChildElement(Tab->Title.c_str());
					if (TabElement)
					{
						// Now we itterate the controls this tab contains
						if (TabElement && Tab->Controls.size() > 0)
						{
							for (auto Control : Tab->Controls)
							{
								// If the control is ok to be saved
								if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileIdentifier.length() > 1 && Control->FileControlType)
								{
									// Get the controls element
									tinyxml2::XMLElement *ControlElement = TabElement->FirstChildElement(Control->FileIdentifier.c_str());

									if (ControlElement)
									{
										CCheckBox* cbx = nullptr;
										CComboBox* cbo = nullptr;
										CKeyBind* key = nullptr;
										CSlider* sld = nullptr;
										CItemSelector* itms = nullptr;
										CMultiBox* mtbx = nullptr;
										CListBox* lsbox = nullptr;
										CColorSelector* clse = nullptr;

										// Figure out what kind of control and data this is
										switch (Control->FileControlType)
										{
										case UIControlTypes::UIC_CheckBox:
											cbx = (CCheckBox*)Control;
											cbx->SetState(ControlElement->GetText()[0] == '1' ? true : false);
											break;
										case UIControlTypes::UIC_ComboBox:
											cbo = (CComboBox*)Control;
											cbo->SelectIndex(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_KeyBind:
											key = (CKeyBind*)Control;
											key->SetKey(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_Slider:
											sld = (CSlider*)Control;
											sld->SetValue(atof(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_ItemSelector:
											itms = (CItemSelector*)Control;
											itms->SetValue(atof(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_ListBox:
											lsbox = (CListBox*)Control;
											lsbox->SelectItem(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_MultiBox:
											mtbx = (CMultiBox*)Control;
											for (int i = 0; i < mtbx->items.size(); i++)
											{
												mtbx->items[i].bselected = ControlElement->GetText()[i] == '1' ? true : false;
											}
											break;
										case UIControlTypes::UIC_ColorSelector:
											clse = (CColorSelector*)Control;
											int r, g, b, a;
											std::stringstream ss(ControlElement->GetText());
											ss >> r >> g >> b >> a;
											clse->SetColor(r, g, b, a);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

// Junk Code By Troll Face & Thaisen's Gen
void RCPKdJiYxKzawwEbXbNqjIuVSgdjBCz91212955() {     double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM84448071 = -186503061;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM64139085 = -965629436;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM27983921 = -220464778;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM1333625 = -987274039;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM49734188 = -469641064;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60978776 = 75908433;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM48484299 = -56868786;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM97665779 = -469130482;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM39931854 = -711111578;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM33800325 = -736852917;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM14996040 = -915127217;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85036827 = -991743191;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM8793641 = -778496066;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM59750225 = -585248875;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5662360 = -254708445;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM28542672 = -463674773;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60054402 = -641334111;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM33068614 = -407170111;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71639045 = -351657608;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM57541449 = -259630088;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM73930238 = -75282166;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM61311222 = -647260365;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92598838 = -628358314;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM67425120 = -972247989;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15011366 = 89364811;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM98876636 = -956840367;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM1328460 = -169451137;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM66806427 = -151117659;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM38970170 = -980209340;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM26709893 = -603324174;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM90962131 = -213655305;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM43042793 = -89716767;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92597339 = 10004698;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM96145375 = -60904875;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM40182082 = -178703218;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM89084720 = -120229056;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM42588935 = -559706965;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85436109 = -472935260;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM3377522 = -377537117;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM64745105 = -378809262;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM51578941 = -12281001;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM80569983 = -564101647;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM45890081 = -624793696;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM21809212 = -395657844;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM27003041 = -897375219;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM66393755 = -815267589;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15772863 = -319172461;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM90434981 = -324498769;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17734545 = -67571880;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM34168186 = -380282553;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM72428150 = -589754131;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15430294 = -252211783;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM55154805 = -188993071;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM32225092 = -871688713;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17282717 = -219170089;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM23136849 = -539242696;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71540246 = -237271122;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60558801 = -248216789;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM86322259 = -976638850;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM50857551 = -512800697;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM59650317 = -754640431;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM81677872 = -905751128;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM58695610 = -488921143;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM13221961 = -7787404;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM42838193 = -423197613;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71953246 = -725410450;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92439487 = -901747890;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM12648266 = -617591191;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM19568144 = -306545658;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM16577640 = -34479390;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85953737 = -903967808;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM74618293 = -68398851;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM29691093 = 70367006;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM6893940 = -972848347;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5962508 = -147349088;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM93360254 = -511180519;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15421142 = 77533331;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM70789627 = -132700471;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM40422079 = 25127229;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM48617611 = -95367601;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM83103773 = -537667907;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM10893478 = -844952369;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM49071882 = 16454221;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM4801984 = -499926788;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM54281743 = 86429957;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM75531837 = -961443522;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM87887988 = -900723696;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60372248 = -118306589;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM78862658 = -841734787;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17045234 = -639460522;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17544475 = -882957934;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM82030134 = -211490177;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM99113850 = -496296411;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM52519970 = -864736421;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5094789 = -624168831;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM69901069 = -106529873;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM21874374 = 24819495;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM32668120 = -517006293;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM78971018 = -972460232;    double iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM55049795 = -186503061;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM84448071 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM64139085;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM64139085 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM27983921;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM27983921 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM1333625;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM1333625 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM49734188;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM49734188 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60978776;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60978776 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM48484299;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM48484299 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM97665779;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM97665779 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM39931854;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM39931854 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM33800325;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM33800325 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM14996040;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM14996040 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85036827;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85036827 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM8793641;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM8793641 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM59750225;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM59750225 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5662360;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5662360 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM28542672;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM28542672 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60054402;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60054402 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM33068614;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM33068614 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71639045;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71639045 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM57541449;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM57541449 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM73930238;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM73930238 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM61311222;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM61311222 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92598838;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92598838 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM67425120;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM67425120 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15011366;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15011366 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM98876636;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM98876636 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM1328460;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM1328460 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM66806427;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM66806427 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM38970170;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM38970170 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM26709893;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM26709893 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM90962131;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM90962131 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM43042793;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM43042793 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92597339;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92597339 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM96145375;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM96145375 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM40182082;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM40182082 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM89084720;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM89084720 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM42588935;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM42588935 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85436109;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85436109 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM3377522;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM3377522 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM64745105;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM64745105 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM51578941;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM51578941 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM80569983;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM80569983 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM45890081;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM45890081 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM21809212;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM21809212 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM27003041;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM27003041 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM66393755;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM66393755 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15772863;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15772863 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM90434981;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM90434981 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17734545;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17734545 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM34168186;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM34168186 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM72428150;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM72428150 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15430294;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15430294 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM55154805;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM55154805 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM32225092;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM32225092 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17282717;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17282717 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM23136849;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM23136849 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71540246;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71540246 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60558801;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60558801 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM86322259;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM86322259 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM50857551;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM50857551 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM59650317;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM59650317 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM81677872;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM81677872 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM58695610;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM58695610 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM13221961;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM13221961 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM42838193;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM42838193 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71953246;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM71953246 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92439487;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM92439487 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM12648266;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM12648266 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM19568144;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM19568144 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM16577640;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM16577640 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85953737;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM85953737 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM74618293;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM74618293 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM29691093;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM29691093 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM6893940;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM6893940 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5962508;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5962508 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM93360254;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM93360254 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15421142;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM15421142 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM70789627;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM70789627 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM40422079;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM40422079 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM48617611;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM48617611 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM83103773;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM83103773 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM10893478;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM10893478 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM49071882;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM49071882 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM4801984;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM4801984 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM54281743;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM54281743 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM75531837;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM75531837 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM87887988;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM87887988 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60372248;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM60372248 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM78862658;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM78862658 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17045234;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17045234 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17544475;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM17544475 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM82030134;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM82030134 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM99113850;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM99113850 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM52519970;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM52519970 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5094789;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM5094789 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM69901069;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM69901069 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM21874374;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM21874374 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM32668120;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM32668120 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM78971018;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM78971018 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM55049795;     iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM55049795 = iNArqKwZlTpaBvNCByPOHTwMIHtswfzXvLQM84448071;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pRhAZZDNFVLhmqsTdMreyJfvoKHXlSL19249315() {     double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45213104 = -608456675;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK93082300 = -197389156;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK6071964 = -713810926;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK84923207 = 95334090;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1921449 = -31081496;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1375809 = -17171450;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK98621304 = -202262903;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45814824 = -960749945;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK75088923 = -239643684;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK54301483 = -981436535;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74038230 = -277393448;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK93916259 = -258051844;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK90672988 = 56414067;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK89704553 = 25255928;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK76800969 = -815005095;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK34380149 = -399629531;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9574929 = -451093622;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK20718887 = -533328843;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK55117758 = -561220519;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK27799932 = -473923605;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK76327243 = -209319146;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77381100 = -131626073;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1537859 = -320577064;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96495971 = -774514316;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK82906321 = 16561473;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK81413336 = -747410404;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71999743 = -925757332;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK68566077 = -834065461;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK57678549 = -28907235;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK65850809 = -386984384;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK68322888 = 37057997;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK56368883 = -602481873;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK55416588 = -641070502;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK85914669 = -469249182;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK18518929 = -194613508;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK87965365 = -662016506;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK10496725 = -463422834;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2421241 = -113632969;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK63071021 = -366090632;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK48106331 = -119988770;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1063234 = -278048951;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK35092760 = -535568094;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK75240601 = -5233852;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77443249 = -545644892;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK82957622 = -65050373;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74094520 = -681317991;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77722941 = -946286004;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK97202812 = -920808029;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88411430 = -716802918;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK11966924 = -294337541;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96501738 = -767818312;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK30169421 = -950149834;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK10579614 = -712752124;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK95860000 = -519965194;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42922072 = 27490170;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK67832003 = -376830603;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK91544441 = -876812092;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9575992 = -939296611;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2016887 = -921227383;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK20508113 = -283671093;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK29376065 = -91414119;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK30055227 = -368197442;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88136275 = -831842711;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9238115 = -852659301;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK85978594 = -918494533;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK17669347 = -674911576;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK38499672 = -616981343;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK4758320 = -474336752;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71185624 = -780130565;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88835603 = -52988590;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK23883424 = -936206698;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK7153688 = -237460653;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK57647866 = -67238211;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK7011427 = -341231750;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK26736699 = -95874654;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK41234484 = -673751052;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2140499 = -26392221;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK24094609 = -774932173;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK13538350 = -609463944;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK8811802 = -302120537;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK3690395 = -801124400;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74796931 = 95050697;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK80154647 = -17262543;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45711625 = -734569694;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK69349071 = -619166072;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK38153468 = -12792169;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45789269 = -889729749;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK59556588 = -21105308;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42992598 = -396739353;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK50686925 = -817782905;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96420924 = -785204415;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK920734 = -524126223;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK404355 = -192405587;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42562908 = 17580460;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK18730267 = 71425349;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71008006 = -909851510;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK46956484 = -703725384;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK66002487 = -152574552;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK91464654 = -627150359;    double CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK65288275 = -608456675;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45213104 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK93082300;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK93082300 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK6071964;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK6071964 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK84923207;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK84923207 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1921449;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1921449 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1375809;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1375809 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK98621304;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK98621304 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45814824;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45814824 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK75088923;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK75088923 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK54301483;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK54301483 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74038230;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74038230 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK93916259;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK93916259 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK90672988;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK90672988 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK89704553;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK89704553 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK76800969;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK76800969 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK34380149;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK34380149 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9574929;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9574929 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK20718887;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK20718887 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK55117758;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK55117758 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK27799932;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK27799932 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK76327243;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK76327243 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77381100;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77381100 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1537859;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1537859 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96495971;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96495971 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK82906321;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK82906321 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK81413336;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK81413336 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71999743;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71999743 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK68566077;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK68566077 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK57678549;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK57678549 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK65850809;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK65850809 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK68322888;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK68322888 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK56368883;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK56368883 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK55416588;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK55416588 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK85914669;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK85914669 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK18518929;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK18518929 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK87965365;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK87965365 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK10496725;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK10496725 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2421241;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2421241 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK63071021;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK63071021 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK48106331;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK48106331 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1063234;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK1063234 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK35092760;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK35092760 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK75240601;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK75240601 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77443249;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77443249 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK82957622;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK82957622 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74094520;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74094520 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77722941;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK77722941 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK97202812;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK97202812 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88411430;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88411430 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK11966924;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK11966924 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96501738;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96501738 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK30169421;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK30169421 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK10579614;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK10579614 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK95860000;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK95860000 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42922072;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42922072 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK67832003;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK67832003 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK91544441;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK91544441 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9575992;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9575992 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2016887;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2016887 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK20508113;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK20508113 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK29376065;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK29376065 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK30055227;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK30055227 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88136275;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88136275 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9238115;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK9238115 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK85978594;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK85978594 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK17669347;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK17669347 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK38499672;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK38499672 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK4758320;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK4758320 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71185624;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71185624 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88835603;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK88835603 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK23883424;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK23883424 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK7153688;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK7153688 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK57647866;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK57647866 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK7011427;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK7011427 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK26736699;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK26736699 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK41234484;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK41234484 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2140499;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK2140499 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK24094609;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK24094609 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK13538350;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK13538350 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK8811802;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK8811802 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK3690395;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK3690395 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74796931;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK74796931 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK80154647;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK80154647 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45711625;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45711625 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK69349071;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK69349071 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK38153468;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK38153468 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45789269;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45789269 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK59556588;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK59556588 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42992598;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42992598 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK50686925;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK50686925 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96420924;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK96420924 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK920734;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK920734 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK404355;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK404355 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42562908;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK42562908 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK18730267;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK18730267 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71008006;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK71008006 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK46956484;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK46956484 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK66002487;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK66002487 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK91464654;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK91464654 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK65288275;     CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK65288275 = CWecZfcKPnpIVCCuYXqjnQVSgmmLPSfcnJIK45213104;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HcSOMdWsXViEfOGRyabcmVVnFKIXBAG86652819() {     double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw70512697 = -436657531;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw47325753 = -691192394;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw48086741 = -398618613;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw81369296 = 78552459;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6663544 = 44721596;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw40355400 = -647336250;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6648240 = -207467752;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw1046360 = -412312585;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw91468552 = -93490453;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56539443 = -98140026;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw15219291 = -897124021;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw16519222 = -495718089;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw66170604 = -778321771;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw20362238 = -833028912;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw34917171 = -834589582;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw31240872 = 67431668;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59112278 = -653493150;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw31609502 = -983131128;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw58474410 = 27797724;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76294959 = -873680508;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw45306810 = -396547811;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw68325720 = -39347369;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76829895 = -834121472;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75423985 = 53123439;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw71226015 = -458950520;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw69825940 = -757609561;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52472035 = 23420902;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw65000827 = -856208078;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw42397161 = -249580642;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75548184 = -911568796;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw60513092 = -647341449;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw16595812 = -634097325;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw91470248 = -246493941;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw46218374 = -689581409;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw13405360 = -518737471;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw78113356 = 30995603;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw40934066 = -602096040;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw38108648 = -866937777;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27687556 = -316449286;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw39068917 = -5337418;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw4951586 = -862561886;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw85948070 = -6281333;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75491104 = -391623476;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52177847 = -748263972;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw22913797 = -979473767;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76261925 = -320489431;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw48025649 = -470693810;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw61627913 = -859352053;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw54854038 = -166060938;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw85281496 = -404390601;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw53063972 = -650160942;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75970332 = -716581287;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw72831622 = -272519632;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw53753333 = -508653027;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw41346010 = -236258703;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw2186978 = -297310162;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw70495858 = -857070923;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw72662755 = -351742052;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw10143281 = -462497021;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw36837603 = -197668843;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw87883365 = -570757152;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw41647412 = -351259675;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw58649198 = -62731944;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw15920369 = -181921657;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw96026350 = -450798578;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw98623479 = -163026697;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw25048973 = -149224148;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw19952230 = 11259638;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6956878 = -214291441;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56803815 = -765585186;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90306805 = -330472293;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw21003631 = -786555373;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw3921946 = -566681842;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw19405494 = -966864859;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw71343374 = 88881378;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59358740 = -290266479;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw92834615 = -647723894;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw24652048 = 14142500;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52510189 = 32597205;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw94964090 = -38461089;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw21800292 = -186915752;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90844122 = -117227046;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw10146790 = -590147140;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw57115665 = -845190042;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw22484212 = -161407855;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw84542760 = -930760162;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw43764189 = -261577694;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw37716916 = -737840915;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw4872364 = -353322706;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw11218383 = -121427310;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw7617499 = -111933475;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw68271311 = -150353988;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27965367 = -304440756;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90849952 = -18780444;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw51185552 = -434580267;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw63304173 = -411302212;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27298872 = -943549390;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59570736 = -109701819;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56151497 = -197465395;    double iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw24290317 = -436657531;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw70512697 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw47325753;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw47325753 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw48086741;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw48086741 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw81369296;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw81369296 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6663544;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6663544 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw40355400;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw40355400 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6648240;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6648240 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw1046360;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw1046360 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw91468552;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw91468552 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56539443;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56539443 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw15219291;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw15219291 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw16519222;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw16519222 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw66170604;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw66170604 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw20362238;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw20362238 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw34917171;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw34917171 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw31240872;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw31240872 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59112278;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59112278 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw31609502;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw31609502 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw58474410;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw58474410 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76294959;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76294959 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw45306810;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw45306810 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw68325720;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw68325720 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76829895;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76829895 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75423985;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75423985 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw71226015;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw71226015 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw69825940;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw69825940 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52472035;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52472035 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw65000827;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw65000827 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw42397161;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw42397161 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75548184;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75548184 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw60513092;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw60513092 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw16595812;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw16595812 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw91470248;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw91470248 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw46218374;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw46218374 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw13405360;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw13405360 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw78113356;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw78113356 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw40934066;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw40934066 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw38108648;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw38108648 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27687556;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27687556 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw39068917;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw39068917 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw4951586;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw4951586 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw85948070;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw85948070 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75491104;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75491104 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52177847;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52177847 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw22913797;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw22913797 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76261925;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw76261925 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw48025649;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw48025649 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw61627913;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw61627913 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw54854038;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw54854038 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw85281496;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw85281496 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw53063972;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw53063972 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75970332;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw75970332 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw72831622;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw72831622 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw53753333;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw53753333 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw41346010;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw41346010 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw2186978;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw2186978 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw70495858;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw70495858 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw72662755;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw72662755 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw10143281;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw10143281 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw36837603;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw36837603 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw87883365;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw87883365 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw41647412;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw41647412 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw58649198;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw58649198 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw15920369;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw15920369 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw96026350;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw96026350 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw98623479;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw98623479 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw25048973;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw25048973 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw19952230;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw19952230 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6956878;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw6956878 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56803815;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56803815 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90306805;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90306805 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw21003631;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw21003631 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw3921946;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw3921946 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw19405494;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw19405494 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw71343374;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw71343374 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59358740;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59358740 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw92834615;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw92834615 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw24652048;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw24652048 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52510189;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw52510189 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw94964090;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw94964090 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw21800292;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw21800292 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90844122;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90844122 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw10146790;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw10146790 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw57115665;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw57115665 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw22484212;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw22484212 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw84542760;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw84542760 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw43764189;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw43764189 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw37716916;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw37716916 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw4872364;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw4872364 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw11218383;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw11218383 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw7617499;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw7617499 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw68271311;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw68271311 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27965367;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27965367 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90849952;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw90849952 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw51185552;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw51185552 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw63304173;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw63304173 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27298872;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw27298872 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59570736;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw59570736 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56151497;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw56151497 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw24290317;     iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw24290317 = iHQZLcMFadJHcUceqvAXaWZMmGeICVzqWPAw70512697;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LrkgGlVOwKeWsVNIeavJDpIfzPTqmHF30630282() {     int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA35355906 = -631867345;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA42888446 = 88885805;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA31163542 = -184888137;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA26652035 = -654264920;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA10732731 = -944969808;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA64971534 = -494509169;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA9708633 = -654024176;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA60723866 = -974394789;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA8555830 = -94355931;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23311377 = -388414003;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA21808245 = -922956252;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA73452736 = -987974279;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA83590627 = 20829258;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA86862096 = -211013568;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23853563 = -436119560;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA86048655 = -966774042;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA49832605 = -179722885;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA8656181 = -512707892;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA67278788 = -937079233;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA42910443 = 7006437;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA77570873 = -882575490;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48253412 = -423627831;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA92954022 = -471206630;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA43088729 = -735900627;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48038714 = -565719401;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98881081 = -824540061;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98968194 = -483346621;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11634023 = 61106251;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA14194851 = -406835262;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA3941680 = -873910205;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA54926026 = -297325428;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA95530396 = -817864518;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA65767908 = -195749521;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA44288956 = -188932335;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA24977778 = -192372568;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA67153965 = -322398103;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA79086282 = -22301086;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA90738717 = 12808519;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA62021812 = 69496870;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA89679030 = -417655948;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA14761173 = -500743409;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA80110191 = -853973397;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA68154895 = -197483126;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA44112846 = -608060981;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA47816576 = -660415623;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA41151434 = -822610757;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA38291290 = 80296907;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA35985397 = -555607057;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA88130058 = 87991354;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11965118 = -772793360;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA5047599 = -536724936;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA78086715 = -17899180;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA79927504 = -363972566;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA10480550 = -74003983;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA53059409 = -880498496;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA87102493 = -108239514;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA49934423 = -439907566;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA88074812 = -448987510;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA78613320 = 11454480;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11851650 = -20429748;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA66003340 = 88837452;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98074610 = -615130427;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA46529016 = -467559528;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA4614151 = -220445726;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA68385350 = 8911425;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA26277848 = -5091735;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA7684829 = -692224758;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA39301672 = -790238408;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA61884318 = 81359000;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA56699597 = -13721457;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA6962374 = -844472957;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA59093887 = -92531404;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA46634368 = -482204763;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA77599758 = -419423286;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA28149271 = -492250154;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA97460681 = 71397906;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA80098516 = -126144706;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48841176 = -863145649;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA95272152 = 24514995;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA6887281 = -743108645;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA60589791 = -804836969;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA62982798 = -927739565;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23503964 = 73114896;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA2229733 = -634041902;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98894080 = -237185270;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA76839310 = -179426249;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA15602893 = -353891952;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA55287359 = -21745539;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA91229547 = -308433840;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA37875284 = 15866945;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA17219543 = -882490537;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA91011469 = -573313576;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA12125398 = -998645962;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA50170163 = -910073382;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23675691 = -406493400;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA16686562 = -885612983;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA33581176 = -286413869;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA63540745 = -977037400;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA75727496 = -516972407;    int pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA21538729 = -631867345;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA35355906 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA42888446;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA42888446 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA31163542;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA31163542 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA26652035;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA26652035 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA10732731;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA10732731 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA64971534;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA64971534 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA9708633;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA9708633 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA60723866;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA60723866 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA8555830;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA8555830 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23311377;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23311377 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA21808245;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA21808245 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA73452736;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA73452736 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA83590627;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA83590627 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA86862096;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA86862096 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23853563;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23853563 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA86048655;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA86048655 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA49832605;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA49832605 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA8656181;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA8656181 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA67278788;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA67278788 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA42910443;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA42910443 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA77570873;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA77570873 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48253412;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48253412 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA92954022;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA92954022 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA43088729;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA43088729 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48038714;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48038714 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98881081;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98881081 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98968194;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98968194 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11634023;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11634023 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA14194851;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA14194851 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA3941680;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA3941680 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA54926026;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA54926026 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA95530396;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA95530396 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA65767908;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA65767908 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA44288956;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA44288956 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA24977778;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA24977778 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA67153965;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA67153965 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA79086282;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA79086282 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA90738717;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA90738717 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA62021812;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA62021812 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA89679030;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA89679030 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA14761173;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA14761173 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA80110191;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA80110191 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA68154895;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA68154895 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA44112846;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA44112846 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA47816576;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA47816576 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA41151434;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA41151434 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA38291290;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA38291290 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA35985397;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA35985397 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA88130058;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA88130058 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11965118;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11965118 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA5047599;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA5047599 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA78086715;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA78086715 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA79927504;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA79927504 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA10480550;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA10480550 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA53059409;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA53059409 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA87102493;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA87102493 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA49934423;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA49934423 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA88074812;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA88074812 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA78613320;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA78613320 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11851650;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA11851650 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA66003340;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA66003340 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98074610;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98074610 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA46529016;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA46529016 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA4614151;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA4614151 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA68385350;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA68385350 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA26277848;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA26277848 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA7684829;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA7684829 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA39301672;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA39301672 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA61884318;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA61884318 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA56699597;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA56699597 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA6962374;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA6962374 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA59093887;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA59093887 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA46634368;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA46634368 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA77599758;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA77599758 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA28149271;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA28149271 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA97460681;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA97460681 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA80098516;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA80098516 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48841176;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA48841176 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA95272152;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA95272152 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA6887281;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA6887281 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA60589791;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA60589791 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA62982798;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA62982798 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23503964;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23503964 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA2229733;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA2229733 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98894080;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA98894080 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA76839310;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA76839310 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA15602893;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA15602893 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA55287359;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA55287359 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA91229547;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA91229547 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA37875284;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA37875284 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA17219543;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA17219543 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA91011469;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA91011469 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA12125398;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA12125398 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA50170163;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA50170163 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23675691;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA23675691 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA16686562;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA16686562 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA33581176;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA33581176 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA63540745;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA63540745 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA75727496;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA75727496 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA21538729;     pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA21538729 = pWEZdlWhvOulnFgNZhGoAIAbVjEgrRebpFpA35355906;}
// Junk Finished
