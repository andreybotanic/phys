#ifndef PBD_H
#define PBD_H
#include "renderer.h"
#include "simulator.h"
#include "helper.h"

class pbody
{
	friend class simulator;
	friend class renderer;
	friend class ui;
private:
	int _x, _y;
	int _k;
	HBRUSH pbrush, bbbrush;		//standart brush; bounding box brush
	HBRUSH hlbrush, stbrush;				//on mouse hover brush
	HPEN ppen, bbpen;			//standart pen; bounding box pen
	HPEN cpen;
	HBRUSH cbrush;
	fpoint l_countour[3];
	fpoint g_countour[3];
	POINT t_cont[3];
	RECT bbox;
	double angle;
	double ang_vel;
	double ang_accel;
	double mass;
	double I;
	fpoint centre_g;
	POINT centre_l;
	renderer *re;
	bool ishighlited;
	unsigned long long lastupdated;
	double inangl;
	HRGN pol;
	void process();
	void draw();
	void gcourecalc();
	fpoint vel;
	double impulse;
	collision colis;
	bool coll;
	int col_edge;
	bool isstatic;
public:
	pbody(fpoint cnt[3], COLORREF pclr, int cnt_wdth, simulator *sim);
	void addimpulse(fpoint origin, fpoint normal, float impulse);
	void check_coll(pbody * body);
};

#endif // PBD_H