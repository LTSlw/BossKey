#pragma once
#include <Windows.h>
#include <String>
using namespace std;

struct window
{
	HWND hd;
	wstring caption;
};

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
LR"(BossKey v0.1.2
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
bool GetHwnd(HWND& hWnd);