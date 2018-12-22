
#include "CommonIncludes.h"
class IDirect3DDevice9;




struct CUSTOMVERTEX {
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
	//  FLOAT tu, tv;   
};


extern void drawfatalpricel(IDirect3DDevice9* pDevice);



