#pragma once

#include "Interfaces.h"

class CResolverData
{
public:
	int index = 0;
	Vector realAngles, oldAngles;
public:
	CResolverData(int _index, Vector _realAngles, Vector _oldAngles)
	{
		this->index = _index;
		this->realAngles = _realAngles;
		this->oldAngles = _oldAngles;
	}
};

class CResolver
{
public:
	std::vector<CResolverData> corrections;
	bool YawResolve2(IClientEntity * player);
	bool YawResolve3(IClientEntity * player);
	bool YawResolve4(IClientEntity * player);
	bool YawResolve5(IClientEntity * player);
	bool YawResolve6(IClientEntity * player);
	bool YawResolve7(IClientEntity * player);
	bool YawResolve8(IClientEntity * player);
	bool YawResolve9(IClientEntity * player);
	void draw_developer_data();
	void legitresolver();
	void add_corrections();
	void apply_corrections(CUserCmd* m_pcmd);
};
extern CResolver resolver;