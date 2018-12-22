#include "Resolver.h"
#include "Hooks.h"
#include "shonaxsettings.h"
CResolver resolver;
DWORD eyeangles = NetVar.GetNetVar(0xBFEA4E7B);
int ResolverStage[65];

void CResolver::draw_developer_data() {
	if (Shonax::settingsxd.resolverdebug) {
		int pos_y = 50;
		int W, H;
		m_pEngine->GetScreenSize(W, H);
		char buffer_shots[128];
		float shots = game::globals.Shots % 4;
		//sprintf_s(buffer_shots, "Shots: %1.0f", shots);
		//draw.text(W - 150, pos_y, buffer_shots, draw.fonts.menu_bold, Color(255, 255, 255));
		pos_y += 10;

		char buffer_chokedticks[128];
		float choked_ticks = game::globals.choked_ticks;
	//	sprintf_s(buffer_chokedticks, "Choked ticks: %1.0f", choked_ticks);
	//	draw.text(W - 150, pos_y, buffer_chokedticks, draw.fonts.menu_bold, Color(255, 255, 255));
		pos_y += 10;

		if (game::globals.UserCmd) {
			char buffer_realyaw[128];
			static float real_yaw = 0;
			if (!game::globals.SendPacket) real_yaw = game::globals.UserCmd->viewangles.y;
			sprintf_s(buffer_realyaw, "Real yaw: %1.0f", real_yaw);
			draw.text(W - 150, pos_y, buffer_realyaw, draw.fonts.menu_bold, Color(200, 200, 0));
			pos_y += 10;

			char buffer_fakeyaw[128];
			static float fake_yaw = 0;
			if (game::globals.SendPacket) fake_yaw = game::globals.UserCmd->viewangles.y;
			sprintf_s(buffer_fakeyaw, "Fake yaw: %1.0f", fake_yaw);
			draw.text(W - 150, pos_y, buffer_fakeyaw, draw.fonts.menu_bold, Color(200, 20, 0));
			pos_y += 20;
		}
	}
}
///���� �����
bool IsUsingFakeAngles = true;

bool IsEntityMoving(IClientEntity *player)
{
	return (player->GetVelocity().Length2D() > 0.1f && player->GetFlags() & FL_ONGROUND);
}
bool HasFakeHead(IClientEntity* pEntity) {
	//lby should update if distance from lby to eye angles exceeds 35 degrees
	return abs(pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw()) > 35;
}
bool Lbywithin35(IClientEntity* pEntity) {
	//lby should update if distance from lby to eye angles less than 35 degrees
	return abs(pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw()) < 35;
}
bool IsMovingOnGround(IClientEntity* pEntity) {
	//Check if player has a velocity greater than 0 (moving) and if they are onground.
	return pEntity->GetVelocity().Length2D() > 42.f && pEntity->GetFlags() & FL_ONGROUND;
}
bool IsStanding(IClientEntity* pEntity) {
	//Check if player has a velocity greater than 0 (moving) and if they are onground.
	return pEntity->GetVelocity().Length2D() < 10.f && pEntity->GetFlags() & FL_ONGROUND;
}
bool IsMovingOnInAir(IClientEntity* pEntity) {
	//Check if player has a velocity greater than 0 (moving) and if they are onground.
	return !(pEntity->GetFlags() & FL_ONGROUND);
}
bool OnGround(IClientEntity* pEntity) {
	//Check if player has a velocity greater than 0 (moving) and if they are onground.
	return pEntity->GetFlags() & FL_ONGROUND;
}
bool IsFakeWalking(IClientEntity* pEntity) {
	//Check if a player is moving, but at below a velocity of 40
	return IsMovingOnGround(pEntity) && pEntity->GetVelocity().Length2D() < 42.0f;
}

bool CResolver::YawResolve2(IClientEntity *m_entity)
{
	float PlayerIsMoving = abs(m_entity->GetVelocity().Length2D());
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;
	int result; // al@1
	float viewangles = m_entity->GetEyeAngles()->y;

	int v3; // [sp+0h] [bp-4h]@1

	v3 = rand() % 40 + 160;
	result = PlayerIsMoving;
	if (bLowerBodyUpdated)
	{
		result = IsUsingFakeAngles;
		if (bLowerBodyUpdated = -1)
		{
			oldlowerbodyyaw = (viewangles + 16);
			result = IsUsingFakeAngles;
		}
	}
	*(float *)&bLowerBodyUpdated = (float)v3 + *(float *)&oldlowerbodyyaw;
	bLowerBodyUpdated = oldlowerbodyyaw;
	return result;
}
bool CResolver::YawResolve3(IClientEntity *m_entity)
{
	float PlayerIsMoving = abs(m_entity->GetVelocity().Length2D());
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;
	int result; // al@1
	float viewangles = m_entity->GetEyeAngles()->y;


	result = (unsigned __int8)PlayerIsMoving;
	if (bLowerBodyUpdated)
	{
		result = (unsigned __int8)oldlowerbodyyaw;
		if (bLowerBodyUpdated)
		{
			result = m_entity->GetEyeAngles()->y;

			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 90.0);
		}
		else
		{
			if ((m_entity->GetEyeAngles()->y + 16) > m_entity->GetEyeAngles()->y + 16) - 90.0;
		}
	}
	else if (++IsUsingFakeAngles == 1)
	{
		oldlowerbodyyaw = 0;
		result = m_entity->GetEyeAngles()->y;
		if ((m_entity->GetEyeAngles()->y + 16 >  m_entity->GetEyeAngles()->y + 16) - 90.0);
	}
	else
	{
		oldlowerbodyyaw = 1;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 90.0);
		bLowerBodyUpdated = 0;
	}
	return result;
}
bool CResolver::YawResolve4(IClientEntity *m_entity)
{
	float PlayerIsMoving = abs(m_entity->GetVelocity().Length2D());
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;
	int result; // al@1



	result = PlayerIsMoving;
	if (bLowerBodyUpdated)
	{
		result = IsUsingFakeAngles;
		if (oldlowerbodyyaw)
		{
			result = m_entity->GetEyeAngles()->y;
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 90.0);
		}
		else
		{
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 180.0);
		}
	}
	else if (++oldlowerbodyyaw == 1)
	{
		bLowerBodyUpdated = 0;
		result = m_entity->GetEyeAngles()->y;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 180.0);
	}
	else
	{
		IsUsingFakeAngles = 1;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 90.0);
		bLowerBodyUpdated = 0;
	}
	return result;
}
bool CResolver::YawResolve5(IClientEntity *m_entity)
{
	int v1; // eax@1
	int result; // eax@1
	signed __int64 v3; // rtt@1
	float v4; // xmm0_4@4
	float v5; // xmm0_4@6
	signed int v6; // [sp+8h] [bp-4h]@1

	v1 = rand();
	v3 = v1;
	result = v1 / 100;
	v6 = v3 % 100;
	if (v6 < 98)
	{
		result = m_entity->GetEyeAngles()->y;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 180.0);
	}
	if (v6 < 15)
	{
		v4 = (m_entity->GetEyeAngles()->y + 16) + (float)(rand() % 141 - 70);
		result = m_entity->GetEyeAngles()->y;
		if (m_entity->GetEyeAngles()->y + 16 == v4);
	}
	if (v6 == 69)
	{
		v5 = (m_entity->GetEyeAngles()->y + 16) + (float)(rand() % 181 - 90);
		result = m_entity->GetEyeAngles()->y;
		if (m_entity->GetEyeAngles()->y + 16 == v5);
	}
	return result;
}
bool CResolver::YawResolve6(IClientEntity *m_entity)
{
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;
	char result; // al@1


	if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 91.0);
	result = bLowerBodyUpdated == 0;
	oldlowerbodyyaw = result;
	if (result)
	{
		result = m_entity->GetEyeAngles()->y;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 180.0);
	}
	return result;
}
bool CResolver::YawResolve7(IClientEntity *m_entity)
{
	int result; // eax@1
	float PlayerIsMoving = abs(m_entity->GetVelocity().Length2D());
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;


	result = oldlowerbodyyaw;
	if (PlayerIsMoving)
	{
		result = IsUsingFakeAngles;
		if (bLowerBodyUpdated)
		{
			result = oldlowerbodyyaw;
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 0.0);
		}
		else
		{
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 180.0);
		}
	}
	else if (++PlayerIsMoving == 1)
	{
		bLowerBodyUpdated = 0;
		result = m_entity->GetEyeAngles()->y;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 180.0);
	}
	else
	{
		oldlowerbodyyaw = 1;
		if ((m_entity->GetEyeAngles()->y + 16 >  m_entity->GetEyeAngles()->y + 16) + 0.0);
		bLowerBodyUpdated = 0;
	}
	return result;
}

bool CResolver::YawResolve8(IClientEntity *m_entity)
{
	float PlayerIsMoving = abs(m_entity->GetVelocity().Length2D());
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;

	double v2; // [sp+0h] [bp-Ch]@4

	if (PlayerIsMoving)
	{
		bLowerBodyUpdated;
		if (oldlowerbodyyaw == -1)
		{
			bLowerBodyUpdated = 0;
			(&PlayerIsMoving);
		}
	}
	v2 = (double)bLowerBodyUpdated;
	if (oldlowerbodyyaw)
	{
		if (IsUsingFakeAngles)
		{
			if (oldlowerbodyyaw)
				if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 90.0);
				else
					if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 90.0);
		}
		else
		{
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 180.0);
		}
	}
	else if (++PlayerIsMoving == 1)
	{
		bLowerBodyUpdated = 0;
		if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 180.0);
	}
	else
	{
		oldlowerbodyyaw = 1;
		if (bLowerBodyUpdated)
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 90.0);
			else
				if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) + 90.0);
		oldlowerbodyyaw = 0;
	}
	if (v2 >= 0.35)
	{
		IsUsingFakeAngles = bLowerBodyUpdated == 0;
		oldlowerbodyyaw = IsUsingFakeAngles;
	}
	return bLowerBodyUpdated;
}
bool CResolver::YawResolve9(IClientEntity *m_entity)
{
	int idx = m_entity->GetIndex();
	float new_yaw = m_entity->GetLowerBodyYaw();



	//AnimationLayer curBalanceLayer, prevBalanceLayer;

	//SResolveInfo &player_recs = arr_infos[idx];

	float PlayerIsMoving = abs(m_entity->GetVelocity().Length2D());
	bool bLowerBodyUpdated = false;
	bool IsUsingFakeAngles = true;
	float oldlowerbodyyaw;
	bool result; // al@4


	if (IsEntityMoving(m_entity))
	{
		if (oldlowerbodyyaw)
		{
			if (bLowerBodyUpdated)
				if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 226.0);
				else
					if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 134.0);
			result = oldlowerbodyyaw == 0;
			PlayerIsMoving = oldlowerbodyyaw == 0;
		}
		else
		{
			if (oldlowerbodyyaw)
			{
				result = m_entity->GetEyeAngles()->y;
				if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 134.0);
			}
			else
			{
				result = m_entity->GetEyeAngles()->y;
				if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 226.0);
			}
			bLowerBodyUpdated = PlayerIsMoving == 0;
		}
	}
	else if (++bLowerBodyUpdated == 1)
	{
		oldlowerbodyyaw = 0;
		if (bLowerBodyUpdated == true)
		{
			result = m_entity->GetEyeAngles()->y;
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 134.0);
		}
		else
		{
			result = m_entity->GetEyeAngles()->y;
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 226.0);
		}
		PlayerIsMoving = oldlowerbodyyaw == 0;
	}
	else
	{
		PlayerIsMoving = 1;
		if (IsUsingFakeAngles)
			if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 226.0);
			else
				if ((m_entity->GetEyeAngles()->y + 16 > m_entity->GetEyeAngles()->y + 16) - 134.0);
		result = oldlowerbodyyaw == 0;
		bLowerBodyUpdated = PlayerIsMoving == 0;
		IsUsingFakeAngles = 0;
	}
	return result;
}

int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / (float)((uintptr_t)&m_pGlobals->interval_per_tick)) + 0.5) + 1 + (int)((uintptr_t)&m_pGlobals->tickcount);
}

#define SETANGLE 180 // basically getze.us resolver

inline float RandomFloat(float min, float max)
{
	static auto fn = (decltype(&RandomFloat))(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
	return fn(min, max);
}
static bool bLowerBodyIsUpdated = false;

static float LatestLowerBodyYawUpdateTime[55];
static float LatestLowerBodyYawUpdateTime1[55];
static float LowerBodyYawTest;
bool IsMoving;
bool MeetsLBYReq;
float v23; float v24;
double v20;



void CResolver::legitresolver()
{

	if (Shonax::settingsxd.legitresolver)
	{
		auto pLocal = m_pEntityList->GetClientEntity(m_pEngine->GetLocalPlayer());
		{
			for (auto i = 0; i < m_pEntityList->GetHighestEntityIndex(); i++)
			{
				auto pEnt = m_pEntityList->GetClientEntity(i);

				if (!pEnt) continue;

				if (pEnt == pLocal) continue;

				if (pEnt->IsDormant()) continue;

				player_info_t pTemp;

				if (!m_pEngine->GetPlayerInfo(i, &pTemp))
					continue;

				if (pEnt->GetTeamNum() == pLocal->GetTeamNum()) continue;

				auto FYaw = pEnt->GetLowerBodyYaw();
				auto pitch = pEnt->GetEyeAngles()->x;

				pEnt->GetEyeAngles()->y = FYaw;
				float PlayerIsMoving = abs(pEnt->GetVelocity().Length2D());
				bool bLowerBodyUpdated = false;
				bool IsBreakingLBY = false;

				bool isFakeHeading = false;

				float oldLBY = pEnt->GetLowerBodyYaw();


				if (oldLBY != pEnt->GetLowerBodyYaw())
				{
					bLowerBodyUpdated = true;
				}
				else
				{
					bLowerBodyUpdated = false;
				}

				if (pEnt->GetEyeAngles()->y - pEnt->GetLowerBodyYaw() > 35)
				{
					isFakeHeading = true;
				}
				else
				{
					isFakeHeading = false;
				}

				float bodyeyedelta = fabs(pEnt->GetEyeAngles()->y - pEnt->GetLowerBodyYaw());
				if (PlayerIsMoving || bLowerBodyUpdated)// || LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
				{
					
					m_pGlobals->resolvemode = 3;

					pEnt->GetEyeAngles()->y = FYaw;
					oldLBY = pEnt->GetEyeAngles()->y;

					IsBreakingLBY = false;
				}
				else
				{
					m_pGlobals->resolvemode = 1;

					if (bodyeyedelta == 0.f && pEnt->GetVelocity().Length2D() >= 0 && pEnt->GetVelocity().Length2D() < 36)
					{
						pEnt->GetEyeAngles()->y = oldLBY;
						IsBreakingLBY = true;
					}
					else
					{
						IsBreakingLBY = false;
					}
				}

				if (IsBreakingLBY)
				{
					m_pGlobals->resolvemode = 2;

					pEnt->GetEyeAngles()->y = oldLBY;

					switch (m_pGlobals->Shots % 3)
					{
					case 1: pEnt->GetEyeAngles()->y = 180; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + pEnt->GetEyeAngles()->y + rand() % 35;
					}
				}
				else if (!IsBreakingLBY && !isFakeHeading)
				{
					m_pGlobals->resolvemode = 3;

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw();

					switch (m_pGlobals->Shots % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = 45 + rand() % 180;
					case 2: pEnt->GetEyeAngles()->y = oldLBY + rand() % 90;
					case 3: pEnt->GetEyeAngles()->y = 180 + rand() % 90;
					case 4: pEnt->GetEyeAngles()->y = oldLBY + pEnt->GetEyeAngles()->y + rand() % 45;
					}
				}
				else if (isFakeHeading)
				{
					m_pGlobals->resolvemode = 2;

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() - pEnt->GetEyeAngles()->y;

					switch (m_pGlobals->Shots % 2)
					{
					case 1: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetEyeAngles()->y + rand() % 360; break;
					}
				}
				else if (m_pGlobals->Shots >= Shonax::settingsxd.bruteAfterX && Shonax::settingsxd.bruteAfterX != 0)
				{
					m_pGlobals->resolvemode = 2;

					pEnt->GetEyeAngles()->y = 180;

					switch (m_pGlobals->Shots % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180; break;
					case 4: pEnt->GetEyeAngles()->y = oldLBY + rand() % 45; break;
					}
				}
				else
				{
					m_pGlobals->resolvemode = 1;

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;

					switch (m_pGlobals->Shots % 13)
					{
					case 1: pEnt->GetEyeAngles()->y = 180; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + pEnt->GetEyeAngles()->y + rand() % 35;
					case 4: pEnt->GetEyeAngles()->y = 45 + rand() % 180;
					case 5: pEnt->GetEyeAngles()->y = oldLBY + rand() % 90;
					case 6: pEnt->GetEyeAngles()->y = 180 + rand() % 90;
					case 7: pEnt->GetEyeAngles()->y = oldLBY + pEnt->GetEyeAngles()->y + rand() % 45;
					case 8: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 9: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 10: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180; break;
					case 11: pEnt->GetEyeAngles()->y = oldLBY + rand() % 45; break;
					case 12: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 13: pEnt->GetEyeAngles()->y = pEnt->GetEyeAngles()->y + rand() % 360; break;
					}
				}
			}
		}
	}
}


void CResolver::add_corrections() {
	if (m_pEngine->IsInGame() && m_pEngine->IsConnected()) {

		IClientEntity* m_local = game::localdata.localplayer();

		for (int i = 1; i < m_pGlobals->maxClients; i++) {

			auto m_entity = m_pEntityList->GetClientEntity(i);
			if (!m_entity || m_entity == m_local || m_entity->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer || !m_entity->IsAlive()) continue;

			CPlayer* m_player = plist.get_player(i);
			m_player->entity = m_entity;

			bool enable_resolver_y = Shonax::settingsxd.antiaimcorrection;

			int resolvermode_y = Shonax::settingsxd.antiaimcorrectiontype ? 1 : 0;
			if (m_player->ForceYaw && m_player->ForceYaw_Yaw) {
				resolvermode_y = m_player->ForceYaw_Yaw;
				enable_resolver_y = true;
			}

			float angletolerance;
			angletolerance = m_entity->GetEyeAngles()->y + 180.0;
			Vector* m_angles = m_entity->GetEyeAngles();
			Vector at_target_angle;

			game::math.calculate_angle(m_entity->GetOrigin(), m_local->GetOrigin(), at_target_angle);
			game::math.normalize_vector(at_target_angle);
			if (enable_resolver_y) {
				if (resolvermode_y == 1) {
					if (m_entity->GetVelocity().Length2D() > .1) {
						m_player->resolver_data.newer_stored_lby = m_entity->GetLowerBodyYaw();
					}

					float simtime_delta = m_entity->m_flSimulationTime() - m_player->resolver_data.time_at_update;

					if (m_entity->GetVelocity().Length2D() > 36) {
						m_player->resolver_data.stored_lby = m_entity->GetLowerBodyYaw();
						m_player->resolver_data.stored_lby_two = m_entity->GetLowerBodyYaw();
					}

					if (m_player->resolver_data.old_lowerbody_yaws != m_entity->GetLowerBodyYaw()) {
						m_player->resolver_data.old_yaw_deltas = m_entity->GetLowerBodyYaw() - m_player->resolver_data.old_lowerbody_yaws;
						m_player->resolver_data.old_lowerbody_yaws = m_entity->GetLowerBodyYaw();
						m_player->resolver_data.time_at_update = m_entity->m_flSimulationTime();

						if (m_entity->GetVelocity().Length2D() > 0.1f && (m_entity->GetFlags() & FL_ONGROUND)) {
							m_player->resolver_data.temp = m_entity->GetLowerBodyYaw();
							m_player->resolver_data.old_lowerbody_yaws = m_entity->GetLowerBodyYaw();
						}
						else {
							m_player->resolver_data.temp = m_player->resolver_data.old_lowerbody_yaws;
						}
					}
					else {
						m_player->resolver_data.temp = m_entity->GetLowerBodyYaw() - m_player->resolver_data.old_yaw_deltas;
					}

					float fixed_resolve = m_player->resolver_data.temp;

					if (m_entity->GetVelocity().Length2D() > 36) {
						m_angles->y = m_entity->GetLowerBodyYaw();
					}
					else if (simtime_delta > 1.525f) {
						if (simtime_delta > 1.525f && simtime_delta < 2.25f) {
							switch (m_player->resolver_data.shots % 2) {
							case 0:m_angles->y = m_player->resolver_data.stored_lby; break;
							case 1:m_angles->y = m_player->resolver_data.newer_stored_lby; break;
							}
						}
						else if (simtime_delta > 2.25f && simtime_delta < 2.95f) {
							m_angles->y = m_entity->GetLowerBodyYaw();
						}
						else if (simtime_delta > 2.95f) {
							switch (m_player->resolver_data.shots % 3) {
							case 0:m_angles->y = m_player->resolver_data.stored_lby; break;
							case 1:m_angles->y = m_entity->GetLowerBodyYaw(); break;
							case 2:m_angles->y = m_player->resolver_data.newer_stored_lby; break;
							}
						}
					}
					else if (simtime_delta <= 1.525f && simtime_delta > 1.125f) {
						m_angles->y = m_entity->GetLowerBodyYaw();
					}
					else {
						if (simtime_delta <= .57f) {
							m_player->resolver_data.resolved_yaw = m_entity->GetLowerBodyYaw();
						}
						else {
							if (simtime_delta <= .1) {
								m_angles->y = m_entity->GetLowerBodyYaw();
							}
							else if ((fabs(m_entity->GetLowerBodyYaw() - m_player->resolver_data.stored_lby)) >= 65) {
								if ((fabs(m_player->resolver_data.newer_stored_lby - m_player->resolver_data.stored_lby)) >= 55) {
									m_angles->y = m_player->resolver_data.stored_lby;
								}
								else {
									switch (m_player->resolver_data.shots % 2) {
									case 0: m_angles->y = m_player->resolver_data.newer_stored_lby; break;
									case 1: m_angles->y = m_entity->GetLowerBodyYaw() + 180; break;
									}
								}
							}
							else {
								m_angles->y = m_entity->GetLowerBodyYaw();
							}
						}
						m_angles->y = m_player->resolver_data.resolved_yaw;
					}
				}
				else if (resolvermode_y == 1) m_angles->y = m_entity->GetEyeAngles()->y;
				else if (resolvermode_y == 1) m_angles->y = at_target_angle.y - 90;
				else if (resolvermode_y == 1) m_angles->y = at_target_angle.y + 90;
				else if (resolvermode_y == 1) m_angles->y = at_target_angle.y + 180;
			}
			if (Shonax::settingsxd.antiaimcorrectiontype == 2)
			{
				{
					static float oldlby[65];
					static float lastlby[65];
					static float standtime[65];
					static float lbystandtime[65];
					static float last_moving_time[65];
					bool lby_updated;
					float bodyeyedelta = *(float*)((DWORD)m_entity + eyeangles) - m_entity->GetLowerBodyYaw();
					const bool is_moving = m_entity->GetVelocity().Length2D() > 0.1;
					const bool could_be_slowmo = m_entity->GetVelocity().Length2D() < 40;
					const bool is_crouching = m_entity->GetFlags() & FL_DUCKING;
					static float last_moving_lby[64];

					if (lbystandtime[m_entity->GetIndex()] >= 1.1)
					{
						lbystandtime[m_entity->GetIndex()] -= 1.1;
					}
					if (m_entity->GetLowerBodyYaw() != oldlby[m_entity->GetIndex()])
					{
						lby_updated = true;
						oldlby[m_entity->GetIndex()] = m_entity->GetLowerBodyYaw();
					}
					else
					{
						lby_updated = false;
					}

					if (lby_updated)
					{
						*(float*)((DWORD)m_entity + eyeangles) = m_entity->GetLowerBodyYaw();
					}
					else if (is_moving && !could_be_slowmo)
					{
						ResolverStage[m_entity->GetIndex()] = 1;
						last_moving_lby[m_entity->GetIndex()] = m_entity->GetLowerBodyYaw();
						*(float*)((DWORD)m_entity + eyeangles) = m_entity->GetLowerBodyYaw();
					}
					else if (lbystandtime[m_entity->GetIndex()] > 1.0 && lbystandtime[m_entity->GetIndex()] < 1.2)
					{
						ResolverStage[m_entity->GetIndex()] = 4;
						*(float*)((DWORD)m_entity + eyeangles) = m_entity->GetLowerBodyYaw();
					}
					else
					{
						ResolverStage[m_entity->GetIndex()] = 2;
						*(float*)((DWORD)m_entity + eyeangles) = last_moving_lby[m_entity->GetIndex()];
					}
				}
			}
			if (Shonax::settingsxd.antiaimcorrectiontype == 3)
			{
				{
					std::vector<int> HitBoxesToScan;
					if (LatestLowerBodyYawUpdateTime[m_entity->GetIndex()] < m_entity->m_flSimulationTime() || bLowerBodyIsUpdated)
					{
						m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw();
						LatestLowerBodyYawUpdateTime[m_entity->GetIndex()] = m_entity->m_flSimulationTime() + 1.1;
					}
					else {

						if (IsMovingOnGround(m_entity))
						{
							if (IsFakeWalking(m_entity))
							{
								HitBoxesToScan.clear();
								HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
								HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
								HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
								HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
								HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
							}
							m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw();
						}
						else if (IsMovingOnInAir(m_entity))
						{
							switch (m_player->resolver_data.shots % 4)//logging hits for everyhitgroup//not anymore
							{
							case 0:
								m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() - 45;
								break;
							case 1:
								m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() + 45;
								break;
							case 2:
								m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() + 90;
								break;
							case 3:
								m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() - 100;
								break;

							}
						}
						else
						{
							if (HasFakeHead(m_entity))
							{
								m_entity->GetEyeAngles()->y = m_entity->GetEyeAngles()->y - m_entity->GetLowerBodyYaw();
							}

							if (IsMovingOnGround(m_entity))
								m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw();
							else if (MeetsLBYReq && !IsMoving && m_entity->GetHealth())
							{
								if ((m_entity->GetEyeAngles()->y + 180.0) <= 180.0)
								{
									if (angletolerance < -180.0)
										angletolerance = angletolerance + 360.0;
								}
								else
								{
									angletolerance = angletolerance - 360.0;
								}
								v23 = angletolerance - m_entity->GetLowerBodyYaw();
								if (v23 <= 180.0)
								{
									if (v23 < -180.0)
										v23 = v23 + 360.0;
								}
								else
								{
									v23 = v23 - 360.0;
								}
								if (v23 >= 0.0)
									v24 = RandomFloat(0.0, v23 / 2);
								else
									v24 = RandomFloat(v23 / 2, 0.0);
								v20 = v24 + m_entity->GetEyeAngles()->y;
								m_entity->GetEyeAngles()->y = v20;
							}

							else
							{
								if (Lbywithin35(m_entity))
								{
									switch (m_player->resolver_data.missed_shots % 5)
									{
									case 0:
										m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() - 180;
										break;
									case 1:
										m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() - 180;
										break;
									case 2:
										m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() - 45;
										break;
									case 3:
										m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() + 45;
										break;
									case 4:
										m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() + 90;
										break;
									case 5:
										m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() - 90;
										break;
									}
								}
							}
						}
					}
				}
			}
			if (m_player->resolver_data.has_hit_angle) m_angles->y = m_player->resolver_data.last_hit_angle.y;

			if (Shonax::settingsxd.overrideenable && GetAsyncKeyState(Shonax::settingsxd.overridehotkey)) {
				Vector pos_enemy;
				Vector local_target_angle;
				if (game::functions.world_to_screen(m_entity->GetOrigin(), pos_enemy)) {
					game::math.calculate_angle(m_local->GetOrigin(), m_entity->GetOrigin(), local_target_angle);

					POINT mouse = GUI.GetMouse();
					float delta = mouse.x - pos_enemy.x;

					if (delta < 0) m_angles->y = local_target_angle.y + 90;
					else m_angles->y = local_target_angle.y - 90;
				}

			}

			if (m_player->resolver_data.has_hit_angle && m_player->resolver_data.missed_shots >= 2) {
				m_player->resolver_data.has_hit_angle = false;
				m_player->resolver_data.last_hit_angle = Vector(0, 0, 0);
			}

			if (m_player->resolver_data.missed_shots >= 8) {
				m_player->resolver_data.has_hit_angle = false;
				m_player->resolver_data.last_hit_angle = Vector(0, 0, 0);
				m_player->resolver_data.shots = 0;
				m_player->resolver_data.missed_shots = 0;
			}

			if (!Shonax::settingsxd.antiuntrusted && m_entity == game::globals.Target)
				m_angles->x = game::globals.Shots % 5 == 4 ? -90.f : 90.f;
			
			if (m_player->ForcePitch && m_player->ForcePitch_Pitch) {
				float angles[] = { -90.f, 90.f };

				m_angles->x = angles[m_player->ForcePitch_Pitch + 1];
			}

			corrections.push_back(CResolverData(i, *m_angles, *m_entity->GetEyeAngles()));
			if (Shonax::settingsxd.shitdogresolver)
			{
				{
					auto m_entity = m_pEntityList->GetClientEntity(m_pEngine->GetLocalPlayer());
					if (OnGround)
						YawResolve9(m_entity) + YawResolve7(m_entity) + YawResolve8(m_entity) + YawResolve6(m_entity) + YawResolve3(m_entity);
					
					else
						YawResolve2(m_entity) + YawResolve3(m_entity) + YawResolve4(m_entity) + YawResolve5(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve8(m_entity) + YawResolve9(m_entity);
					

					if (IsUsingFakeAngles)
						YawResolve2(m_entity) + YawResolve3(m_entity);
					
					else
						YawResolve2(m_entity) + YawResolve3(m_entity) + YawResolve4(m_entity) + YawResolve5(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve8(m_entity) + YawResolve9(m_entity);
					
					if (IsFakeWalking)
						YawResolve3(m_entity) + YawResolve4(m_entity);
					
					else
						YawResolve4(m_entity) + YawResolve5(m_entity) + YawResolve9(m_entity);
					
					if (IsMovingOnGround)
						YawResolve4(m_entity) + YawResolve5(m_entity) + YawResolve7(m_entity) + YawResolve8(m_entity) + YawResolve9(m_entity);
					else
						YawResolve2(m_entity) + YawResolve4(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve8(m_entity) + YawResolve9(m_entity);
					

					if (IsMovingOnInAir)
						YawResolve2(m_entity) + YawResolve3(m_entity) + YawResolve4(m_entity) + YawResolve5(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve8(m_entity) + YawResolve9(m_entity);
					
					else
						YawResolve2(m_entity) + YawResolve4(m_entity) + YawResolve6(m_entity) + YawResolve8(m_entity);
					
					if (Lbywithin35)
						YawResolve3(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve9(m_entity);
					
					else
						YawResolve8(m_entity) + YawResolve9(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve5(m_entity) + YawResolve3(m_entity) + YawResolve4(m_entity);
					
					if (HasFakeHead)
						m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() + YawResolve8(m_entity) + YawResolve9(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve5(m_entity);
					else
						m_entity->GetLowerBodyYaw() + YawResolve8(m_entity) + YawResolve9(m_entity) + YawResolve6(m_entity) + YawResolve7(m_entity) + YawResolve5(m_entity) + YawResolve3(m_entity) + YawResolve4(m_entity);
					
				}


			}
			if (m_player->resolver_data.has_hit_angle) m_angles->y = m_player->resolver_data.last_hit_angle.y;

			if (Shonax::settingsxd.overrideenable && GetAsyncKeyState(Shonax::settingsxd.overridehotkey)) {
				Vector pos_enemy;
				Vector local_target_angle;
				if (game::functions.world_to_screen(m_entity->GetOrigin(), pos_enemy)) {
					game::math.calculate_angle(m_local->GetOrigin(), m_entity->GetOrigin(), local_target_angle);

					POINT mouse = GUI.GetMouse();
					float delta = mouse.x - pos_enemy.x;

					if (delta < 0) m_angles->y = local_target_angle.y + 90;
					else m_angles->y = local_target_angle.y - 90;
				}

			}
			if (m_player->resolver_data.has_hit_angle && m_player->resolver_data.missed_shots >= 1) {
				m_player->resolver_data.has_hit_angle = false;
				m_player->resolver_data.last_hit_angle = Vector(0, 0, 0);
			}

			if (m_player->resolver_data.missed_shots >= 5) {
				m_player->resolver_data.has_hit_angle = false;
				m_player->resolver_data.last_hit_angle = Vector(0, 0, 0);
				m_player->resolver_data.shots = 0;
				m_player->resolver_data.missed_shots = 0;
			}

			if (!Shonax::settingsxd.antiuntrusted && m_entity == game::globals.Target)
				m_angles->x = game::globals.Shots % 5 == 4 ? -90.f : 90.f;

			if (m_player->ForcePitch && m_player->ForcePitch_Pitch) {
				float angles[] = { -90.f, 90.f };

				m_angles->x = angles[m_player->ForcePitch_Pitch + 1];
			}

			corrections.push_back(CResolverData(i, *m_angles, *m_entity->GetEyeAngles()));
		}
	}
}


void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;
	

	switch (Shonax::settingsxd.antiaimcorrectiontype)
	{

	case 0:
	{
		break;
	}
	case 1:
	{
		bHasAA = ((*flPitch == 90.0f) || (*flPitch == 270.0f));
		bSpinbot = false;

		if (!bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
			&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f) && !bHasAA)
		{
			flYaw = vLast[((IClientEntity*)(pStruct))->GetIndex()].y;
			bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
		}
		else
		{
			if (bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
				&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f))
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
				bSpinbot = true;
			}
			else
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = false;
			}
		}

		vLast[((IClientEntity*)(pStruct))->GetIndex()].y = flYaw;


		bool bTmp = bJitterFix[((IClientEntity*)(pStruct))->GetIndex()];

		bJitterFix[((IClientEntity*)(pStruct))->GetIndex()] = (flYaw >= 180.0f && flYaw <= 360.0f);

		if (bTmp && (flYaw >= 0.0f && flYaw <= 180.0f))
		{
			flYaw += 359.0f;
		}
		break;
	}
	case 2:
	{
		flYaw += 25;
		break;
	}
	case 3:
	{
		int value = rand() % 10;
		switch (value) {
		case 0:flYaw = flYaw; break;
		case 1:flYaw += 0; break;
		case 2:flYaw = flYaw + 180; break;
		case 3:flYaw += 15; break;
		case 4:flYaw = flYaw + 90; break;
		case 5:flYaw -= 15; break;
		case 6:flYaw = flYaw + 270; break;
		case 7:flYaw += 0; break;
		case 8:flYaw = flYaw + 180; break;
		case 9:flYaw -= 45; break;
		case 10:flYaw += 45; break;
			break;
		}
		break;
	}
	case 4:
	{
		int value = rand() % 3 + 0;
		switch (value)
		{
		case 0:
		{
			flYaw = (rand() % 180);
			break;
		}
		case 1:
		{
			flYaw = (rand() % 360);
			break;
		}
		case 2:
		{
			flYaw = 0;
			break;
		}
		}
		break;
	}


	}
	*(float*)(pOut) = flYaw;
}





void CResolver::apply_corrections(CUserCmd* m_pcmd) {
	auto m_entity = m_pEntityList->GetClientEntity(m_pEngine->GetLocalPlayer());
	if (m_pEngine->IsInGame() && m_pEngine->IsConnected() && game::localdata.localplayer()) {
		for each (CResolverData current in resolver.corrections) {
			IClientEntity* ent = (IClientEntity*)m_pEntityList->GetClientEntity(current.index);
			if (!ent || ent == game::localdata.localplayer() || ent->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer || !ent->IsAlive()) continue;
			ent->GetEyeAngles()->y = current.realAngles.y;
			ent->GetEyeAngles()->x = current.realAngles.x;

			float movinglby[64];
			float lbytomovinglbydelta[64];
			bool onground = m_entity->GetFlags() & FL_ONGROUND;
			lbytomovinglbydelta[m_entity->GetIndex()] = m_entity->GetLowerBodyYaw() - lbytomovinglbydelta[m_entity->GetIndex()];

			if (m_entity->GetVecVelocity().Length2D() > 6 && m_entity->GetVecVelocity().Length2D() < 42)
			{

				m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw() + 180;
			}
			else if (m_entity->GetVecVelocity().Length2D() < 6 || m_entity->GetVecVelocity().Length2D() > 42) {
				m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw();
				movinglby[m_entity->GetIndex()] = m_entity->GetLowerBodyYaw();
			}
			else if (lbytomovinglbydelta[m_entity->GetIndex()] > 50 && lbytomovinglbydelta[m_entity->GetIndex()] < -50) {
				m_entity->GetEyeAngles()->y = movinglby[m_entity->GetIndex()];
			}
			else
				m_entity->GetEyeAngles()->y = m_entity->GetLowerBodyYaw();
		}

	}
}

// Junk Code By Troll Face & Thaisen's Gen
void qAMyzvyAhfpcFaPLkYkpQIGYYUivUzc76275793() {     double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68171589 = -536525128;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61737180 = -769281287;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA16570038 = -88772469;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA98481577 = -992155968;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA29295524 = -867589255;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA72318294 = -27412236;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA85364862 = -78382924;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA41005862 = 10414932;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA90151382 = -308594274;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA79905913 = -959893933;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA57885075 = -755412475;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA75248597 = -920882462;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA59847493 = -621328309;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54123370 = -934931737;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9841619 = -180405751;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA356701 = -387802424;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA59919813 = -800213973;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA47145884 = -18021685;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA12615526 = -140306846;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31649093 = -975923005;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA41269748 = -749748686;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA62313293 = -560415651;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61774704 = -657753051;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68567815 = -511480642;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51613459 = 11034049;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51869394 = -299807395;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA22920399 = -613326560;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9405627 = 62440853;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA10888311 = -404405240;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA62258220 = -175930549;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA72326554 = -432753325;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA67836081 = -638913990;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA6722041 = -655209393;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31870089 = -465001523;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA7785408 = 87006174;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58945954 = -98625533;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA42352525 = -880048262;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA35817901 = -372078477;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA63993241 = -683095999;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA3934222 = -325456141;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA16346462 = -762321127;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9909710 = -170127316;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA78690227 = -277197950;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA69004731 = 25398423;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54990292 = -123389297;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA10660637 = -430299281;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA48951832 = -340818368;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA29176829 = -86620666;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51608758 = -867356031;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA70041516 = 87702012;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA12518982 = -755526533;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA66936013 = -4264569;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA537208 = -200925437;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA78157697 = -348397901;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA35006045 = -535897034;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA5858296 = -976109477;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA99962476 = -11528236;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA48002223 = -577291827;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA46868119 = -903190017;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA77426130 = -467781861;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA49397895 = -414085677;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA75959235 = -40823778;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA30117551 = -585179829;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA27893162 = -32663726;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA7579359 = -427140608;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA90048993 = -16498485;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68526557 = -165673070;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA27977404 = -56326787;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA46337962 = -921937912;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA50895664 = 18219782;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58004175 = -507754163;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA24101913 = -328135497;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA83152643 = -334925687;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA8681305 = -814850706;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA15302632 = -113601879;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31360039 = -479621371;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA83623066 = -183217701;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA92769973 = -583151475;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA13577524 = -288091345;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40952823 = -558666670;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA2917562 = -958989028;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA93743570 = -426705894;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA57796869 = -70203116;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40846795 = -392107252;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA49739239 = -420404016;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA5390541 = -328488756;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA67298874 = -337988553;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA28564344 = -206811492;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA96864044 = -929104489;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA1927112 = 63115651;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58983478 = 12902703;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA94350302 = -202756435;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA88949781 = -468888460;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA86567110 = -115314138;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54536326 = -911370465;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40387227 = -621497350;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA79792159 = -584947488;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA50797065 = -144534225;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61425372 = -547460969;    double qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA64941298 = -536525128;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68171589 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61737180;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61737180 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA16570038;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA16570038 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA98481577;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA98481577 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA29295524;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA29295524 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA72318294;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA72318294 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA85364862;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA85364862 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA41005862;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA41005862 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA90151382;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA90151382 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA79905913;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA79905913 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA57885075;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA57885075 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA75248597;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA75248597 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA59847493;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA59847493 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54123370;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54123370 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9841619;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9841619 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA356701;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA356701 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA59919813;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA59919813 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA47145884;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA47145884 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA12615526;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA12615526 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31649093;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31649093 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA41269748;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA41269748 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA62313293;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA62313293 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61774704;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61774704 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68567815;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68567815 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51613459;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51613459 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51869394;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51869394 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA22920399;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA22920399 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9405627;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9405627 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA10888311;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA10888311 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA62258220;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA62258220 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA72326554;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA72326554 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA67836081;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA67836081 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA6722041;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA6722041 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31870089;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31870089 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA7785408;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA7785408 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58945954;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58945954 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA42352525;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA42352525 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA35817901;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA35817901 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA63993241;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA63993241 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA3934222;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA3934222 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA16346462;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA16346462 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9909710;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA9909710 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA78690227;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA78690227 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA69004731;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA69004731 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54990292;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54990292 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA10660637;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA10660637 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA48951832;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA48951832 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA29176829;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA29176829 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51608758;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA51608758 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA70041516;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA70041516 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA12518982;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA12518982 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA66936013;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA66936013 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA537208;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA537208 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA78157697;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA78157697 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA35006045;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA35006045 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA5858296;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA5858296 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA99962476;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA99962476 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA48002223;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA48002223 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA46868119;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA46868119 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA77426130;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA77426130 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA49397895;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA49397895 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA75959235;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA75959235 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA30117551;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA30117551 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA27893162;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA27893162 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA7579359;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA7579359 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA90048993;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA90048993 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68526557;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68526557 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA27977404;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA27977404 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA46337962;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA46337962 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA50895664;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA50895664 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58004175;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58004175 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA24101913;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA24101913 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA83152643;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA83152643 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA8681305;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA8681305 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA15302632;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA15302632 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31360039;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA31360039 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA83623066;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA83623066 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA92769973;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA92769973 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA13577524;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA13577524 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40952823;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40952823 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA2917562;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA2917562 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA93743570;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA93743570 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA57796869;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA57796869 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40846795;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40846795 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA49739239;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA49739239 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA5390541;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA5390541 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA67298874;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA67298874 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA28564344;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA28564344 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA96864044;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA96864044 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA1927112;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA1927112 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58983478;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA58983478 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA94350302;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA94350302 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA88949781;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA88949781 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA86567110;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA86567110 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54536326;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA54536326 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40387227;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA40387227 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA79792159;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA79792159 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA50797065;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA50797065 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61425372;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA61425372 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA64941298;     qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA64941298 = qcfIUQvzZaTerUIhtwvqGGmLitoafNCfhDzA68171589;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZusbZSOreNeyQNMLUEoTscnaGaXuwED43679298() {     double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93471182 = -364725984;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR15980634 = -163084525;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58584816 = -873580156;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94927666 = 91062402;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34037619 = -791786164;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR11297887 = -657577036;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93391797 = -83587773;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR96237396 = -541147708;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR6531012 = -162441043;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR82143873 = -76597424;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR99066136 = -275143048;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR97851559 = -58548707;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR35345109 = -356064146;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR84781054 = -693216577;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR67957820 = -199990237;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR97217423 = 79258774;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR9457163 = 97386498;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58036498 = -467823970;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR15972179 = -651288603;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR80144121 = -275679908;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR10249315 = -936977351;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR53257913 = -468136948;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR37066741 = -71297459;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR47495829 = -783842888;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR39933153 = -464477944;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR40281999 = -310006553;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR3392691 = -764148327;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR5840378 = 40298237;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR95606922 = -625078647;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71955595 = -700514961;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR64516758 = -17152771;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR28063010 = -670529442;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR42775701 = -260632832;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR92173794 = -685333749;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR2671839 = -237117789;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR49093945 = -505613424;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR72789866 = 81278532;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71505308 = -25383284;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR28609776 = -633454653;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94896807 = -210804789;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR20234814 = -246834062;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR60765020 = -740840555;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR78940729 = -663587575;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43739329 = -177220658;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94946466 = 62187309;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR12828041 = -69470721;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR19254540 = -965226173;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93601929 = -25164690;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR18051367 = -316614050;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43356088 = -22351047;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR69081215 = -637869163;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR12736926 = -870696022;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR62789216 = -860692945;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR36051030 = -337085733;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR33429983 = -799645907;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR40213270 = -896589036;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR78913893 = 8212933;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR11088987 = 10262731;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR54994514 = -444459655;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93755619 = -381779611;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR7905196 = -893428709;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR87551419 = -23886011;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR630475 = -916069061;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34575416 = -461926083;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR17627115 = 40555347;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71003126 = -604613606;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR55075858 = -797915875;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43171315 = -670730398;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR82109216 = -356098788;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR18863876 = -694376814;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR24427557 = 97980242;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR37951855 = -877230218;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR29426723 = -834369317;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR21075371 = -340483814;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR59909307 = 71154154;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR49484294 = -96136797;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR74317183 = -804549374;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93327411 = -894076802;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR52549363 = -746030197;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR27105112 = -295007223;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR21027460 = -344780380;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR9790762 = -638983637;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR87789010 = -643087713;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR52250835 = -502727600;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR2874380 = 37354201;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR51779833 = -146456749;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR65273794 = -809836498;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR6724672 = -923547099;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58743811 = -885687843;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR62458569 = -340528753;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR70180052 = -413826357;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR61700880 = -928984200;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR16510794 = -580923630;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34854156 = -151675042;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR86991611 = -317376080;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR32683394 = -122948052;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR60134546 = -824771494;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR44365314 = -101661493;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR26112215 = -117776005;    double SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR23943340 = -364725984;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93471182 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR15980634;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR15980634 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58584816;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58584816 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94927666;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94927666 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34037619;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34037619 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR11297887;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR11297887 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93391797;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93391797 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR96237396;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR96237396 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR6531012;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR6531012 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR82143873;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR82143873 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR99066136;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR99066136 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR97851559;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR97851559 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR35345109;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR35345109 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR84781054;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR84781054 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR67957820;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR67957820 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR97217423;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR97217423 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR9457163;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR9457163 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58036498;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58036498 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR15972179;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR15972179 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR80144121;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR80144121 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR10249315;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR10249315 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR53257913;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR53257913 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR37066741;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR37066741 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR47495829;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR47495829 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR39933153;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR39933153 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR40281999;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR40281999 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR3392691;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR3392691 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR5840378;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR5840378 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR95606922;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR95606922 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71955595;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71955595 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR64516758;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR64516758 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR28063010;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR28063010 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR42775701;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR42775701 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR92173794;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR92173794 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR2671839;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR2671839 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR49093945;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR49093945 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR72789866;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR72789866 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71505308;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71505308 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR28609776;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR28609776 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94896807;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94896807 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR20234814;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR20234814 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR60765020;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR60765020 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR78940729;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR78940729 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43739329;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43739329 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94946466;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR94946466 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR12828041;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR12828041 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR19254540;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR19254540 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93601929;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93601929 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR18051367;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR18051367 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43356088;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43356088 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR69081215;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR69081215 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR12736926;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR12736926 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR62789216;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR62789216 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR36051030;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR36051030 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR33429983;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR33429983 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR40213270;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR40213270 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR78913893;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR78913893 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR11088987;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR11088987 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR54994514;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR54994514 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93755619;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93755619 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR7905196;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR7905196 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR87551419;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR87551419 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR630475;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR630475 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34575416;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34575416 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR17627115;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR17627115 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71003126;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR71003126 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR55075858;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR55075858 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43171315;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR43171315 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR82109216;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR82109216 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR18863876;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR18863876 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR24427557;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR24427557 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR37951855;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR37951855 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR29426723;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR29426723 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR21075371;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR21075371 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR59909307;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR59909307 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR49484294;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR49484294 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR74317183;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR74317183 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93327411;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93327411 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR52549363;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR52549363 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR27105112;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR27105112 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR21027460;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR21027460 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR9790762;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR9790762 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR87789010;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR87789010 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR52250835;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR52250835 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR2874380;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR2874380 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR51779833;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR51779833 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR65273794;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR65273794 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR6724672;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR6724672 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58743811;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR58743811 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR62458569;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR62458569 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR70180052;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR70180052 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR61700880;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR61700880 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR16510794;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR16510794 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34854156;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR34854156 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR86991611;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR86991611 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR32683394;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR32683394 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR60134546;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR60134546 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR44365314;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR44365314 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR26112215;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR26112215 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR23943340;     SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR23943340 = SgsckPsHkMksxidjbhAyZzJcwHSUzuTvZKLR93471182;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void FuiMmJesxYLQBBKCcYojTGVWjcmXotz11082803() {     double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx18770776 = -192926839;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx70224087 = -656887764;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx599594 = -558387842;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx91373755 = 74280772;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx38779713 = -715983072;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx50277478 = -187741835;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx1418733 = -88792623;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51468932 = 7289652;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx22910640 = -16287812;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84381832 = -293300915;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx40247197 = -894873622;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx20454521 = -296214951;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx10842725 = -90799984;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx15438739 = -451501417;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx26074022 = -219574724;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx94078146 = -553680027;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58994513 = -105013030;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx68927112 = -917626255;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx19328831 = -62270360;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28639149 = -675436810;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx79228881 = -24206016;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx44202532 = -375858245;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx12358778 = -584841867;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx26423844 = 43794867;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28252847 = -939989937;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28694604 = -320205711;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx83864982 = -914970094;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx2275128 = 18155620;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx80325534 = -845752055;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx81652970 = -125099374;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx56706962 = -701552217;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx88289938 = -702144895;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx78829362 = -966056271;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx52477499 = -905665975;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx97558269 = -561241753;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx39241936 = -912601314;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx3227208 = -57394674;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx7192715 = -778688092;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93226309 = -583813308;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx85859393 = -96153437;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx24123167 = -831346997;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx11620331 = -211553793;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx79191232 = 50022801;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx18473927 = -379839738;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx34902641 = -852236085;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx14995446 = -808642162;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx89557247 = -489633978;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58027029 = 36291286;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84493975 = -865872070;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx16670660 = -132404107;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25643450 = -520211793;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58537837 = -637127475;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25041224 = -420460452;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93944363 = -325773566;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx31853922 = 36605220;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx74568243 = -817068595;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx57865309 = 27954102;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx74175749 = -502182710;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63120908 = 14270708;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx10085109 = -295777361;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx66412496 = -272771742;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx99143604 = -6948243;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx71143397 = -146958294;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx41257670 = -891188439;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx27674870 = -591748699;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51957259 = -92728727;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx41625159 = -330158681;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58365226 = -185134009;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx17880470 = -890259664;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx86832086 = -306973410;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx90850939 = -396285354;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51801798 = -326324939;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx75700802 = -233812948;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx33469438 = -966116923;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx4515983 = -844089814;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx67608550 = -812652224;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx65011299 = -325881047;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93884850 = -105002129;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx91521202 = -103969048;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx13257401 = -31347775;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx39137357 = -830571733;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25837953 = -851261381;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx17781153 = -115972311;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63654875 = -613347948;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx56009521 = -604887581;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx98169125 = 35575258;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63248714 = -181684443;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84884999 = -540282705;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx20623577 = -842271196;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx22990027 = -744173158;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx81376626 = -840555417;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx29051458 = -555211964;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx44071806 = -692958800;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx83141200 = -188035947;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx19446897 = -823381696;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx24979562 = -724398754;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx40476934 = 35404500;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx37933563 = -58788760;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx90799056 = -788091040;    double yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx82945382 = -192926839;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx18770776 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx70224087;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx70224087 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx599594;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx599594 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx91373755;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx91373755 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx38779713;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx38779713 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx50277478;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx50277478 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx1418733;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx1418733 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51468932;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51468932 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx22910640;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx22910640 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84381832;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84381832 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx40247197;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx40247197 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx20454521;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx20454521 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx10842725;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx10842725 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx15438739;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx15438739 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx26074022;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx26074022 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx94078146;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx94078146 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58994513;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58994513 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx68927112;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx68927112 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx19328831;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx19328831 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28639149;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28639149 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx79228881;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx79228881 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx44202532;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx44202532 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx12358778;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx12358778 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx26423844;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx26423844 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28252847;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28252847 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28694604;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx28694604 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx83864982;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx83864982 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx2275128;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx2275128 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx80325534;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx80325534 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx81652970;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx81652970 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx56706962;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx56706962 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx88289938;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx88289938 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx78829362;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx78829362 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx52477499;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx52477499 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx97558269;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx97558269 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx39241936;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx39241936 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx3227208;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx3227208 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx7192715;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx7192715 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93226309;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93226309 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx85859393;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx85859393 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx24123167;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx24123167 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx11620331;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx11620331 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx79191232;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx79191232 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx18473927;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx18473927 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx34902641;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx34902641 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx14995446;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx14995446 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx89557247;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx89557247 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58027029;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58027029 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84493975;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84493975 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx16670660;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx16670660 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25643450;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25643450 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58537837;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58537837 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25041224;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25041224 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93944363;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93944363 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx31853922;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx31853922 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx74568243;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx74568243 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx57865309;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx57865309 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx74175749;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx74175749 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63120908;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63120908 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx10085109;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx10085109 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx66412496;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx66412496 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx99143604;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx99143604 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx71143397;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx71143397 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx41257670;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx41257670 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx27674870;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx27674870 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51957259;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51957259 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx41625159;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx41625159 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58365226;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx58365226 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx17880470;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx17880470 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx86832086;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx86832086 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx90850939;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx90850939 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51801798;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx51801798 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx75700802;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx75700802 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx33469438;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx33469438 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx4515983;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx4515983 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx67608550;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx67608550 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx65011299;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx65011299 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93884850;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx93884850 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx91521202;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx91521202 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx13257401;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx13257401 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx39137357;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx39137357 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25837953;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx25837953 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx17781153;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx17781153 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63654875;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63654875 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx56009521;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx56009521 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx98169125;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx98169125 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63248714;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx63248714 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84884999;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx84884999 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx20623577;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx20623577 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx22990027;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx22990027 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx81376626;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx81376626 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx29051458;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx29051458 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx44071806;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx44071806 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx83141200;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx83141200 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx19446897;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx19446897 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx24979562;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx24979562 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx40476934;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx40476934 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx37933563;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx37933563 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx90799056;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx90799056 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx82945382;     yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx82945382 = yTYjXuQWhdeYKstlPTqlSMKsyZLcKXHDEYNx18770776;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UeiPfLzpOMosKocTqGRiONHnVFtRWaA55060265() {     int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS83613984 = -388136653;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS65786779 = -976809565;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS83676394 = -344657366;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS36656493 = -658536608;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS42848900 = -605674476;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS74893612 = -34914754;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS4479126 = -535349047;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS11146439 = -554792552;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS39997917 = -17153290;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51153766 = -583574891;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS46836151 = -920705853;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77388036 = -788471141;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28262748 = -391648955;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS81938597 = -929486073;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS15010414 = -921104702;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS48885931 = -487885737;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS49714839 = -731242765;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS45973791 = -447203019;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28133209 = 72852683;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS95254631 = -894749865;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS11492945 = -510233695;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS24130225 = -760138707;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28482904 = -221927024;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS94088587 = -745229198;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5065546 = 53241183;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS57749744 = -387136210;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS30361142 = -321737616;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS48908322 = -164530052;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS52123225 = 96993325;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10046466 = -87440783;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51119896 = -351536196;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS67224524 = -885912087;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS53127022 = -915311851;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS50548081 = -405016902;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS9130688 = -234876850;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28282545 = -165995021;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS41379424 = -577599720;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59822785 = -998941796;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS27560567 = -197867151;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS36469507 = -508471967;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS33932753 = -469528520;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5782452 = 40754143;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS71855023 = -855836848;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10408926 = -239636747;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59805421 = -533177941;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79884955 = -210763487;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79822888 = 61356739;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32384513 = -759963718;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS17769996 = -611819778;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS43354282 = -500806866;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77627076 = -406775787;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS60654220 = 61554632;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32137107 = -511913386;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS50671580 = -991124522;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS43567321 = -607634573;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59483760 = -627997947;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS37303875 = -654882541;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS89587807 = -599428168;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS31590948 = -611777791;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS85099155 = -118538266;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS44532471 = -713177139;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS55570803 = -270818996;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59023214 = -551785878;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS29951451 = -929712508;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS33870 = -132038695;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79611627 = 65206234;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS24261014 = -873159290;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77714667 = -986632054;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS72807910 = -594609223;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS86727868 = -655109682;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS7506507 = -910286017;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS89892054 = -732300969;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS18413225 = -149335869;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS91663702 = -418675350;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS61321879 = -325221346;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5710493 = -450987838;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS52275201 = -904301859;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS18073979 = -982290277;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS34283167 = -112051258;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS25180591 = -735995331;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77926856 = -348492949;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS97976628 = -561773899;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS31138327 = -552710274;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS8768944 = -402199809;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32419389 = -680664996;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS90465675 = -313090829;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS35087418 = -273998702;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS2455443 = -924187330;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS6980761 = -797382329;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS49646928 = -606878903;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS90978670 = -511112480;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51791617 = -978171553;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28231838 = -287164005;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS42461411 = 20671115;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS91937036 = -795294829;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS78361950 = -98709525;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS46759238 = -407459980;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS41903573 = -926124341;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10375056 = -7598053;    int fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS80193794 = -388136653;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS83613984 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS65786779;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS65786779 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS83676394;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS83676394 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS36656493;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS36656493 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS42848900;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS42848900 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS74893612;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS74893612 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS4479126;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS4479126 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS11146439;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS11146439 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS39997917;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS39997917 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51153766;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51153766 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS46836151;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS46836151 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77388036;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77388036 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28262748;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28262748 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS81938597;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS81938597 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS15010414;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS15010414 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS48885931;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS48885931 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS49714839;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS49714839 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS45973791;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS45973791 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28133209;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28133209 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS95254631;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS95254631 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS11492945;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS11492945 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS24130225;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS24130225 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28482904;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28482904 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS94088587;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS94088587 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5065546;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5065546 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS57749744;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS57749744 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS30361142;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS30361142 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS48908322;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS48908322 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS52123225;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS52123225 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10046466;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10046466 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51119896;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51119896 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS67224524;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS67224524 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS53127022;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS53127022 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS50548081;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS50548081 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS9130688;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS9130688 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28282545;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28282545 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS41379424;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS41379424 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59822785;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59822785 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS27560567;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS27560567 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS36469507;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS36469507 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS33932753;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS33932753 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5782452;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5782452 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS71855023;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS71855023 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10408926;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10408926 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59805421;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59805421 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79884955;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79884955 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79822888;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79822888 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32384513;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32384513 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS17769996;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS17769996 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS43354282;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS43354282 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77627076;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77627076 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS60654220;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS60654220 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32137107;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32137107 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS50671580;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS50671580 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS43567321;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS43567321 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59483760;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59483760 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS37303875;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS37303875 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS89587807;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS89587807 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS31590948;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS31590948 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS85099155;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS85099155 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS44532471;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS44532471 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS55570803;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS55570803 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59023214;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS59023214 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS29951451;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS29951451 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS33870;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS33870 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79611627;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS79611627 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS24261014;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS24261014 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77714667;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77714667 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS72807910;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS72807910 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS86727868;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS86727868 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS7506507;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS7506507 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS89892054;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS89892054 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS18413225;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS18413225 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS91663702;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS91663702 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS61321879;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS61321879 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5710493;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS5710493 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS52275201;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS52275201 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS18073979;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS18073979 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS34283167;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS34283167 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS25180591;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS25180591 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77926856;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS77926856 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS97976628;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS97976628 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS31138327;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS31138327 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS8768944;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS8768944 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32419389;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS32419389 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS90465675;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS90465675 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS35087418;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS35087418 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS2455443;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS2455443 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS6980761;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS6980761 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS49646928;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS49646928 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS90978670;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS90978670 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51791617;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS51791617 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28231838;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS28231838 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS42461411;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS42461411 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS91937036;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS91937036 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS78361950;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS78361950 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS46759238;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS46759238 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS41903573;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS41903573 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10375056;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS10375056 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS80193794;     fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS80193794 = fZDrQjjUPcOiKmufAElDklpvGXvtZRiTnMIS83613984;}
// Junk Finished
