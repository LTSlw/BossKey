#include <iostream>
#include <string>
#include <cstring>
#include <tchar.h>
#include <windows.h>
#include <thread>
#include <cctype>
#include "BossKey.h"
#include <climits>
using namespace std;

int wnum = 0;
window windows[1001]{};
hotkey* hotkeys{};
hotkey* PassWindowHotKey{};
thread HotKeyThread;
HWND HideHwnd = NULL, HotKeyHwnd = NULL, MyHwnd = NULL;

int main()
{
	MyHwnd = GetConsoleWindow();
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);
	HMENU MySysHmenu = GetSystemMenu(MyHwnd, FALSE);
	RemoveMenu(MySysHmenu, SC_CLOSE, MF_BYCOMMAND);

	setlocale(LC_ALL, "chs");
	SetConsoleTitle(TEXT("Bosskey - LTSlw"));
	mhInstance = GetModuleHandle(NULL);

	WNDCLASS wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = HotKeyWindowProc;
	wc.hInstance = mhInstance;
	wc.hCursor = LoadCursor(mhInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wcname;
	RegisterClass(&wc);

	HotKeyThread = thread(HotKeyThreadFunc);
	HotKeyThread.detach();
	hotkeys = new hotkey{};
	hotkeys->id = 0;
	hotkeys->mod = MOD_WIN | MOD_ALT;
	hotkeys->vkcode = 'Q';
	hotkeys->win = new hkWindow{};
	hotkeys->win->hwnd = MyHwnd;

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
		else if (Command == L"Reg")
			SetHotKey();
		else
			wcout << WrongWord;
		GetClear();
	}
	PostMessage(HotKeyHwnd, WM_CLOSE, 0, 0);
	UnregisterClass(wcname, mhInstance);
	hotkey* hk = hotkeys;
	while (hk != NULL)
	{
		hotkey* hktemp = hk;
		hk = hk->nextkey;
		hkWindow* hkw = hktemp->win;
		while (hkw != NULL)
		{
			hkWindow* hkwtemp = hkw;
			hkw = hkw->nextwin;
			delete hkwtemp;
		}
		delete hktemp;
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
	if (!wcin)
		wcin.clear();
	while (!Get_EndLine)
		GetWord();
	Get_EndLine = 0;
	Get_BadInput = 0;
}

bool GetHwnd(HWND& hWnd)
{
	long long HwndTemp;
	wcin >> hex >> HwndTemp >> dec;
	if (!wcin)
	{
		Get_BadInput = 1;
		return 0;
	}
	EndLineTry();
	hWnd = (HWND)HwndTemp;
	return 1;
}

void UpperToLower(wstring& upstr)
{
	for (unsigned int i = 0; i < upstr.length(); i++)
		upstr[i] = tolower(upstr[i]);
}

bool EndLineTry()
{
	wchar_t tempEndLine;
	wcin.get(tempEndLine);
	if (tempEndLine == L'\n')
		Get_EndLine = 1;
	return Get_EndLine;
}

bool GetHotKey(UINT& mod, UINT& vkcode)
{
	wstring Key, KeyStr = GetWord();
	bool modtmp[4]{};
	wcout << hex;
	while (KeyStr != L"")
	{
		//Key = GetWord();
		size_t pluspos = KeyStr.find(L"+");
		if (pluspos != KeyStr.npos)
		{
			Key = KeyStr.substr(0, pluspos);
			KeyStr = KeyStr.substr(pluspos + 1);
		}
		else
		{
			Key = KeyStr;
			KeyStr = L"";
		}
		UINT vkcodetmp = vkcode;
		if (Key == L"Ctrl")
		{
			if (modtmp[0])
				return 0;
			modtmp[0] = true;
			continue;
		}
		else if (Key == L"Win")
		{
			if (modtmp[1])
				return 0;
			modtmp[1] = true;
			continue;
		}
		else if (Key == L"Alt")
		{
			if (modtmp[2])
				return 0;
			modtmp[2] = true;
			continue;
		}
		else if (Key == L"Shift")
		{
			if (modtmp[2])
			{
				wcout << WrongWord;
				return 0;
			}
			modtmp[3] = true;
			continue;
		}
		else if (vkcode != 0)
			return 0;
		else if (Key == L"LBUTTON")
			vkcode = VK_LBUTTON;
		else if (Key == L"RBUTTON")
			vkcode = VK_RBUTTON;
		else if (Key == L"CANCEL")
			vkcode = VK_CANCEL;
		else if (Key == L"MBUTTON")
			vkcode = VK_MBUTTON;
		else if (Key == L"XBUTTON1")
			vkcode = VK_XBUTTON1;
		else if (Key == L"XBUTTON2")
			vkcode = VK_XBUTTON2;
		else if (Key == L"0x07") //Undefined
			vkcode = 0x07;
		else if (Key == L"BACKSPACE")
			vkcode = VK_BACK;
		else if (Key == L"TAB")
			vkcode = VK_TAB;
		else if (Key == L"0x0A") //Reserved
			vkcode = 0x0A;
		else if (Key == L"0x0B") //Reserved
			vkcode = 0x0B;
		else if (Key == L"CLEAR")
			vkcode = VK_CLEAR;
		else if (Key == L"ENTER")
			vkcode = VK_RETURN;
		else if (Key == L"0x0E") //Undefined
			vkcode = 0x0E;
		else if (Key == L"0x0F") //Undefined
			vkcode = 0x0F;
		else if (Key == L"SHIFT")
			vkcode = VK_SHIFT;
		else if (Key == L"CONTROL")
			vkcode = VK_CONTROL;
		else if (Key == L"ALT")
			vkcode = VK_MENU;
		else if (Key == L"PAUSE")
			vkcode = VK_PAUSE;
		else if (Key == L"CAPSLOCK")
			vkcode = VK_CAPITAL;
		else if (Key == L"IME_KANA")
			vkcode = VK_KANA;
		else if (Key == L"IME_HANGUEL")
			vkcode = VK_HANGEUL;
		else if (Key == L"IME_HANGUL")
			vkcode = VK_HANGUL;
		else if (Key == L"IME_ON")
			vkcode = 0x16;
		else if (Key == L"IME_JUNJA")
			vkcode = VK_JUNJA;
		else if (Key == L"IME_FINAL")
			vkcode = VK_FINAL;
		else if (Key == L"IME_HANJA")
			vkcode = VK_HANJA;
		else if (Key == L"IME_KANJI")
			vkcode = VK_KANJI;
		else if (Key == L"IME_OFF")
			vkcode = 0x1A;
		else if (Key == L"ESC")
			vkcode = VK_ESCAPE;
		else if (Key == L"IME_CONVERT")
			vkcode = VK_CONVERT;
		else if (Key == L"IME_NONCONVERT")
			vkcode = VK_NONCONVERT;
		else if (Key == L"IME_ACCEPT")
			vkcode = VK_ACCEPT;
		else if (Key == L"IME_MODECHANGE")
			vkcode = VK_MODECHANGE;
		else if (Key == L"SPACE")
			vkcode = VK_SPACE;
		else if (Key == L"PAGEUP")
			vkcode = VK_PRIOR;
		else if (Key == L"PAGEDOWN")
			vkcode = VK_NEXT;
		else if (Key == L"END")
			vkcode = VK_END;
		else if (Key == L"HOME")
			vkcode = VK_HOME;
		else if (Key == L"LEFT")
			vkcode = VK_LEFT;
		else if (Key == L"UP")
			vkcode = VK_UP;
		else if (Key == L"RIGHT")
			vkcode = VK_RIGHT;
		else if (Key == L"DOWN")
			vkcode = VK_DOWN;
		else if (Key == L"SELECT")
			vkcode = VK_SELECT;
		else if (Key == L"PRINT")
			vkcode = VK_PRINT;
		else if (Key == L"EXECUTE")
			vkcode = VK_EXECUTE;
		else if (Key == L"PRINTSCREEN")
			vkcode = VK_SNAPSHOT;
		else if (Key == L"INSERT")
			vkcode = VK_INSERT;
		else if (Key == L"DELETE")
			vkcode = VK_DELETE;
		else if (Key == L"HELP")
			vkcode = VK_HELP;
		else if (Key == L"0")
			vkcode = '0';
		else if (Key == L"1")
			vkcode = '1';
		else if (Key == L"2")
			vkcode = '2';
		else if (Key == L"3")
			vkcode = '3';
		else if (Key == L"4")
			vkcode = '4';
		else if (Key == L"5")
			vkcode = '5';
		else if (Key == L"6")
			vkcode = '6';
		else if (Key == L"7")
			vkcode = '7';
		else if (Key == L"8")
			vkcode = '8';
		else if (Key == L"9")
			vkcode = '9';
		else if (Key == L"0x3A") //Undefined
			vkcode = 0x3A;
		else if (Key == L"0x3B") //Undefined
			vkcode = 0x3B;
		else if (Key == L"0x3C") //Undefined
			vkcode = 0x3C;
		else if (Key == L"0x3D") //Undefined
			vkcode = 0x3D;
		else if (Key == L"0x3E") //Undefined
			vkcode = 0x3E;
		else if (Key == L"0x3F") //Undefined
			vkcode = 0x3F;
		else if (Key == L"0x40") //Undefined
			vkcode = 0x40;
		else if (Key == L"A")
			vkcode = 'A';
		else if (Key == L"B")
			vkcode = 'B';
		else if (Key == L"C")
			vkcode = 'C';
		else if (Key == L"D")
			vkcode = 'D';
		else if (Key == L"E")
			vkcode = 'E';
		else if (Key == L"F")
			vkcode = 'F';
		else if (Key == L"G")
			vkcode = 'G';
		else if (Key == L"H")
			vkcode = 'H';
		else if (Key == L"I")
			vkcode = 'I';
		else if (Key == L"J")
			vkcode = 'J';
		else if (Key == L"K")
			vkcode = 'K';
		else if (Key == L"L")
			vkcode = 'L';
		else if (Key == L"M")
			vkcode = 'M';
		else if (Key == L"N")
			vkcode = 'N';
		else if (Key == L"O")
			vkcode = 'O';
		else if (Key == L"P")
			vkcode = 'P';
		else if (Key == L"Q")
			vkcode = 'Q';
		else if (Key == L"R")
			vkcode = 'R';
		else if (Key == L"S")
			vkcode = 'S';
		else if (Key == L"T")
			vkcode = 'T';
		else if (Key == L"U")
			vkcode = 'U';
		else if (Key == L"V")
			vkcode = 'V';
		else if (Key == L"W")
			vkcode = 'W';
		else if (Key == L"X")
			vkcode = 'X';
		else if (Key == L"Y")
			vkcode = 'Y';
		else if (Key == L"Z")
			vkcode = 'Z';
		else if (Key == L"LWIN")
			vkcode = VK_LWIN;
		else if (Key == L"RWIN")
			vkcode = VK_RWIN;
		else if (Key == L"APP")
			vkcode = VK_APPS;
		else if (Key == L"0x5E") //Reserved
			vkcode = 0x5E;
		else if (Key == L"SLEEP")
			vkcode = VK_SLEEP;
		else if (Key == L"PAD0")
			vkcode = VK_NUMPAD0;
		else if (Key == L"PAD1")
			vkcode = VK_NUMPAD1;
		else if (Key == L"PAD2")
			vkcode = VK_NUMPAD2;
		else if (Key == L"PAD3")
			vkcode = VK_NUMPAD3;
		else if (Key == L"PAD4")
			vkcode = VK_NUMPAD4;
		else if (Key == L"PAD5")
			vkcode = VK_NUMPAD5;
		else if (Key == L"PAD6")
			vkcode = VK_NUMPAD6;
		else if (Key == L"PAD7")
			vkcode = VK_NUMPAD7;
		else if (Key == L"PAD8")
			vkcode = VK_NUMPAD8;
		else if (Key == L"PAD9")
			vkcode = VK_NUMPAD9;
		else if (Key == L"MULTIPLY")
			vkcode = VK_MULTIPLY;
		else if (Key == L"ADD")
			vkcode = VK_ADD;
		else if (Key == L"SEPARATOR")
			vkcode = VK_SEPARATOR;
		else if (Key == L"SUBTRACT")
			vkcode = VK_SUBTRACT;
		else if (Key == L"DECIMAL")
			vkcode = VK_DECIMAL;
		else if (Key == L"DIVIDE")
			vkcode = VK_DIVIDE;
		else if (Key == L"F1")
			vkcode = VK_F1;
		else if (Key == L"F2")
			vkcode = VK_F2;
		else if (Key == L"F3")
			vkcode = VK_F3;
		if (Key == L"F4")
			vkcode = VK_F4;
		else if (Key == L"F5")
			vkcode = VK_F5;
		else if (Key == L"F6")
			vkcode = VK_F6;
		else if (Key == L"F7")
			vkcode = VK_F7;
		else if (Key == L"F8")
			vkcode = VK_F8;
		else if (Key == L"F9")
			vkcode = VK_F9;
		else if (Key == L"F10")
			vkcode = VK_F10;
		else if (Key == L"F11")
			vkcode = VK_F11;
		else if (Key == L"F12")
			vkcode = VK_F12;
		else if (Key == L"F13")
			vkcode = VK_F13;
		else if (Key == L"F14")
			vkcode = VK_F14;
		else if (Key == L"F15")
			vkcode = VK_F15;
		else if (Key == L"F16")
			vkcode = VK_F16;
		else if (Key == L"F17")
			vkcode = VK_F17;
		else if (Key == L"F18")
			vkcode = VK_F18;
		else if (Key == L"F19")
			vkcode = VK_F19;
		else if (Key == L"F20")
			vkcode = VK_F20;
		else if (Key == L"F21")
			vkcode = VK_F21;
		else if (Key == L"F22")
			vkcode = VK_F22;
		else if (Key == L"F23")
			vkcode = VK_F23;
		else if (Key == L"F24")
			vkcode = VK_F24;
		else if (Key == L"0x88") //Unassigned
			vkcode = 0x88;
		else if (Key == L"0x89") //Unassigned
			vkcode = 0x89;
		else if (Key == L"0x8A") //Unassigned
			vkcode = 0x8A;
		else if (Key == L"0x8B") //Unassigned
			vkcode = 0x8B;
		else if (Key == L"0x8C") //Unassigned
			vkcode = 0x8C;
		else if (Key == L"0x8D") //Unassigned
			vkcode = 0x8D;
		else if (Key == L"0x8E") //Unassigned
			vkcode = 0x8E;
		else if (Key == L"0x8F") //Unassigned
			vkcode = 0x8F;
		else if (Key == L"NUMLOCK")
			vkcode = VK_NUMLOCK;
		else if (Key == L"SCROLLLOCK")
			vkcode = VK_SCROLL;
		else if (Key == L"0x92") //OEM specific
			vkcode = 0x92;
		else if (Key == L"0x93") //OEM specific
			vkcode = 0x93;
		else if (Key == L"0x94") //OEM specific
			vkcode = 0x94;
		else if (Key == L"0x95") //OEM specific
			vkcode = 0x95;
		else if (Key == L"0x96") //OEM specific
			vkcode = 0x96;
		else if (Key == L"0x97") //Unassigned
			vkcode = 0x97;
		else if (Key == L"0x98") //Unassigned
			vkcode = 0x98;
		else if (Key == L"0x99") //Unassigned
			vkcode = 0x99;
		else if (Key == L"0x9A") //Unassigned
			vkcode = 0x9A;
		else if (Key == L"0x9B") //Unassigned
			vkcode = 0x07;
		else if (Key == L"0x9C") //Unassigned
			vkcode = 0x9C;
		else if (Key == L"0x9D") //Unassigned
			vkcode = 0x9D;
		else if (Key == L"0x9E") //Unassigned
			vkcode = 0x9E;
		else if (Key == L"0x9F") //Unassigned
			vkcode = 0x9F;
		else if (Key == L"LSHIFT")
			vkcode = VK_LSHIFT;
		else if (Key == L"RSHIFT")
			vkcode = VK_RSHIFT;
		else if (Key == L"LCONTROL")
			vkcode = VK_LCONTROL;
		else if (Key == L"RCONTROL")
			vkcode = VK_RCONTROL;
		else if (Key == L"LALT")
			vkcode = VK_LMENU;
		else if (Key == L"RALT")
			vkcode = VK_RMENU;
		else if (Key == L"BROWSER_BACK")
			vkcode = VK_BROWSER_BACK;
		else if (Key == L"BROWSER_FORWARD")
			vkcode = VK_BROWSER_FORWARD;
		else if (Key == L"BROWSER_REFRESH")
			vkcode = VK_BROWSER_REFRESH;
		else if (Key == L"BROWSER_STOP")
			vkcode = VK_BROWSER_STOP;
		else if (Key == L"BROWSER_SEARCH")
			vkcode = VK_BROWSER_SEARCH;
		else if (Key == L"BROWSER_FAVORITES")
			vkcode = VK_BROWSER_FAVORITES;
		else if (Key == L"BROWSER_HOME")
			vkcode = VK_BROWSER_HOME;
		else if (Key == L"VOLUME_MUTE")
			vkcode = VK_VOLUME_MUTE;
		else if (Key == L"VOLUME_DOWN")
			vkcode = VK_VOLUME_DOWN;
		else if (Key == L"VOLUME_UP")
			vkcode = VK_VOLUME_UP;
		else if (Key == L"MEDIA_NEXT_TRACK")
			vkcode = VK_MEDIA_NEXT_TRACK;
		else if (Key == L"MEDIA_PREV_TRACK")
			vkcode = VK_MEDIA_PREV_TRACK;
		else if (Key == L"MEDIA_STOP")
			vkcode = VK_MEDIA_STOP;
		else if (Key == L"MEDIA_PLAY_PAUSE")
			vkcode = VK_MEDIA_PLAY_PAUSE;
		else if (Key == L"LAUNCH_MAIL")
			vkcode = VK_LAUNCH_MAIL;
		else if (Key == L"LAUNCH_MEDIA_SELECT")
			vkcode = VK_LAUNCH_MEDIA_SELECT;
		else if (Key == L"LAUNCH_APP1")
			vkcode = VK_LAUNCH_APP1;
		else if (Key == L"LAUNCH_APP2")
			vkcode = VK_LAUNCH_APP2;
		else if (Key == L"0xB8") //Reserved
			vkcode = 0xB8;
		else if (Key == L"0xB9") //Reserved
			vkcode = 0xB9;
		else if (Key == L"OEM_1") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the ';:' key
			vkcode = VK_OEM_1;
		else if (Key == L"OEM_PLUS") //For any country/region, the '+' key
			vkcode = VK_OEM_PLUS;
		else if (Key == L"OEM_COMMA") //For any country/region, the ',' key
			vkcode = VK_OEM_COMMA;
		else if (Key == L"OEM_MINUS") //For any country/region, the '-' key
			vkcode = VK_OEM_MINUS;
		else if (Key == L"OEM_PERIOD") //For any country/region, the '.' key
			vkcode = VK_OEM_PERIOD;
		else if (Key == L"OEM_2") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the '/?' key
			vkcode = VK_OEM_2;
		else if (Key == L"OEM_3") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the '`~' key
			vkcode = VK_OEM_3;
		else if (Key == L"0xC1") //Reserved
			vkcode = 0xC1;
		else if (Key == L"0xC2") //Reserved
			vkcode = 0xC2;
		else if (Key == L"0xC3") //Reserved
			vkcode = 0xC3;
		else if (Key == L"0xC4") //Reserved
			vkcode = 0xC4;
		else if (Key == L"0xC5") //Reserved
			vkcode = 0xC5;
		else if (Key == L"0xC6") //Reserved
			vkcode = 0xC6;
		else if (Key == L"0xC7") //Reserved
			vkcode = 0xC7;
		else if (Key == L"0xC8") //Reserved
			vkcode = 0xC8;
		else if (Key == L"0xC9") //Reserved
			vkcode = 0xC9;
		else if (Key == L"0xCA") //Reserved
			vkcode = 0xCA;
		else if (Key == L"0xCB") //Reserved
			vkcode = 0xCB;
		else if (Key == L"0xCC") //Reserved
			vkcode = 0xCC;
		else if (Key == L"0xCD") //Reserved
			vkcode = 0xCD;
		else if (Key == L"0xCE") //Reserved
			vkcode = 0xCE;
		else if (Key == L"0xCF") //Reserved
			vkcode = 0xCF;
		else if (Key == L"0xD0") //Reserved
			vkcode = 0xD0;
		else if (Key == L"0xD1") //Reserved
			vkcode = 0xD1;
		else if (Key == L"0xD2") //Reserved
			vkcode = 0xD2;
		else if (Key == L"0xD3") //Reserved
			vkcode = 0xD3;
		else if (Key == L"0xD4") //Reserved
			vkcode = 0xD4;
		else if (Key == L"0xD5") //Reserved
			vkcode = 0xD5;
		else if (Key == L"0xD6") //Reserved
			vkcode = 0xD6;
		else if (Key == L"0xD7") //Reserved
			vkcode = 0xD7;
		else if (Key == L"0xD8") //Unassigned
			vkcode = 0xD8;
		else if (Key == L"0xD9") //Unassigned
			vkcode = 0xD9;
		else if (Key == L"0xDA") //Unassigned
			vkcode = 0xDA;
		else if (Key == L"OEM_4") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the '[{' key
			vkcode = VK_OEM_4;
		else if (Key == L"OEM_5") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the '\|' key
			vkcode = VK_OEM_5;
		else if (Key == L"OEM_6") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the ']}' key
			vkcode = VK_OEM_6;
		else if (Key == L"OEM_7") //Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the 'single-quote/double-quote' key
			vkcode = VK_OEM_7;
		else if (Key == L"OEM_8") //Used for miscellaneous characters; it can vary by keyboard.
			vkcode = VK_OEM_8;
		else if (Key == L"0xE0") //Reserved
			vkcode = 0xE0;
		else if (Key == L"0xE1") //OEM specific
			vkcode = 0xE1;
		else if (Key == L"OEM_102") //Either the angle bracket key or the backslash key on the RT 102-key keyboard
			vkcode = VK_OEM_102;
		else if (Key == L"0xE3") //OEM specific
			vkcode = 0xE3;
		else if (Key == L"0xE4") //OEM specific
			vkcode = 0xE4;
		else if (Key == L"IME_PROCESS")
			vkcode = VK_PROCESSKEY;
		else if (Key == L"0xE6") //	OEM specific
			vkcode = 0xE6;
		else if (Key == L"PACKET") //Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
			vkcode = VK_PACKET;
		else if (Key == L"0xE8") //Unassigned
			vkcode = 0xE8;
		else if (Key == L"0xE9") //OEM specific
			vkcode = 0xE9;
		else if (Key == L"0xEA") //OEM specific
			vkcode = 0xEA;
		else if (Key == L"0xEB") //OEM specific
			vkcode = 0xEB;
		else if (Key == L"0xEC") //OEM specific
			vkcode = 0xEC;
		if (Key == L"0xED") //OEM specific
			vkcode = 0xED;
		else if (Key == L"0xEE") //OEM specific
			vkcode = 0xEE;
		else if (Key == L"0xEF") //OEM specific
			vkcode = 0xEF;
		else if (Key == L"0xF0") //OEM specific
			vkcode = 0xF0;
		else if (Key == L"0xF1") //OEM specific
			vkcode = 0xF1;
		else if (Key == L"0xF2") //OEM specific
			vkcode = 0xF2;
		else if (Key == L"0xF3") //OEM specific
			vkcode = 0xF3;
		else if (Key == L"0xF4") //OEM specific
			vkcode = 0xF4;
		else if (Key == L"0xF5") //OEM specific
			vkcode = 0xF5;
		else if (Key == L"ATTN")
			vkcode = VK_ATTN;
		else if (Key == L"CRSEL")
			vkcode = VK_CRSEL;
		else if (Key == L"EXSEL")
			vkcode = VK_EXSEL;
		else if (Key == L"EREOF")
			vkcode = VK_EREOF;
		else if (Key == L"PLAY")
			vkcode = VK_PLAY;
		else if (Key == L"ZOOM")
			vkcode = VK_ZOOM;
		else if (Key == L"NONAME") //Reserved
			vkcode = VK_NONAME;
		else if (Key == L"PA1")
			vkcode = VK_PA1;
		else if (Key == L"OEM_CLEAR")
			vkcode = VK_OEM_CLEAR;
		if (vkcode == vkcodetmp)
			return 0;
	}
	mod = (int)modtmp[1] << 3 | (int)modtmp[3] << 2 | (int)modtmp[0] << 1 | (int)modtmp[2];
	return 1;
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
		else if (argument == L"-t")
		{
			if (List_Mode != 0)
			{
				wcout << WrongWord;
				return;
			}
			List_Mode = 3;
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
	else if (List_Mode != 0)
	{
		wcout << WrongWord;
		return;
	}
	for (int i = 1; i <= wnum; ++i)
	{
		wcout << i << " . ";
		for (unsigned int j = 0; j < windows[i].caption.length(); ++j)
		{
			wcout << windows[i].caption[j];
			if (!wcout)
			{
				wcout.clear();
				wcout << L"!";
			}
		}
		wcout << endl;
	}
	wcout << endl;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR Caption[200]{};
	GetWindowText(hWnd, Caption, 200);
	switch (lParam)
	{
	case 0:
	case 3:
		if (IsWindowVisible(hWnd) && (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) != WS_EX_TOOLWINDOW && GetWindowLong(hWnd, GWLP_HWNDPARENT) == 0)
			break;
		else
			return TRUE;
	case 1:
		if (wcscmp(Caption, TEXT("")) == 0)
			return TRUE;
		break;
	case 2:
		break;
	}
	++wnum;
	if (wnum == 1001)
	{
		wcout << L"扫描到的窗口超过1000，后面的将被舍去";
		return FALSE;
	}
	windows[wnum].caption = Caption;
	windows[wnum].hd = hWnd;
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
			if (!wcin)
			{
				wcout << WrongWord;
				return 0;
			}
			EndLineTry();
		}
		else if (argument == L"-h")
		{
			if (Hide_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Hide_Mode = 2;
			if (!GetHwnd(WindowHwnd))
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
		if (WindowIndex < 1 || WindowIndex > wnum)
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
			if (!wcin)
			{
				wcout << WrongWord;
				return 0;
			}
			EndLineTry();
		}
		else if (argument == L"-h")
		{
			if (Show_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Show_Mode = 2;
			if (!GetHwnd(WindowHwnd))
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
		if (WindowIndex < 1 || WindowIndex > wnum)
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

bool SetHotKey()
{
	wstring argument;
	int Reg = 0, Set_Mode = 0, WindowIndex = 0;
	bool IsKey = 0;
	UINT mod = 0, vkcode = 0;
	HWND WindowHwnd = NULL;
	while (!Get_EndLine)
	{
		argument = GetWord();
		if (argument == L"-i")
		{
			if (Reg != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Reg = 1;
		}
		else if (argument == L"-u")
		{
			if (Reg != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Reg = 2;
		}
		else if (argument == L"-n")
		{
			if (Set_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Set_Mode = 1;
			wcin >> WindowIndex;
			if (!wcin)
			{
				wcout << WrongWord;
				return 0;
			}
			EndLineTry();
		}
		else if (argument == L"-h")
		{
			if (Set_Mode != 0)
			{
				wcout << WrongWord;
				return 0;
			}
			Set_Mode = 2;
			if (!GetHwnd(WindowHwnd))
			{
				wcout << WrongWord;
				return 0;
			}
		}
		else if (argument == L"-k")
		{
			if (IsKey)
			{
				wcout << WrongWord;
				return 0;
			}
			IsKey = 1;
			if (!GetHotKey(mod, vkcode))
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

	if (Set_Mode == 1)
	{
		if (WindowIndex < 1 || WindowIndex > wnum)
		{
			wcout << WrongWord;
			return 0;
		}
		WindowHwnd = windows[WindowIndex].hd;
	}

	hotkey * hk = hotkeys;
	hotkey* hkfather = NULL;
	int hkid = -1;
	switch (Reg)
	{
	case 0:
	case 1:	
		if (!IsKey || Set_Mode == 0 || !IsWindow(WindowHwnd))
		{
			wcout << WrongWord;
			return 0;
		}
		while (hk != NULL)
		{
			if (mod == hk->mod && vkcode == hk->vkcode)
				break;
			if (hk->id == hkid + 1)
			{
				++hkid;
				hkfather = hk;
			}
			if (hkid == INT_MAX)
			{
				wcout << WrongWord;
				return 0;
			}
			hk = hk->nextkey;
		}
		++hkid;
		if (hk == NULL)
		{
			hk = new hotkey{};
			hk->id = hkid;
			hk->mod = mod;
			hk->vkcode = vkcode;
			hk->win = new hkWindow{};
			hk->win->hwnd = WindowHwnd;
			if (hkfather == NULL)
			{
				hk->nextkey = hotkeys;
				hotkeys = hk;
			}
			else
			{
				hk->nextkey = hkfather->nextkey;
				hkfather->nextkey = hk;
			}
			PassWindowHotKey = hk;
			PostMessage(HotKeyHwnd, WM_MY_HKREG, 0, 0);
		}
		else
		{
			hkWindow* hkw = hk->win;
			hkWindow* hkwfather = NULL;
			while (hkw != NULL)
			{
				if (hkw->hwnd == WindowHwnd)
					break;
				hkwfather = hkw;
				hkw = hkw->nextwin;
			}
			if (hkw == NULL)
			{
				hkw = new hkWindow{};
				hkw->hwnd = WindowHwnd;
				hkwfather->nextwin = hkw;
			}
		}
		break;
	case 2:
		if (IsKey && Set_Mode != 0 || !IsKey && Set_Mode == 0)
		{
			wcout << WrongWord;
			return 0;
		}
		if (IsKey)
		{
			while (hk != NULL)
			{
				if (hk->mod == mod && hk->vkcode == vkcode)
					break;
				hkfather = hk;
				hk = hk->nextkey;
			}
			if (hk == NULL)
			{
				wcout << WrongWord;
				return 0;
			}
			PostMessage(HotKeyHwnd, WM_MY_HKUNREG, hk->id, 0);
			if (hkfather == NULL)
				hotkeys = hk->nextkey;
			else
				hkfather->nextkey = hk->nextkey;
			delete hk;
		}
		else
		{
			bool IsWinFind = 0;
			while (hk != NULL)
			{
				hkWindow* hkw = hk->win;
				hkWindow* hkwfather = NULL;
				while (hkw != NULL)
				{
					if (hkw->hwnd == WindowHwnd)
					{
						IsWinFind = 1;
						if (hkwfather == NULL)
						{
							hk->win = hkw->nextwin;
							delete hkw;
							hkw = hk->win;
						}
						else
						{
							hkwfather->nextwin = hkw->nextwin;
							delete hkw;
							hkw = hkwfather->nextwin;
						}
					}
					else
					{
						hkwfather = hkw;
						hkw = hkw->nextwin;
					}
				}
				if (hk->win == NULL)
				{
					if (hkfather == NULL)
					{
						hotkeys = hk->nextkey;
						delete hk;
						hk = hotkeys;
					}
					else
					{
						hkfather->nextkey = hk->nextkey;
						PostMessage(HotKeyHwnd, WM_MY_HKUNREG, hk->id, 0);
						delete hk;
						hk = hkfather->nextkey;
					}
				}
				else
				{
					hkfather = hk;
					hk = hk->nextkey;
				}
			}
			if (!IsWinFind)
			{
				wcout << WrongWord;
				return 0;
			}
		}
	}
	wcout << L"完成\n\n";
	hk = hotkeys;
	while (hk != NULL)
	{
		wcout << L"id: " << hk->id << endl;
		wcout << L"mod: " << hk->mod << endl;
		wcout << L"vkcode: " << hk->vkcode << endl;
		wcout << L"io:" << hk->keyon << endl;
		hkWindow* hkw = hk->win;
		while (hkw != NULL)
		{
			wcout << L"Win:" << hkw->hwnd << endl;
			hkw = hkw->nextwin;
		}
		hk = hk->nextkey;
		wcout << endl;
	}
	return 1;
}

void HotKeyThreadFunc()
{
	HotKeyHwnd = CreateWindow(wcname, TEXT(""), WS_OVERLAPPEDWINDOW, 400, 300, 200, 100, NULL, NULL, mhInstance, NULL);
	UpdateWindow(HotKeyHwnd);

	MSG HotKeyMsg;
	while (GetMessage(&HotKeyMsg, NULL, 0, 0))
	{
		TranslateMessage(&HotKeyMsg);
		DispatchMessage(&HotKeyMsg);
	}
}

LRESULT CALLBACK HotKeyWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	hotkey* hk = hotkeys;
	HMENU hMenu = NULL;
	switch (uMsg)
	{
	case WM_CLOSE:
		while (hk != NULL)
		{
			if(!UnregisterHotKey(hWnd, hk->id))
				MessageBox(MyHwnd, L"反注册热键失败", to_wstring(GetLastError()).c_str(), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
			hk = hk->nextkey;
		}
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		hMenu = GetSystemMenu(hWnd, FALSE);
		RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		if (!RegisterHotKey(hWnd, 0, MOD_WIN | MOD_ALT, 'Q'))
			MessageBox(hWnd, L"注册热键失败\n\n可能的原因：热键被占用", L"BossKey", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		break;
	case WM_HOTKEY:
		while (hk != NULL)
		{
			if (wParam == hk->id)
			{
				int nCmd = hk->keyon ? SW_SHOW : SW_HIDE; //传入ShowWindow的参数
				hkWindow* hkw = hk->win;
				while (hkw != NULL)
				{
					ShowWindow(hkw->hwnd, nCmd);
					hkw = hkw->nextwin;
				}
				hk->keyon = hk->keyon - 1;
				break;
			}
			hk = hk->nextkey;
		}
		break;
	case WM_MY_HKREG:
		if (!RegisterHotKey(hWnd, PassWindowHotKey->id, PassWindowHotKey->mod, PassWindowHotKey->vkcode))
			MessageBox(MyHwnd, L"注册热键失败\n\n可能的原因：热键被占用", L"BossKey", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		break;
	case WM_MY_HKUNREG:
		if (!UnregisterHotKey(hWnd, (int)wParam))
			MessageBox(MyHwnd, L"反注册热键失败", L"BossKey", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType)
{
	return TRUE;
}