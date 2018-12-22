#include "Legitbot.h"
#include "shonaxsettings.h"
CLegitbot legitbot;

void CLegitbot::run(CUserCmd* m_pcmd, bool& send_packet)
{
	auto m_local = game::localdata.localplayer();

	if (m_local && m_local->IsAlive())
	{
		sync();

		if (!active) return;

		IClientEntity* m_target = nullptr;
		bool find_target = true;

		auto m_weapon = m_local->GetWeapon();
		if (m_weapon)
		{
			if (m_weapon->GetAmmoInClip() == 0 || m_weapon->IsKnife()) return;
			sync();
		}
		else return;

		if (!game::functions.can_shoot()) return;

		if (locked && target_id >= 0 && hitbox >= 0)
		{
			m_target = m_pEntityList->GetClientEntity(target_id);
			if (m_target  && viable(m_target))
			{
				sync();
				if (hitbox >= 0)
				{
					Vector ViewOffset = m_target->GetOrigin() + m_target->GetViewOffset();
					Vector View; m_pEngine->GetViewAngles(View);
					View += m_local->localPlayerExclusive()->GetAimPunchAngle() * recoil;
					float nFoV = distance(ViewOffset, View, m_target, hitbox);
					if (nFoV < fov)
						find_target = false;
				}
			}
		}
		if (find_target)
		{
			target_id = 0;
			m_target = nullptr;
			hitbox = -1;
			target_id = get_target();
			if (target_id >= 0) m_target = m_pEntityList->GetClientEntity(target_id);
			else
			{
				m_target = nullptr;
				hitbox = -1;
			}
		}
		sync();
		if (target_id >= 0 && m_target)
		{
			sync();
			if (keypress)
			{
				if (key >= 0 && !GUI.GetKeyState(key))
				{
					target_id = -1;
					m_target = nullptr;
					hitbox = -1;
					return;
				}
			}

			Vector point;
			if (multi_hitbox) point = game::functions.get_hitbox_location(m_target, besthitbox);
			else point = game::functions.get_hitbox_location(m_target, hitbox);
			if (aim(point, m_pcmd))
			{
				if (autofire && !(m_pcmd->buttons & IN_ATTACK))
				{
					m_pcmd->buttons |= IN_ATTACK;
				}
			}
		}
	}
}

bool CLegitbot::viable(IClientEntity* m_player)
{
	auto m_local = game::localdata.localplayer();
	if (!m_player) return false;
	if (m_player->IsDormant()) return false;
	if (!m_player->IsAlive()) return false;
	if (m_player->GetIndex() == m_local->GetIndex()) return false;

	ClientClass *pClientClass = m_player->GetClientClass();
	player_info_t pinfo;

	if (pClientClass->m_ClassID != (int)CSGOClassID::CCSPlayer) return false;
	if (!m_pEngine->GetPlayerInfo(m_player->GetIndex(), &pinfo)) return false;
	if (m_player->GetTeamNum() == m_local->GetTeamNum()) return false;
	if (m_player->m_bGunGameImmunity()) return false;

	return true;
}

void CLegitbot::sync()
{
	auto m_local = game::localdata.localplayer();
	auto m_weapon = m_local->GetWeapon();

	if (!m_weapon) return;

	

	if (m_weapon->m_bIsPistol())
	{
		
		active = Shonax::settingsxd.pistolenabled;
		smoothing = Shonax::settingsxd.pistolfov;
		fov = Shonax::settingsxd.pistolfov;
		recoil = Shonax::settingsxd.pistolrecoil;
		int speeds[] = { 20, 15, 10, 5, 0 };
		delay = speeds[Shonax::settingsxd.reactiontime];
		keypress = Shonax::settingsxd.pistolautofire_mode;
		key = Shonax::settingsxd.pistolhotkey;
		autofire = Shonax::settingsxd.pistolautofire;

		switch (Shonax::settingsxd.pistolbone)
		{
		case 0:
			hitbox = ((int)CSGOHitboxID::Head);
			multi_hitbox = false;
			break;
		case 1:
			hitbox = ((int)CSGOHitboxID::Neck);
			multi_hitbox = false;
			break;
		case 2:
			hitbox = ((int)CSGOHitboxID::Chest);
			multi_hitbox = false;
			break;
		case 3:
			hitbox = ((int)CSGOHitboxID::Stomach);
			multi_hitbox = false;
			break;
		case 4:
			hitbox = ((int)CSGOHitboxID::Pelvis);
			multi_hitbox = false;
			break;
		case 5:
			hitbox = ((int)CSGOHitboxID::LeftLowerArm);
			multi_hitbox = false;
			break;
		case 6:
			hitbox = ((int)CSGOHitboxID::RightLowerArm);
			multi_hitbox = false;
			break;
		case 7:
			hitbox = ((int)CSGOHitboxID::LeftShin);
			multi_hitbox = false;
			break;
		case 8:
			hitbox = ((int)CSGOHitboxID::RightShin);
			multi_hitbox = false;
			break;
		}
	}
	else if (m_weapon->m_bIsSmg())
	{
		
		active = Shonax::settingsxd.smgenabled;
		smoothing = Shonax::settingsxd.smgsmooth;
		fov = Shonax::settingsxd.smgfov;
		recoil = Shonax::settingsxd.smgrecoil;
		int speeds[] = { 20, 15, 10, 5, 0 };
		delay = speeds[Shonax::settingsxd.smgreactiontime];
		keypress = Shonax::settingsxd.smgautofire_mode;
		key = Shonax::settingsxd.smghotkey;
		autofire = Shonax::settingsxd.smgautofire;

		switch (Shonax::settingsxd.smgbone)
		{
		case 0:
			hitbox = ((int)CSGOHitboxID::Head);
			multi_hitbox = false;
			break;
		case 1:
			hitbox = ((int)CSGOHitboxID::Neck);
			multi_hitbox = false;
			break;
		case 2:
			hitbox = ((int)CSGOHitboxID::Chest);
			multi_hitbox = false;
			break;
		case 3:
			hitbox = ((int)CSGOHitboxID::Stomach);
			multi_hitbox = false;
			break;
		case 4:
			hitbox = ((int)CSGOHitboxID::Pelvis);
			multi_hitbox = false;
			break;
		case 5:
			hitbox = ((int)CSGOHitboxID::LeftLowerArm);
			multi_hitbox = false;
			break;
		case 6:
			hitbox = ((int)CSGOHitboxID::RightLowerArm);
			multi_hitbox = false;
			break;
		case 7:
			hitbox = ((int)CSGOHitboxID::LeftShin);
			multi_hitbox = false;
			break;
		case 8:
			hitbox = ((int)CSGOHitboxID::RightShin);
			multi_hitbox = false;
			break;
		}
	}
	else if (m_weapon->m_bIsShotgun())
	{
		
		active = Shonax::settingsxd.shotgunenabled;
		smoothing = Shonax::settingsxd.shotgunsmooth;
		fov = Shonax::settingsxd.shotgunfov;
		recoil = Shonax::settingsxd.shotgunrecoil;
		int speeds[] = { 20, 15, 10, 5, 0 };
		delay = speeds[Shonax::settingsxd.shotgunreactiontime];
		keypress = Shonax::settingsxd.shotgunautofire_mode;
		key = Shonax::settingsxd.shotgunhotkey;
		autofire = Shonax::settingsxd.shotgunautofire;

		switch (Shonax::settingsxd.shotgunbone)
		{
		case 0:
			hitbox = ((int)CSGOHitboxID::Head);
			multi_hitbox = false;
			break;
		case 1:
			hitbox = ((int)CSGOHitboxID::Neck);
			multi_hitbox = false;
			break;
		case 2:
			hitbox = ((int)CSGOHitboxID::Chest);
			multi_hitbox = false;
			break;
		case 3:
			hitbox = ((int)CSGOHitboxID::Stomach);
			multi_hitbox = false;
			break;
		case 4:
			hitbox = ((int)CSGOHitboxID::Pelvis);
			multi_hitbox = false;
			break;
		case 5:
			hitbox = ((int)CSGOHitboxID::LeftLowerArm);
			multi_hitbox = false;
			break;
		case 6:
			hitbox = ((int)CSGOHitboxID::RightLowerArm);
			multi_hitbox = false;
			break;
		case 7:
			hitbox = ((int)CSGOHitboxID::LeftShin);
			multi_hitbox = false;
			break;
		case 8:
			hitbox = ((int)CSGOHitboxID::RightShin);
			multi_hitbox = false;
			break;
		}
	}
	else if (m_weapon->m_bIsSniper())
	{
		
		active = Shonax::settingsxd.sniperenabled;
		smoothing = Shonax::settingsxd.snipersmooth;
		fov = Shonax::settingsxd.sniperfov;
		recoil = Shonax::settingsxd.sniperrecoil;
		int speeds[] = { 20, 15, 10, 5, 0 };
		delay = speeds[Shonax::settingsxd.sniperreactiontime];
		keypress = Shonax::settingsxd.sniperautofire_mode;
		key = Shonax::settingsxd.sniperhotkey;
		autofire = Shonax::settingsxd.sniperautofire;

		switch (Shonax::settingsxd.sniperbone)
		{
		case 0:
			hitbox = ((int)CSGOHitboxID::Head);
			multi_hitbox = false;
			break;
		case 1:
			hitbox = ((int)CSGOHitboxID::Neck);
			multi_hitbox = false;
			break;
		case 2:
			hitbox = ((int)CSGOHitboxID::Chest);
			multi_hitbox = false;
			break;
		case 3:
			hitbox = ((int)CSGOHitboxID::Stomach);
			multi_hitbox = false;
			break;
		case 4:
			hitbox = ((int)CSGOHitboxID::Pelvis);
			multi_hitbox = false;
			break;
		case 5:
			hitbox = ((int)CSGOHitboxID::LeftLowerArm);
			multi_hitbox = false;
			break;
		case 6:
			hitbox = ((int)CSGOHitboxID::RightLowerArm);
			multi_hitbox = false;
			break;
		case 7:
			hitbox = ((int)CSGOHitboxID::LeftShin);
			multi_hitbox = false;
			break;
		case 8:
			hitbox = ((int)CSGOHitboxID::RightShin);
			multi_hitbox = false;
			break;
		}
	}
	else
	{
		
		active = Shonax::settingsxd.rifleenabled;
		smoothing = Shonax::settingsxd.riflesmooth;
		fov = Shonax::settingsxd.riflefov;
		recoil = Shonax::settingsxd.riflerecoil;
		int speeds[] = { 20, 15, 10, 5, 0 };
		delay = speeds[Shonax::settingsxd.riflereactiontime];
		keypress = Shonax::settingsxd.rifleautofire_mode;
		key = Shonax::settingsxd.riflehotkey;
		autofire = Shonax::settingsxd.rifleautofire;

		switch (Shonax::settingsxd.riflebone)
		{
		case 0:
			hitbox = ((int)CSGOHitboxID::Head);
			multi_hitbox = false;
			break;
		case 1:
			hitbox = ((int)CSGOHitboxID::Neck);
			multi_hitbox = false;
			break;
		case 2:
			hitbox = ((int)CSGOHitboxID::Chest);
			multi_hitbox = false;
			break;
		case 3:
			hitbox = ((int)CSGOHitboxID::Stomach);
			multi_hitbox = false;
			break;
		case 4:
			hitbox = ((int)CSGOHitboxID::Pelvis);
			multi_hitbox = false;
			break;
		case 5:
			hitbox = ((int)CSGOHitboxID::LeftLowerArm);
			multi_hitbox = false;
			break;
		case 6:
			hitbox = ((int)CSGOHitboxID::RightLowerArm);
			multi_hitbox = false;
			break;
		case 7:
			hitbox = ((int)CSGOHitboxID::LeftShin);
			multi_hitbox = false;
			break;
		case 8:
			hitbox = ((int)CSGOHitboxID::RightShin);
			multi_hitbox = false;
			break;
		}
	}
}

float distance_3d(Vector me, Vector ent)
{
	return sqrt(pow(double(ent.x - me.x), 2.0) + pow(double(ent.y - me.y), 2.0) + pow(double(ent.z - me.z), 2.0));
}

float CLegitbot::distance(Vector offset, Vector view, IClientEntity* m_player, int hitbox)
{
	CONST FLOAT MaxDegrees = 180.0f;
	Vector Angles = view;
	Vector Origin = offset;
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);
	game::math.angle_vectors(Angles, &Forward);
	Vector AimPos = game::functions.get_hitbox_location(m_player, hitbox);
	VectorSubtract(AimPos, Origin, Delta);
	game::math.normalize(Delta, Delta);
	FLOAT DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / PI));
}

bool CLegitbot::aim(Vector point, CUserCmd *pCmd)
{
	auto m_local = game::localdata.localplayer();
	if (point.Length() == 0) return false;
	Vector angles;
	if (!game::functions.visible(m_local, m_pEntityList->GetClientEntity(target_id), hitbox)) return false;
	Vector src = m_local->GetOrigin() + m_local->GetViewOffset();
	game::math.calculate_angle(src, point, angles);
	game::math.normalize_vector(angles);
	if (recoil)
	{
		Vector AimPunch = m_local->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			angles -= AimPunch * 2;
			game::math.normalize_vector(angles);
		}
	}
	locked = true;
	Vector delta = angles - pCmd->viewangles;
	bool return_data = false;
	if (delta.Length() > smoothing)
	{
		game::math.normalize(delta, delta);
		delta *= smoothing;
	}
	else return_data = true;
	pCmd->viewangles += delta;
	m_pEngine->SetViewAngles(pCmd->viewangles);
	return return_data;
}
/*void CLegitbot::AutoPistol(CUserCmd *pCmd, IClientEntity *pLocal)
{
	auto m_local = game::localdata.localplayer();
	auto m_weapon = m_local->GetWeapon();

	if (!m_weapon) return;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)m_pEntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (m_weapon->m_bIsPistol() && legitconfig.pistol.bAutoPistol)
	{
		static bool WasFiring = false;
		if (m_weapon->m_bIsPistol() && legitconfig.pistol.bAutoPistol)
		{
			if (pCmd->buttons & IN_ATTACK)
			{
				static bool WasFiring = false;
				WasFiring = !WasFiring;

				if (WasFiring)
				{
					pCmd->buttons &= ~IN_ATTACK;
				}
			}
		}
	}
}*/
int CLegitbot::get_target()
{
	sync();
	int target = -1;
	float minFoV = fov;

	IClientEntity* m_local = game::localdata.localplayer();
	Vector offset = m_local->GetOrigin() + m_local->GetViewOffset();
	Vector view; m_pEngine->GetViewAngles(view);
	view += m_local->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	for (int i = 1; i < m_pGlobals->maxClients; i++)
	{
		IClientEntity *m_entity = m_pEntityList->GetClientEntity(i);
		if (viable(m_entity))
		{
			if (hitbox >= 0)
			{
				float dist = distance(offset, view, m_entity, 0);
				if (!game::functions.visible(m_local, m_entity, hitbox)) continue;
				if (dist < minFoV)
				{
					minFoV = dist;
					target = i;
				}
			}
		}
	}

	return target;
}

// Junk Code By Troll Face & Thaisen's Gen
void pRCnQvweiARLZbwYasKQTpwPRIsNjdg82162237() {     double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai87646230 = 22867801;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai76188727 = -172564158;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32093818 = -883272470;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32048866 = -988189400;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai20901939 = 74493650;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai63104936 = -218464193;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai61649404 = -817152687;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai37042045 = -379215717;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68098015 = -223139583;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai61195122 = -228673108;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91787733 = -128930703;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai64451534 = -84706804;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai18366239 = 75972889;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai8695190 = -32064412;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai25195971 = -515776690;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92007802 = -518198707;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai10029167 = -52374085;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai48208102 = -471704781;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91822135 = -999529340;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40186632 = 18564990;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai74056396 = -545494638;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai98999110 = -562226981;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93069313 = -496369828;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai42639376 = -267104112;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai34374258 = -956572207;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40062779 = -77396685;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36626948 = 22322221;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai24793777 = -592325438;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai39954821 = -872246071;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai70875204 = -591937869;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68717961 = 89013817;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai22691535 = -811441246;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32745721 = 91527056;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai90343759 = -892922996;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai65357706 = -816382707;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai52183702 = -322428395;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai98794607 = -207270958;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai7382696 = -454024613;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai95992969 = -434829407;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai53343065 = -712555552;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69972851 = -84163524;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36071182 = -215231460;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai89540108 = -765869494;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93158371 = -866709794;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai51000651 = 72747141;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai24693795 = -55586031;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai3243920 = -873231069;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai53949078 = -761146624;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai74085960 = -217531408;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai90894435 = -86285447;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai73695181 = -483336457;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68837616 = 528320;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai94914005 = 15019610;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40837455 = -911071686;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai8105841 = -553556391;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai88647120 = -414905218;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai83119414 = -676194331;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai89454442 = -516168359;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97674607 = 68382806;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai80839160 = -848109665;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai26477988 = -140786415;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36855628 = -124827250;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97087223 = -506969647;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97222810 = -631201714;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92477161 = -217686925;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69096199 = -317489457;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai31705813 = -76233861;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai28022479 = -31104115;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai43337484 = -215681705;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai73012269 = -93348296;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93213194 = -210927749;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai2646472 = -598349472;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai52215133 = 63124626;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai38479071 = -186973789;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai70213781 = -897271486;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai37985214 = -230263179;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9459003 = -796357488;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai99910941 = -629660034;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91638725 = -239851253;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9680464 = -800986176;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36818859 = -204165617;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai82677870 = -216531155;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai50707817 = -274794030;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai49060386 = -685960625;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97180023 = -8601413;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai99880344 = -911514504;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai27777530 = -726460857;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91908265 = 2598742;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai82237918 = -239366606;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai76710585 = -301477490;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69064288 = -646234065;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9340166 = -691102600;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9708088 = -422407420;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai15153810 = -586719743;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai26865077 = -471769137;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai33117224 = -959336275;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai38983958 = -708261814;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92317297 = -34667780;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai681210 = -549022870;    double qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai81904452 = 22867801;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai87646230 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai76188727;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai76188727 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32093818;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32093818 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32048866;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32048866 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai20901939;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai20901939 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai63104936;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai63104936 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai61649404;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai61649404 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai37042045;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai37042045 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68098015;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68098015 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai61195122;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai61195122 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91787733;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91787733 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai64451534;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai64451534 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai18366239;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai18366239 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai8695190;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai8695190 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai25195971;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai25195971 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92007802;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92007802 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai10029167;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai10029167 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai48208102;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai48208102 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91822135;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91822135 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40186632;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40186632 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai74056396;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai74056396 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai98999110;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai98999110 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93069313;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93069313 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai42639376;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai42639376 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai34374258;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai34374258 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40062779;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40062779 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36626948;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36626948 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai24793777;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai24793777 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai39954821;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai39954821 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai70875204;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai70875204 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68717961;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68717961 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai22691535;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai22691535 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32745721;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai32745721 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai90343759;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai90343759 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai65357706;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai65357706 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai52183702;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai52183702 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai98794607;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai98794607 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai7382696;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai7382696 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai95992969;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai95992969 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai53343065;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai53343065 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69972851;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69972851 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36071182;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36071182 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai89540108;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai89540108 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93158371;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93158371 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai51000651;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai51000651 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai24693795;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai24693795 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai3243920;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai3243920 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai53949078;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai53949078 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai74085960;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai74085960 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai90894435;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai90894435 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai73695181;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai73695181 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68837616;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai68837616 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai94914005;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai94914005 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40837455;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai40837455 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai8105841;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai8105841 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai88647120;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai88647120 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai83119414;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai83119414 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai89454442;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai89454442 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97674607;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97674607 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai80839160;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai80839160 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai26477988;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai26477988 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36855628;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36855628 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97087223;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97087223 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97222810;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97222810 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92477161;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92477161 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69096199;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69096199 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai31705813;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai31705813 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai28022479;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai28022479 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai43337484;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai43337484 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai73012269;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai73012269 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93213194;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai93213194 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai2646472;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai2646472 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai52215133;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai52215133 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai38479071;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai38479071 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai70213781;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai70213781 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai37985214;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai37985214 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9459003;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9459003 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai99910941;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai99910941 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91638725;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91638725 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9680464;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9680464 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36818859;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai36818859 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai82677870;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai82677870 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai50707817;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai50707817 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai49060386;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai49060386 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97180023;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai97180023 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai99880344;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai99880344 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai27777530;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai27777530 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91908265;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai91908265 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai82237918;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai82237918 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai76710585;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai76710585 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69064288;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai69064288 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9340166;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9340166 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9708088;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai9708088 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai15153810;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai15153810 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai26865077;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai26865077 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai33117224;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai33117224 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai38983958;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai38983958 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92317297;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai92317297 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai681210;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai681210 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai81904452;     qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai81904452 = qdpQjAFqonIeXBcLyueVWcCpAVnVHDjuEUai87646230;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void axdbaEzAdVCVjIyEtcjSnazCxcCeBNn10198597() {     double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF48411263 = -399085813;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF5131943 = -504323878;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10181861 = -276618618;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF15638449 = 94418728;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF73089199 = -586946782;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3501968 = -311544076;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF11786411 = -962546804;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85191090 = -870835180;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3255086 = -851671690;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF81696281 = -473256726;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50829924 = -591196934;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF73330967 = -451015458;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF245586 = -189116979;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38649518 = -521559609;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96334580 = 23926660;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF97845278 = -454153465;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF59549693 = -962133596;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35858375 = -597863513;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF75300848 = -109092252;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10445116 = -195728527;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF76453401 = -679531618;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF15068989 = -46592689;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF2008334 = -188588577;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF71710226 = -69370439;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF2269214 = 70624455;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF22599478 = -967966721;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF7298233 = -733983973;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF26553428 = -175273240;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF58663200 = 79056034;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10016121 = -375598079;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46078718 = -760272882;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF36017625 = -224206352;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF95564969 = -559548144;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF80113053 = -201267304;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF43694552 = -832292996;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF51064347 = -864215845;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF66702398 = -110986827;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF24367827 = -94722323;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF55686468 = -423382922;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF36704290 = -453735060;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF19457144 = -349931475;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF90593958 = -186697907;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF18890629 = -146309650;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF48792409 = 83303159;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF6955232 = -194928013;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF32394560 = 78363567;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF65193998 = -400344612;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF60716909 = -257455885;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF44762845 = -866762447;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF68693173 = -340436;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF97768768 = -661400637;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF83576743 = -697409732;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50338814 = -508739443;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF4472364 = -559348167;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF33745196 = -306896132;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF33342275 = -252493124;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3123610 = -215735301;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38471634 = -107248180;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF13369235 = -976205727;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50489722 = -618980061;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96203735 = -577560103;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85232982 = -687273564;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF26527890 = -849891215;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF93238965 = -376073611;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35617563 = -712983844;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF14812300 = -266990582;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF77765997 = -891467314;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF20132533 = -987849676;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF94954965 = -689266613;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF45270233 = -111857495;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF31142881 = -243166639;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35181867 = -767411274;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF80171906 = -74480591;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38596558 = -655357193;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF90987971 = -845797053;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85859443 = -392833712;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96178359 = -900283039;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF53215924 = -171891737;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF64754995 = -874442426;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF69874654 = 92260888;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF57405480 = -467622110;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46581323 = -376528089;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF81790582 = -308510794;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF89970027 = -920603531;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF12247352 = -714197442;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF62501975 = 37136849;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85678810 = -715466909;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF91092605 = 99800022;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46367858 = -894371172;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10352278 = -479799873;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF47940737 = -548480545;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF28230765 = 96261353;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10998592 = -118516596;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF5196747 = -804402862;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF40500555 = -876174957;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF34224161 = -662657912;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF64066069 = -336806693;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF25651664 = -770236039;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF13174846 = -203712998;    double UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF92142931 = -399085813;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF48411263 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF5131943;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF5131943 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10181861;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10181861 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF15638449;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF15638449 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF73089199;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF73089199 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3501968;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3501968 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF11786411;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF11786411 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85191090;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85191090 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3255086;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3255086 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF81696281;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF81696281 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50829924;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50829924 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF73330967;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF73330967 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF245586;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF245586 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38649518;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38649518 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96334580;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96334580 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF97845278;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF97845278 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF59549693;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF59549693 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35858375;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35858375 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF75300848;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF75300848 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10445116;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10445116 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF76453401;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF76453401 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF15068989;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF15068989 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF2008334;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF2008334 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF71710226;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF71710226 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF2269214;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF2269214 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF22599478;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF22599478 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF7298233;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF7298233 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF26553428;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF26553428 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF58663200;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF58663200 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10016121;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10016121 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46078718;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46078718 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF36017625;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF36017625 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF95564969;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF95564969 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF80113053;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF80113053 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF43694552;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF43694552 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF51064347;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF51064347 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF66702398;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF66702398 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF24367827;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF24367827 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF55686468;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF55686468 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF36704290;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF36704290 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF19457144;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF19457144 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF90593958;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF90593958 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF18890629;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF18890629 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF48792409;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF48792409 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF6955232;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF6955232 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF32394560;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF32394560 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF65193998;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF65193998 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF60716909;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF60716909 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF44762845;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF44762845 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF68693173;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF68693173 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF97768768;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF97768768 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF83576743;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF83576743 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50338814;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50338814 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF4472364;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF4472364 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF33745196;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF33745196 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF33342275;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF33342275 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3123610;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF3123610 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38471634;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38471634 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF13369235;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF13369235 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50489722;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF50489722 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96203735;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96203735 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85232982;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85232982 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF26527890;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF26527890 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF93238965;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF93238965 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35617563;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35617563 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF14812300;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF14812300 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF77765997;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF77765997 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF20132533;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF20132533 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF94954965;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF94954965 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF45270233;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF45270233 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF31142881;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF31142881 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35181867;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF35181867 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF80171906;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF80171906 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38596558;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF38596558 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF90987971;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF90987971 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85859443;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85859443 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96178359;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF96178359 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF53215924;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF53215924 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF64754995;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF64754995 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF69874654;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF69874654 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF57405480;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF57405480 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46581323;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46581323 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF81790582;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF81790582 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF89970027;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF89970027 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF12247352;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF12247352 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF62501975;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF62501975 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85678810;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF85678810 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF91092605;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF91092605 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46367858;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF46367858 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10352278;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10352278 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF47940737;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF47940737 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF28230765;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF28230765 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10998592;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF10998592 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF5196747;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF5196747 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF40500555;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF40500555 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF34224161;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF34224161 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF64066069;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF64066069 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF25651664;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF25651664 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF13174846;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF13174846 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF92142931;     UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF92142931 = UQlyaBCnpuxDxWWyiMKeEYdCAyadxCUyPhiF48411263;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void osrwAvqifXFMhXZzAIpnWGkCgrcUNGV77602102() {     double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ73710856 = -227286666;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ59375396 = -998127110;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ52196638 = 38573695;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ12084538 = 77637095;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77831294 = -511143682;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ42481560 = -941708875;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19813346 = -967751653;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ40422625 = -322397804;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19634715 = -705518458;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ83934240 = -689960217;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ92010985 = -110927506;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95933928 = -688681693;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ75743201 = 76147184;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ69307202 = -279844448;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54450782 = 4342192;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ94706001 = 12907733;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ9087043 = -64533124;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ46748989 = 52334206;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ78657499 = -620074009;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ58940143 = -595485430;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ45432968 = -866760278;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ6013609 = 45686015;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77300370 = -702132985;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ50638241 = -341732684;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ90588908 = -404887526;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11012083 = -978165883;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ87770523 = -884805740;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ22988178 = -197415850;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ43381812 = -141617388;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19713496 = -900182491;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38268922 = -344672328;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ96244553 = -255821808;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ31618630 = -164971586;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ40416758 = -421599530;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38580984 = -56416963;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ41212337 = -171203755;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97139739 = -249660033;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ60055233 = -848027127;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ20303003 = -373741589;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ27666877 = -339083708;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ23345496 = -934444410;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ41449269 = -757411166;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19141132 = -532699276;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ23527008 = -119315922;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ46911406 = -9351408;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ34561965 = -660807892;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35496706 = 75247583;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ25142009 = -195999910;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11205454 = -316020491;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ42007746 = -110393490;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54331003 = -543743267;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ29377655 = -463841189;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ12590823 = -68506948;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ62365696 = -548035999;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ32169134 = -570645011;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ67697248 = -172972681;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ82075026 = -195994125;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ1558397 = -619693622;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ21495630 = -517475380;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ66819212 = -532977799;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54711036 = 43096865;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ96825167 = -670335803;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97040812 = -80780416;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ99921219 = -805335967;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ45665319 = -245287890;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95766432 = -855105699;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ64315298 = -423710107;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35326444 = -502253287;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ30726219 = -123427485;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ13238445 = -824454054;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97566262 = -737432235;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ49031809 = -216505998;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ26445987 = -573924206;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ50990623 = -180990302;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35594647 = -661041020;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ3983700 = -9349113;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ86872476 = -421614710;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ53773362 = -482817064;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ3726835 = -232381277;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ56026943 = -744079634;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ75515376 = -953413467;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ62628515 = -588805831;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11782725 = -881395360;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ1374067 = 68776101;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ65382492 = -256439225;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ8891268 = -880831139;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ83653730 = -87314861;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ69252933 = -616935587;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ8247624 = -850954520;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ70883735 = -883444282;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ59137310 = -975209630;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95581342 = -629966411;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38559604 = -230551747;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ53483791 = -840763790;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ72955840 = -282180573;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ26520328 = -164108607;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ44408456 = -576630750;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19219913 = -727363310;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77861688 = -874028033;    double kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ51144974 = -227286666;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ73710856 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ59375396;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ59375396 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ52196638;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ52196638 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ12084538;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ12084538 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77831294;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77831294 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ42481560;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ42481560 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19813346;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19813346 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ40422625;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ40422625 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19634715;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19634715 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ83934240;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ83934240 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ92010985;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ92010985 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95933928;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95933928 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ75743201;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ75743201 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ69307202;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ69307202 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54450782;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54450782 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ94706001;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ94706001 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ9087043;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ9087043 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ46748989;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ46748989 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ78657499;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ78657499 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ58940143;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ58940143 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ45432968;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ45432968 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ6013609;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ6013609 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77300370;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77300370 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ50638241;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ50638241 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ90588908;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ90588908 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11012083;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11012083 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ87770523;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ87770523 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ22988178;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ22988178 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ43381812;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ43381812 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19713496;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19713496 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38268922;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38268922 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ96244553;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ96244553 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ31618630;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ31618630 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ40416758;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ40416758 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38580984;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38580984 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ41212337;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ41212337 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97139739;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97139739 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ60055233;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ60055233 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ20303003;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ20303003 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ27666877;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ27666877 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ23345496;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ23345496 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ41449269;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ41449269 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19141132;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19141132 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ23527008;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ23527008 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ46911406;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ46911406 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ34561965;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ34561965 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35496706;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35496706 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ25142009;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ25142009 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11205454;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11205454 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ42007746;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ42007746 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54331003;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54331003 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ29377655;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ29377655 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ12590823;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ12590823 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ62365696;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ62365696 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ32169134;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ32169134 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ67697248;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ67697248 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ82075026;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ82075026 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ1558397;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ1558397 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ21495630;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ21495630 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ66819212;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ66819212 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54711036;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ54711036 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ96825167;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ96825167 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97040812;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97040812 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ99921219;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ99921219 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ45665319;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ45665319 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95766432;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95766432 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ64315298;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ64315298 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35326444;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35326444 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ30726219;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ30726219 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ13238445;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ13238445 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97566262;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ97566262 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ49031809;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ49031809 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ26445987;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ26445987 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ50990623;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ50990623 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35594647;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ35594647 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ3983700;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ3983700 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ86872476;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ86872476 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ53773362;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ53773362 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ3726835;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ3726835 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ56026943;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ56026943 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ75515376;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ75515376 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ62628515;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ62628515 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11782725;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ11782725 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ1374067;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ1374067 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ65382492;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ65382492 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ8891268;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ8891268 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ83653730;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ83653730 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ69252933;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ69252933 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ8247624;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ8247624 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ70883735;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ70883735 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ59137310;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ59137310 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95581342;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ95581342 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38559604;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ38559604 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ53483791;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ53483791 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ72955840;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ72955840 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ26520328;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ26520328 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ44408456;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ44408456 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19219913;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ19219913 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77861688;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ77861688 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ51144974;     kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ51144974 = kDQUWeTHIZubWkldxRuYafwciIoQJSWVYBVJ73710856;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZNwwcZlqlGrnIqyGbKSRkPVOPhhWuIx21579564() {     int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn38554066 = -422496482;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54938088 = -218048917;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35273439 = -847695829;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn57367275 = -655180282;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn81900481 = -400835094;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67097694 = -788881795;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn22873739 = -314308077;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn100132 = -884480024;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn36721991 = -706383937;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50706174 = -980234193;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn98599938 = -136759738;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn52867444 = -80937892;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn93163224 = -224701788;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35807061 = -757829105;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn43387174 = -697187805;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn49513786 = 78702024;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99807369 = -690762859;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23795669 = -577242562;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn87461878 = -484950966;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25555626 = -814798485;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn77697031 = -252787962;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn85941300 = -338594448;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn93424496 = -339218143;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18302985 = -30756749;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67401606 = -511656419;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn40067224 = 54903621;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn34266684 = -291573263;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn69621372 = -380101528;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn15179503 = -298871993;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn48106991 = -862523900;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn32681856 = 5343693;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn75179138 = -439588997;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn5916290 = -114227164;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn38487340 = 79049544;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50153402 = -830052057;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn30252947 = -524597442;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35291956 = -769865079;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn12685304 = 31719165;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54637260 = 12204580;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn78276990 = -751402238;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn33155083 = -572625933;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35611390 = -505103210;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn11804923 = -338558924;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn15462007 = 20887069;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn71814186 = -790293262;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99451473 = -62929199;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25762347 = -473761700;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99499493 = -992254913;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn44481474 = -61968175;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn68691367 = -478796255;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn6314630 = -430307261;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn31494038 = -865159077;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn19686705 = -159959885;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn19092913 = -113386955;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn43882533 = -114884798;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn52612765 = 16097965;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn61513591 = -878830775;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn16970455 = -716939080;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn89965668 = -43523864;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn41833258 = -355738716;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn32831011 = -397308532;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn53252366 = -934206549;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn84920629 = -485608032;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn88615000 = -843860037;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18024319 = -885577886;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23420801 = -697170742;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46951154 = -966710729;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54675885 = -203751332;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn85653659 = -927777048;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn13134227 = -72590363;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn14221831 = -151432898;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn87122065 = -622482025;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn69158408 = -489447143;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn9184889 = -733548729;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn92400543 = -142172552;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn42085641 = -747684753;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn74136377 = 99964476;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn77962490 = -260105212;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46488799 = -240463488;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67950133 = -348727221;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn14304877 = -471334679;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn34767190 = -299318351;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25139899 = -218133354;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46488135 = -820075739;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn41792361 = -332216640;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn1187818 = -129497231;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn55492433 = -179629112;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn86823376 = 99159791;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn94604807 = -806065659;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn97540636 = -746150023;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn68739355 = -645766668;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18321501 = 47074000;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn22719635 = -924756971;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn12804002 = -632056704;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn45445979 = -254093706;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn79902716 = -638419384;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50690760 = 80504821;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23189923 = -494698887;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn97437687 = -93535046;    int EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn48393386 = -422496482;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn38554066 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54938088;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54938088 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35273439;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35273439 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn57367275;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn57367275 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn81900481;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn81900481 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67097694;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67097694 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn22873739;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn22873739 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn100132;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn100132 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn36721991;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn36721991 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50706174;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50706174 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn98599938;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn98599938 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn52867444;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn52867444 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn93163224;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn93163224 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35807061;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35807061 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn43387174;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn43387174 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn49513786;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn49513786 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99807369;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99807369 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23795669;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23795669 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn87461878;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn87461878 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25555626;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25555626 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn77697031;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn77697031 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn85941300;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn85941300 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn93424496;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn93424496 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18302985;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18302985 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67401606;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67401606 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn40067224;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn40067224 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn34266684;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn34266684 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn69621372;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn69621372 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn15179503;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn15179503 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn48106991;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn48106991 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn32681856;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn32681856 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn75179138;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn75179138 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn5916290;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn5916290 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn38487340;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn38487340 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50153402;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50153402 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn30252947;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn30252947 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35291956;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35291956 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn12685304;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn12685304 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54637260;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54637260 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn78276990;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn78276990 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn33155083;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn33155083 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35611390;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn35611390 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn11804923;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn11804923 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn15462007;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn15462007 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn71814186;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn71814186 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99451473;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99451473 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25762347;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25762347 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99499493;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn99499493 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn44481474;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn44481474 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn68691367;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn68691367 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn6314630;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn6314630 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn31494038;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn31494038 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn19686705;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn19686705 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn19092913;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn19092913 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn43882533;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn43882533 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn52612765;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn52612765 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn61513591;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn61513591 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn16970455;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn16970455 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn89965668;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn89965668 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn41833258;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn41833258 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn32831011;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn32831011 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn53252366;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn53252366 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn84920629;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn84920629 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn88615000;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn88615000 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18024319;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18024319 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23420801;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23420801 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46951154;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46951154 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54675885;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn54675885 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn85653659;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn85653659 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn13134227;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn13134227 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn14221831;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn14221831 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn87122065;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn87122065 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn69158408;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn69158408 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn9184889;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn9184889 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn92400543;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn92400543 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn42085641;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn42085641 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn74136377;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn74136377 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn77962490;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn77962490 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46488799;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46488799 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67950133;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn67950133 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn14304877;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn14304877 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn34767190;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn34767190 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25139899;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn25139899 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46488135;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn46488135 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn41792361;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn41792361 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn1187818;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn1187818 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn55492433;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn55492433 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn86823376;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn86823376 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn94604807;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn94604807 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn97540636;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn97540636 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn68739355;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn68739355 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18321501;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn18321501 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn22719635;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn22719635 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn12804002;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn12804002 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn45445979;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn45445979 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn79902716;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn79902716 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50690760;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn50690760 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23189923;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn23189923 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn97437687;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn97437687 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn48393386;     EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn48393386 = EzlpENjhBnDNqwTHPEKKuoMlJrigurPvdInn38554066;}
// Junk Finished
