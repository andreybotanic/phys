#include "renderer.h"
#include "pbody.h"
#include "ui.h"
#include "simulator.h"
#include <conio.h>

#define tmr1 1

renderer rend;
ui UI;
simulator simul;

void __stdcall tmr(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	simul.process();
	UI.update();
	rend.Draw();
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CWinApp MyApp;
	rend.Create();
	rend.init(&simul, &UI);
	simul.init(&rend, &UI);
	UI.init(&simul, &rend);
	SetTimer(rend.GetHwnd(), tmr1, 0, tmr); 
	return MyApp.Run();
	KillTimer (rend.GetHwnd(), tmr1);
}