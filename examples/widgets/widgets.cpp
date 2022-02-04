#include "widgets.hpp"
#include <algorithm>

#pragma region Tabs
Tabs::Tabs() :
	TabIndex(0)
{
}

void Tabs::AddWidget(std::string name, Widget *widget)
{
	mWidgets.emplace_back(name, widget);
}

void Tabs::ClearTabs()
{
	mWidgets.clear();
}

void Tabs::GoToIndex(int index)
{
	if (index < 0 || index >= mWidgets.size())
		return;

	TabIndex = index;
}

void Tabs::OnButton(Button btn)
{
	if (mWidgets.empty())
		return;

	switch (btn)
	{
		case Button::BTN_RIGHT:
			if (mWidgets.size() <= 1)
				return;

			TabIndex = (TabIndex + 1) % mWidgets.size();
			break;
		case Button::BTN_LEFT:
			if (mWidgets.size() <= 1)
				return;

			TabIndex = TabIndex == 0 ? mWidgets.size() - 1 : TabIndex - 1;
			break;
		case Button::BTN_ACTION1:
		case Button::BTN_ACTION2:
		case Button::BTN_ACTION3:
		case Button::BTN_ACTION4:
		case Button::BTN_UP:
		case Button::BTN_DOWN:
		case Button::BTN_MORE:
		case Button::BTN_QUIT:
			mWidgets[TabIndex].second->OnButton(btn);
			break;
		default:
			break;
	}
}

std::string Tabs::Print()
{
	std::string str = "<                                               >\n";
	str.append("-------------------------------------------------\n")
	   .append(mWidgets.empty() ? "\n\n\n\n\n" : mWidgets[TabIndex].second->Print())
	   .append("-------------------------------------------------\n")
	   .append("                                                 \n");

	if (!mWidgets.empty())
		str.replace(2, 
			mWidgets[TabIndex].first.length(), 
			mWidgets[TabIndex].first.c_str());

	return str;
}

Tabs::~Tabs()
{
	ClearTabs();
}
#pragma endregion

#pragma region Text
Text::Text()
{
	String = "";
}

Text::Text(const char * text):
	String(text)
{
}

Text::Text(std::string text) :
	String(text)
{
}

std::string Text::Print()
{
	return String;
}

void Text::SetText(const char* text)
{
	String = text;
}

void Text::SetText(std::string text)
{
	String = text;
}
#pragma endregion

#pragma region Playlist
Playlist::Playlist() :
	Scroll(0)
{}

Playlist::Playlist(std::vector<std::string> list) :
	Scroll(0),
	List(list.begin(), list.end())
{
}

void Playlist::AddSong(std::string file)
{
	List.push_back(file);
}

void Playlist::AddSongs(std::vector<std::string> &list)
{
	List.insert(List.end(), list.begin(), list.end());
}

void Playlist::ClearList()
{
	List.clear();
}

std::string Playlist::GetFilename(std::string path)
{
	std::string::size_type slash = path.rfind("/");
	if (slash == std::string::npos)
		slash = path.rfind("\\");
	if (slash == std::string::npos)
		return path;

	return path.substr(slash + 1);
}

int Playlist::GetIndex()
{
	return Scroll;
}

std::string Playlist::GetSong()
{
	if (List.empty())
		return "";

	return List[Scroll];
}

void Playlist::OnButton(Button btn)
{
	switch (btn)
	{
		case Button::BTN_UP:
			Scroll = std::max(0, Scroll - 1);
			break;
		case Button::BTN_DOWN:
			Scroll = std::min((int) List.size() - 1, Scroll + 1);
			break;
		case Button::BTN_ACTION1:
		case Button::BTN_ACTION2:
		case Button::BTN_ACTION3:
		case Button::BTN_ACTION4:
		case Button::BTN_LEFT:
		case Button::BTN_RIGHT:
		case Button::BTN_MORE:
		case Button::BTN_QUIT:
		default:
			break;
	}
}

std::string Playlist::Print()
{
	std::string str = "^                                                \n";
	str.append("                                                 \n")
		.append("v                                                \n");

	if (List.empty())
		return str;

	int i = 0;
	for (auto it = List.begin() + Scroll; it != List.end(); ++it)
	{
		std::string file = GetFilename(*it);
		str.replace(2 + (50 * i), file.length(), file);
		if (++i >= 3)
			break;
	}

	if (Scroll == 0)
		str.replace(0, 1, " ");
	else if (Scroll == List.size() - 1)
		str.replace(100, 1, " ");

	str.append("(1) Play, (2) Remove, (3) Clear, (4) Reload\n");

	return str;
}

void Playlist::RemoveSong(int index)
{
	if (index < 0 || index >= List.size())
		return;

	List.erase(List.begin() + index);

	Scroll = 0;
}

void Playlist::SwapSongs(int first, int second)
{
	if (first < 0 || first >= List.size() || second < 0 || second >= List.size())
		return;

	if (first == second)
		return;
	
	std::string temp = List[first];
	List[first] = List[second];
	List[second] = temp;

	if (Scroll == first)
		Scroll = second;
	else if (Scroll == second)
		Scroll = first;
}
#pragma endregion