#include "pbody.h"

float R;
float A, B, C;
pbody::pbody(fpoint cnt[3], COLORREF pclr, int cnt_wdth, simulator *sim)
{
	pbrush = CreateSolidBrush(pclr);
	hlbrush = CreateSolidBrush(RGB(0,200,0));
	cbrush = CreateSolidBrush(RGB(0,0,255));
	ppen = CreatePen(0, cnt_wdth, 0);
	cpen = CreatePen(0, cnt_wdth, RGB(0,255,0));
	centre_g.x = (cnt[0].x+ cnt[1].x+ cnt[2].x)/3;
	centre_g.y = (cnt[0].y+ cnt[1].y+ cnt[2].y)/3;
	l_countour[1] = cnt[1];
	l_countour[2] = cnt[2];
	l_countour[0].x = l_countour[0].y = 0;
	l_countour[1].x -= cnt[0].x;
	l_countour[1].y -= cnt[0].y;
	l_countour[2].x -= cnt[0].x;
	l_countour[2].y -= cnt[0].y;
	sim->UI->vect = (l_countour[1].x  -  l_countour[0].x )* (l_countour[2].y  -  l_countour[1].y ) - (l_countour[2].x  -  l_countour[1].x ) * (l_countour[1].y  -  l_countour[0].y );
	if (sim->UI->vect >= 0) std::reverse(&(l_countour[0]), &(l_countour[2]));
	centre_l.x = (l_countour[0].x+ l_countour[1].x+ l_countour[2].x)/3;
	centre_l.y = (l_countour[0].y+ l_countour[1].y+ l_countour[2].y)/3;
	R = sqrt((l_countour[0].x - centre_l.x)*(l_countour[0].x - centre_l.x)  + (l_countour[0].y - centre_l.y)*(l_countour[0].y - centre_l.y));
	R = max(R, sqrt((l_countour[1].x - centre_l.x)*(l_countour[1].x - centre_l.x)  + (l_countour[1].y - centre_l.y)*(l_countour[1].y - centre_l.y)));
	R = max(R, sqrt((l_countour[2].x - centre_l.x)*(l_countour[2].x - centre_l.x)  + (l_countour[2].y - centre_l.y)*(l_countour[2].y - centre_l.y)));
	gcourecalc();
	re = sim->rend;
	angle = 0;
	ang_vel = 0;
	bbbrush = GetStockBrush(HOLLOW_BRUSH);
	bbpen = CreatePen(2, 1, RGB(0,0,0));
	ishighlited = false;
	lastupdated = GetTickCount();
	mass = abs( ((l_countour[2].x-l_countour[1].x)*(l_countour[1].y-l_countour[0].y)-(l_countour[1].x-l_countour[0].x)*(l_countour[2].y-l_countour[1].y)) / 16.0);
	a = sqrt(pow((l_countour[0].x - l_countour[1].x),2) + pow((l_countour[0].y - l_countour[1].y),2));
	b = sqrt(pow((l_countour[1].x - l_countour[2].x),2) + pow((l_countour[1].y - l_countour[2].y),2));
	c = sqrt(pow((l_countour[2].x - l_countour[0].x),2) + pow((l_countour[2].y - l_countour[0].y),2));
	A = ((b*b - a*a -c*c)/(2.0*a));
	B = pow(((b*b - a*a -c*c)/(2.0*a)), 2.0);
	C = pow(((b*b - a*a -c*c)/(2.0*a)), 2.0);
	I = double((mass * (b*b + A + B + sqrt(abs(b*b - C ))))/1560.0);
	ang_accel = 0;
	sim->bodies.push_back(this);
	vel.x = 0;
	vel.y = 0;
	col_edge = 0;
	coll = false;
	ivel[0].x = ivel[1].x = ivel[2].x = 0;
	ivel[0].y = ivel[1].y = ivel[2].y = 0;
}

void pbody::draw()
{
	re->draw_triangle(g_countour, coll ? & cbrush : (ishighlited ? &hlbrush : &pbrush), &ppen, &cpen, col_edge);
	if (coll)
	{
		MoveToEx(re->buffer_dc, colis.position.x, colis.position.y, NULL);
		LineTo(re->buffer_dc, colis.position.x+colis.vector.x, colis.position.y+colis.vector.y);
	}
	if (DRAWBOUNDING)
	{
		SelectBrush(re->buffer_dc, bbbrush);
		SelectPen(re->buffer_dc, bbpen);
		Rectangle(re->buffer_dc, bbox.left, bbox.top, bbox.right, bbox.bottom);
		SetPixel(re->buffer_dc, centre_g.x, centre_g.y, 0);
		SetPixel(re->buffer_dc, centre_g.x+1, centre_g.y, 0);
		SetPixel(re->buffer_dc, centre_g.x-1, centre_g.y, 0);
		SetPixel(re->buffer_dc, centre_g.x, centre_g.y+1, 0);
		SetPixel(re->buffer_dc, centre_g.x, centre_g.y-1, 0);
	}
}


void pbody::process()
{
	if (bbox.bottom >= re->screen_dim.bottom || bbox.top <= re->screen_dim.top ) vel.y *= -1.0; 
	if (bbox.right >= re->screen_dim.right || bbox.left <= re->screen_dim.left ) vel.x *= -1.0; 
	angle += ang_vel*((GetTickCount() - lastupdated)/1000.0);
	centre_g.x += vel.x;
	centre_g.y += vel.y;
	lastupdated = GetTickCount();
	gcourecalc();
	//impulse = sqrt(vel.x*vel.x + vel.y*vel.y) * mass;
	//impulse_moment = ang_vel * I;

}

void pbody::setAV(float av)
{
	ang_vel = av;
}

void pbody::gcourecalc()
{
	for (_k = 0; _k < 3; _k++)
	{
		_x =  l_countour[_k].x - centre_l.x ;
		_y =  l_countour[_k].y - centre_l.y;
		g_countour[_k].x = long(_x*cos(angle) - _y*sin(angle) + centre_g.x);
		g_countour[_k].y = long(_x*sin(angle) + _y*cos(angle) + centre_g.y);
	}
	bbox.left = bbox.right =  g_countour[0].x;
	bbox.top = bbox.bottom =  g_countour[0].y;
	for (_k = 1; _k < 3; _k++)
	{
		if (g_countour[_k].x <  bbox.left ) bbox.left = g_countour[_k].x;
		if (g_countour[_k].x >  bbox.right ) bbox.right = g_countour[_k].x;
		if (g_countour[_k].y <  bbox.top ) bbox.top = g_countour[_k].y;
		if (g_countour[_k].y >  bbox.bottom ) bbox.bottom = g_countour[_k].y;
	}
	DeleteObject(pol);
	t_cont[0].x = g_countour[0].x * 100.0f;
	t_cont[0].y = g_countour[0].y * 100.0f;

	t_cont[1].x = g_countour[1].x * 100.0f;
	t_cont[1].y = g_countour[1].y * 100.0f;

	t_cont[2].x = g_countour[2].x * 100.0f;
	t_cont[2].y = g_countour[2].y * 100.0f;

	pol = CreatePolygonRgn(t_cont, 3, WINDING);
}

fpoint RA, R1, R2; 
void pbody::addimpulse(fpoint origin, fpoint normal, float impulse)
{
	RA.x  = origin.x - centre_g.x;
	RA.y =  origin.y - centre_g.y;

	vel.x += impulse * normal.x / mass;
	vel.y += impulse * normal.y / mass;

	ang_vel += (impulse * (normal.y * RA.x - normal.x * RA.y) /I);
}


int vcount;
float x_, y_;
float len;
float D;
float _cx1, _cx2, _cy1, _cy2;
float vab;
float Z1, Z2;
float J;
float impulse;
bool co;

pbody *m1, *m2;
void pbody::check_coll(pbody * body)
{
	m1 = this;
	m2 = body;

	for (vcount = 0; vcount < 3; vcount++)
	{
		co = false;
		x_ = this->g_countour[vcount].x;
		y_ = this->g_countour[vcount].y;
		if (x_ < (body->bbox.left) ||  x_ > (body->bbox.right) ||   y_ < (body->bbox.top)  || y_ > (body->bbox.bottom))
			continue;
		if (PtInRegion((body)->pol, int(x_*100.0),int(y_*100.0)))
		{

			colis.position.x = x_;
			colis.position.y = y_;
			if (intersect(centre_g.x, centre_g.y, x_, y_, body->g_countour[0].x, body->g_countour[0].y, body->g_countour[1].x, body->g_countour[1].y))
			{
				body->col_edge = 1;
				colis.vector.y = -1.0*(body->g_countour[0].x -body->g_countour[1].x);
				colis.vector.x = (body->g_countour[0].y -body->g_countour[1].y);
				_cx1 = body->g_countour[0].x;
				_cx2 = body->g_countour[1].x;
				_cy1 = body->g_countour[0].y;
				_cy2 = body->g_countour[1].y;
				co = true;
			}
			else if
				(intersect(centre_g.x, centre_g.y, x_, y_, body->g_countour[1].x, body->g_countour[1].y, body->g_countour[2].x, body->g_countour[2].y))
			{
				body->col_edge = 2;
				colis.vector.y = -1.0*(body->g_countour[1].x -body->g_countour[2].x);
				colis.vector.x = (body->g_countour[1].y -body->g_countour[2].y);
				_cx1 = body->g_countour[1].x;
				_cx2 = body->g_countour[2].x;
				_cy1 = body->g_countour[1].y;
				_cy2 = body->g_countour[2].y;
				co = true;
			}
			else  if
				(intersect(centre_g.x, centre_g.y, x_, y_, body->g_countour[2].x, body->g_countour[2].y, body->g_countour[0].x, body->g_countour[0].y))
			{
				body->col_edge = 3;
				colis.vector.y = -1.0*(body->g_countour[2].x -body->g_countour[0].x);
				colis.vector.x = (body->g_countour[2].y -body->g_countour[0].y);
				_cx1 = body->g_countour[2].x;
				_cx2 = body->g_countour[0].x;
				_cy1 = body->g_countour[2].y;
				_cy2 = body->g_countour[0].y;
				co = true;
			}
			//if (co)
			{
				len = sqrt(colis.vector.x*colis.vector.x+colis.vector.y*colis.vector.y);
				colis.vector.y /= len;
				colis.vector.x /= len;
				D = abs(((_cy1-_cy2)*x_ +(_cx2-_cx1)*y_ + (_cx1*_cy2 - _cx2*_cy1))/sqrt((_cx2-_cx1)*(_cx2-_cx1) + (_cy2-_cy1)*(_cy2-_cy1)));
				colis.vector.x *= D;
				colis.vector.y *= D;
				
				R1.x = colis.position.x - m1->centre_g.x;
				R1.y = colis.position.y - m1->centre_g.y;

				R2.x = colis.position.x - m2->centre_g.x;
				R2.y = colis.position.y - m2->centre_g.y;

				vab = colis.vector.x * (m1->vel.x - m1->ang_vel * R1.y - m2->vel.x + m2->ang_vel * R2.y) + 
					colis.vector.y * (m1->vel.y + m1->ang_vel * R1.x - m2->vel.y - m2->ang_vel* R2.x);

				Z1 = (colis.vector.y * R1.x - colis.vector.x * R1.y) /m1->I;
				Z2 = (colis.vector.y * R2.x - colis.vector.x * R2.y) /m2->I;

				J = colis.vector.x *  (colis.vector.x /m1->mass - R1.y * Z1 + colis.vector.x /m2->mass + R2.y * Z2) 
					+ colis.vector.y * (colis.vector.y /m1->mass + R1.x * Z1 /m1->I
					+ colis.vector.y /m2->mass - R2.x * Z2 /m2->I);

				impulse = -(-(1.0 + 1.0) * vab) / J;

			//	body->centre_g.x -= colis.vector.x;
			//body->centre_g.y -= colis.vector.y;
			//centre_g.x += colis.vector.x;
			//centre_g.y += colis.vector.y;
				
				{
				m2->addimpulse(colis.position, colis.vector, -impulse);
				m1->addimpulse(colis.position, colis.vector, impulse);
				}
				
				break;
			}

		}
	}
}