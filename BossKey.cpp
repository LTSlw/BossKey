#include <iostream>
#include <string>
#include <cstring>
#include <tchar.h>
#include <windows.h>
using namespace std;

struct window
{
	HWND hd;
	wstring caption;
};

int wnum = 0;
window windows[1001]{};
bool Get_EndLine = 0, Get_BadInput = 0;
wstring HelpWord =
LR"(BossKey帮助

 - List 列出窗口列表
     [-a] 列出所有窗口（包括隐藏的窗口，标题为空的窗口)
     [-e] 不列出标题不为空的窗口（默认）
     [-t] 仅列出任务栏中的窗口 待
     [-f] 刷新窗口列表
     [-o] 不刷新窗口列表（默认）
 - Reg 热键设置 待
     [-i] 注册热键（默认），与 -u 不共存
     [-k] 热键（设置 -i 必须）
     [-h] 指定hwnd
     [-n] 窗口序号
     [-u] 反注册热键
 - Hide 隐藏窗口
     [-h] 指定hwnd
     [-n] 窗口序号
 - Show 显示窗口
     [-h] 指定hwnd
     [-n] 窗口序号
 - Help 显示帮助
 - Exit 退出BossKey

)",
WelcomeWord =
LR"(BossKey v0.1.0
作者：LTS
==================================

)",
WrongWord =
LR"(错误！
使用Help命令查看帮助

)";
wstring GetWord();
inline void GetClear();
void ListWindow();
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
bool HideWindow();
bool UnHideWindow();

int main()
{
	setlocale(LC_ALL, "chs");
	wcout << WelcomeWord;
	wstring Command;
	while (1)
	{
		wcout << L">>>";
		Command = GetWord();
		if (Get_BadInput)
		{
			wcout << WrongWord;
			continue;
		}
		if (Command == L"Help")
			wcout << HelpWord;
		else if (Command == L"List")
			ListWindow();
		else if (Command == L"Exit")
			break;
		else if (Command == L"Hide")
			HideWindow();
		else if (Command == L"Show")
			UnHideWindow();
		else
			wcout << WrongWord;
		GetClear();
	}
	return 0;
}

wstring GetWord()
{
	if (Get_BadInput || Get_EndLine)
		return L"";
	TCHAR ch;
	wstring word;
	bool isquo = 0;
	while (wcin.get(ch))
	{
		if (ch == L'\n')
		{
			Get_EndLine = 1;
			return L"";
		}
		else if (ch == L'"')
		{
			isquo = 1;
			break;
		}
		else if (ch == L' ')
			continue;
		else
		{
			word += ch;
			break;
		}
	}
	while (wcin.get(ch))
	{
		if (ch == L'\n')
		{
			Get_EndLine = 1;
			if (isquo)
			{
				Get_BadInput = 1;
				return L"";
			}
			break;
		}
		else if (isquo == 0 && ch == L' ')
			break;
		else if (ch == L'"')
		{
			if (isquo == 1)
				break;
			else
				isquo = 1;
		}
		else
			word += ch;
	}
	return word;
}

inline void GetClear()
{
	while (!Get_EndLine)
		GetWord();
	Get_EndLine = 0;
	Get_BadInput = 0;
	if (!wcin)
		wcin.clear();
}

void ListWindow()
{
	wstring argument;
	int List_Mode = 0, List_Fresh = 0;
	while (!Get_EndLine)
	{
		argument = GetWord();
		if (argument == L"-e")
		{
			if (List_Mode != 0)
			{
				wcout << WrongWord;
				return;
			}
			List_Mode = 1;
		}
		else if (argument == L"-a")
		{
			if (List_Mode != 0)
			{
				wcout << WrongWord;
				return;
			}
			List_Mode = 2;
		}
		else if (argument == L"-o")
		{
			if (List_Fresh != 0)
			{
				wcout << WrongWord;
				return;
			}
			List_Fresh = 1;
		}
		else if (argument == L"-f")
		{
			if (List_Fresh != 0)
			{
				wcout << WrongWord;
				return;
			}
			List_Fresh = 2;
		}
		else
		{
			wcout << WrongWord;
			return;
		}
	}
	wcout << L"窗口列表：" << endl;
	if (List_Fresh == 2)
	{
		wnum = 0;
		EnumWindows(EnumWindowsProc, List_Mode);
	}
	for (int i = 1; i <= wnum; ++i)
		wcout << i << L" . " << windows[i].caption << endl;
}
//方案二
/*
windows[1].hd = GetDesktopWindow();
	GetWindowText(windows[1].hd, windows[1].caption, 200);
	windows[2].hd = GetWindow(windows[1].hd, GW_CHILD);
	while (windows[wnum].hd != NULL)
	{
		GetWindowText(windows[wnum].hd, windows[wnum].caption, 200);
		windows[wnum+1].hd = GetNextWindow(windows[wnum].hd, GW_HWNDNEXT);
		++wnum;
	}
	for (int i = 1; i <= wnum; i++)
		wcout << i << ':' << windows[i].caption << endl;
*/

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR Caption[200]{};
	GetWindowText(hWnd, Caption, 200);
	if (lParam == 0 || lParam == 1)
	{
		if (wcscmp(Caption, TEXT("")) == 0)
			return TRUE;
		++wnum;
		if (wnum == 1001)
		{
			wcout << L"扫描到的窗口超过1000，后面的将被舍去";
			return FALSE;
		}
		windows[wnum].caption = Caption;
		windows[wnum].hd = hWnd;
	}
	else if (lParam == 2)
	{
		++wnum;
		if (wnum == 1001)
		{
			wcout << L"扫描到的窗口超过1000，后面的将被舍去";
			return FALSE;
		}
		windows[wnum].caption = Caption;
		windows[wnum].hd = hWnd;
	}
	return TRUE;
}

bool HideWindow()
{
	wstring argument;
	int Hide_Mode = 0, WindowIndex = 0;
	HWND WindowHwnd = NULL;
	while (!Get_EndLine)
	{
		argument = GetWord();
		if (argument == L"-n")
		{
			if (Hide_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Hide_Mode = 1;
			wcin >> WindowIndex;
			if (!wcin || GetWord() == L"" && Get_BadInput == 1)
			{
				wcout << WrongWord;
				return 0;
			}
		}
		else if (argument == L"-h")
		{
			if (Hide_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Hide_Mode = 2;
			wcin >> WindowHwnd->unused;
			if (!wcin || GetWord() == L"" && Get_BadInput == 1)
			{
				wcout << WrongWord;
				return 0;
			}
		}
		else
		{
			wcout << WrongWord;
			return 0;
		}
	}
	switch (Hide_Mode)
	{
	case 0:
		wcout << WrongWord;
		return 0;
	case 1:
		if (WindowIndex<1 || WindowIndex>wnum)
		{
			wcout << WrongWord;
			return 0;
		}
		WindowHwnd = windows[WindowIndex].hd;
	case 2:
		ShowWindow(WindowHwnd, SW_HIDE);
		wcout << L"完成\n\n";
		return 1;
	}
	return 1;
}

bool UnHideWindow()
{
	wstring argument;
	int Show_Mode = 0, WindowIndex = 0;
	HWND WindowHwnd = NULL;
	while (!Get_EndLine)
	{
		argument = GetWord();
		if (argument == L"-n")
		{
			if (Show_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Show_Mode = 1;
			wcin >> WindowIndex;
			if (!wcin || GetWord() == L"" && Get_BadInput == 1)
			{
				wcout << WrongWord;
				return 0;
			}
		}
		else if (argument == L"-h")
		{
			if (Show_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Show_Mode = 2;
			wcin >> WindowHwnd->unused;
			if (!wcin || GetWord() == L"" && Get_BadInput == 1)
			{
				wcout << WrongWord;
				return 0;
			}
		}
		else
		{
			wcout << WrongWord;
			return 0;
		}
	}
	switch (Show_Mode)
	{
	case 0:
		wcout << WrongWord;
		return 0;
	case 1:
		if (WindowIndex<1 || WindowIndex>wnum)
		{
			wcout << WrongWord;
			return 0;
		}
		WindowHwnd = windows[WindowIndex].hd;
	case 2:
		ShowWindow(WindowHwnd, SW_SHOW);
		wcout << L"完成\n\n";
		return 1;
	}
	return 1;
}