#ifndef WIDGETS_H
#define WIDGETS_H

#include <vector>
#include <utility>
#include <string>

enum class WidgetState
{
	Main,
	Tabs,
	Blank
};

enum class Button
{
	BTN_ACTION1,
	BTN_ACTION2,
	BTN_ACTION3,
	BTN_ACTION4,
	BTN_LEFT,
	BTN_RIGHT,
	BTN_UP,
	BTN_DOWN,
	BTN_MORE,
	BTN_QUIT
};

class Widget
{
public:
	virtual std::string Print() = 0;
	virtual void OnButton(Button btn) = 0;
	static const char* PrintState(WidgetState state)
	{
		switch (state)
		{
			case WidgetState::Main:
				return "Main";
				break;
			case WidgetState::Tabs:
				return "Tabs";
				break;
			case WidgetState::Blank:
			default:
				return "none";
				break;
		}
	}
};

class Tabs : public Widget
{
public:
	Tabs();
	~Tabs();

	virtual std::string Print() override;
	void AddWidget(std::string name, Widget *widget);
	virtual void OnButton(Button btn) override;
	void GoToIndex(int index);
	void ClearTabs();

private:
	int TabIndex;
	std::vector<std::pair<std::string, Widget*>> mWidgets;
};

class Text : public Widget
{
public:
	Text();
	Text(const char* text);
	Text(std::string text);

	virtual std::string Print() override;
	virtual void OnButton(Button btn) override {};
	void SetText(const char *text);
	void SetText(std::string text);

private:
	std::string String;
};

class Playlist : public Widget
{
public:
	Playlist();
	Playlist(std::vector<std::string> list);

	virtual std::string Print() override;
	virtual void OnButton(Button btn) override;
	void AddSong(std::string file);
	void AddSongs(std::vector<std::string> &list);
	void SwapSongs(int first, int second);
	void RemoveSong(int index);
	int GetIndex();
	std::string GetSong();
	void ClearList();

private:
	std::string GetFilename(std::string path);
	int Scroll;
	std::vector<std::string> List;
};

#endif // !WIDGETS_H
