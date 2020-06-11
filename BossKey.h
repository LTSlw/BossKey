#pragma once
#include <Windows.h>
#include <String>
using namespace std;

#define WM_MY_HKREG WM_USER + 1
#define WM_MY_HKUNREG WM_USER + 2

struct window
{
	HWND hd;
	wstring caption;
};
struct hkWindow
{
	HWND hwnd;
	hkWindow* nextwin;
};
struct hotkey
{
	int id;
	bool keyon;
	UINT mod;
	UINT vkcode;
	hkWindow* win;
	hotkey* nextkey;
};

HINSTANCE mhInstance = NULL;
TCHAR wcname[] = TEXT("HotKeyWindow");
bool Get_EndLine = 0, Get_BadInput = 0;
const wstring HelpWord =
LR"(BossKey帮助

 - Exit 退出BossKey
 - Help 显示帮助
 - Hide 隐藏窗口
     [-h] 指定hwnd
     [-n] 窗口序号
 - List 列出窗口列表
     [-f] 刷新窗口列表
         [-a] 列出所有窗口（包括隐藏的窗口，标题为空的窗口)
         [-e] 不列出标题不为空的窗口
         [-t] 仅列出任务栏中的窗口（默认）
     [-o] 不刷新窗口列表（默认）
 - Reg 热键设置
     [-i] 注册热键（默认），与 -u 不共存
         [-h] 指定hwnd
         [-n] 窗口序号
         [-k] 热键
     [-u] 反注册热键
         [-h] 指定hwnd
         [-n] 窗口序号
         [-k] 热键
 - Show 显示窗口
     [-h] 指定hwnd
     [-n] 窗口序号

)",
WelcomeWord =
LR"(BossKey v0.2.1
作者：LTS
==================================

)",

WrongWord =
LR"(错误！
使用Help命令查看帮助

)";

wstring GetWord();
inline void GetClear();
bool GetHwnd(HWND& hWnd);
void UpperToLower(wstring& upstr);
bool EndLineTry();
bool GetHotKey(UINT& mod, UINT& vkcode);

void ListWindow();
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
bool HideWindow();
bool UnHideWindow();

bool SetHotKey();
void HotKeyThreadFunc();
LRESULT CALLBACK HotKeyWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);