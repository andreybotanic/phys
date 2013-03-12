#include "renderer.h"
using namespace std;

void renderer::init(simulator *s)
{

	screen = new CClientDC(FromHandle(m_hWnd));
	buffer = new CMemDC(screen);
	screen_dc = screen->GetHDC();
	buffer_dc = buffer->GetHDC();
	screen_dim = GetClientRect();
	buffer_bmp.CreateCompatibleBitmap(screen, screen_dim.right, screen_dim.bottom);
	buffer->SelectObject(&buffer_bmp);
	buffer->SetBkColor(BGCLR);
	bgbrush = CreateSolidBrush(BGCLR);
	bgpen =  GetStockPen(BLACK_PEN);
	sim = s;
}

void renderer::draw_triangle(POINT cnt[3], HBRUSH *brsh, HPEN *pn)
{
	SelectBrush(buffer_dc, *brsh);
	SelectPen(buffer_dc, *pn);
	Polygon(buffer_dc, cnt, 3);
}

void renderer::Draw()
{
	SelectBrush(buffer_dc, bgbrush);
	SelectPen(buffer_dc, bgpen);
	Rectangle(buffer_dc, 0, 0, screen_dim.right, screen_dim.bottom);
	for (sim->it = sim->bodies.begin(); sim->it < sim->bodies.end(); sim->it++)
		(*sim->it)->draw();
	BitBlt(screen_dc, 0,0, screen_dim.right, screen_dim.bottom, buffer_dc, 0,0, SRCCOPY);
}

LRESULT renderer::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		{
			pnt[c].x = GET_X_LPARAM(lParam);
			pnt[c].y = GET_Y_LPARAM(lParam);
			c++;
			if (c == 3)
			{
				c = 0;
				pb = new pbody(pnt, RGB(255,30,30), 2, sim);
				pb->setAV(float(0.01*(rand()%8) - 0.01*(rand()%8)));
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			mouse.x = GET_X_LPARAM(lParam);
			mouse.y = GET_Y_LPARAM(lParam);
		}
	}
	return WndProcDefault(uMsg, wParam, lParam);
}