#include "renderer.h"
using namespace std;

void renderer::init(simulator *s, ui *u)
{
	sim = s;
	UI = u;
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
	if (UI->state == RMBB)
	{
		SelectPen(buffer_dc, bgpen);
		MoveToEx(buffer_dc, UI->force_line_begin.x, UI->force_line_begin.y, NULL);
		LineTo(buffer_dc, UI->mouse_pos.x, UI->mouse_pos.y);
	}
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
		UI->click_pos.x = GET_X_LPARAM(lParam);
		UI->click_pos.y = GET_Y_LPARAM(lParam);
		UI->onLMBD();
		break;
	case WM_LBUTTONUP:
		UI->onLMBU();
		break;
	case WM_MOUSEMOVE:
		UI->mouse_pos.x = GET_X_LPARAM(lParam);
		UI->mouse_pos.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONDOWN:
		UI->click_pos.x = GET_X_LPARAM(lParam);
		UI->click_pos.y = GET_Y_LPARAM(lParam);
		UI->onRMBD();
		break;
	case WM_RBUTTONUP:
		UI->onRMBU();
		break;
	default:
		return WndProcDefault(uMsg, wParam, lParam);
		break;
	}
}