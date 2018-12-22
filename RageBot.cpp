
#include "RageBot.h"
#include "DrawManager.h"
#include "Autowall.h"
#include <iostream>
#include "Resolver.h"
#include "shonaxsettings.h"
#define M_PI_F ((float)(M_PI))

CRagebot ragebot;



void CRagebot::run(CUserCmd *pCmd, bool& bSendPacket)
{
	if (m_pEngine->IsConnected() && m_pEngine->IsInGame())
	{
		auto m_local = game::localdata.localplayer();
		if (!m_local || !m_local->IsAlive()) return;

		auto m_weapon = m_local->GetWeapon();
		if (m_weapon)
		{
			aa.Manage(pCmd, bSendPacket);

			if (m_weapon->GetAmmoInClip())
			{
				
				if (Shonax::settingsxd.aimbot_enabled)
				{
					if (m_local->IsAlive())
					{
						aimbot.aimbotted_in_current_tick = false;
						if (game::functions.can_shoot()) {
							aimbot.DoAimbot(pCmd, bSendPacket);
						}
						if (!aimbot.aimbotted_in_current_tick) aimbot.auto_revolver(pCmd);

						RemoveRecoil(pCmd);
					}
				}
			}
			else
			{
				pCmd->buttons &= ~IN_ATTACK;
				pCmd->buttons |= IN_RELOAD;
			}

			if (game::globals.Target)
			{
				if (Shonax::settingsxd.autostop)
				{
					if (m_local->GetVelocity().Length() > 0)
					{
						if (!(Shonax::settingsxd.autostop_mode))
						{
							pCmd->forwardmove = 0.f;
							pCmd->sidemove = 0.f;
						}
						else if (Shonax::settingsxd.autostop_mode == 1)
						{
							Vector velocity = m_local->GetVelocity();
							Vector direction = velocity.Angle();
							float speed = velocity.Length();

							direction.y = pCmd->viewangles.y - direction.y;

							Vector negated_direction = direction.Forward() * -speed;

							pCmd->forwardmove = negated_direction.x;
							pCmd->sidemove = negated_direction.y;
						}
					}

					if ((pCmd->buttons & IN_FORWARD))
						pCmd->buttons &= ~IN_FORWARD;

					if ((pCmd->buttons & IN_LEFT))
						pCmd->buttons &= ~IN_LEFT;

					if ((pCmd->buttons & IN_RIGHT))
						pCmd->buttons &= ~IN_RIGHT;

					if ((pCmd->buttons & IN_BACK))
						pCmd->buttons &= ~IN_BACK;

					if ((pCmd->buttons & IN_JUMP))
						pCmd->buttons &= ~IN_JUMP;
				}
			}
		}
		
		if (Shonax::settingsxd.antiuntrusted)
			game::math.normalize_vector(pCmd->viewangles);
	}
}
bool CanWallbang(Vector& EndPos, float &Damage)
{

	if (CanHit(EndPos, &Damage))
	{
		return true;
	}
	return false;
}
Vector GetBestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* m_local = game::localdata.localplayer();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	m_pTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;
}
void VectorAngles1(const Vector& forward, Vector &angles) {
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float hitchance() {
	auto m_local = game::localdata.localplayer();
	auto m_weapon = m_local->GetWeapon();

	float hitchance = 101;
	if (!m_weapon) return 0;
	
	if (Shonax::settingsxd.hitchance && Shonax::settingsxd.hitchancescale > 1)
	{
		float inaccuracy = m_weapon->GetCone();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;
	}
	return hitchance;
}

void CRagebot::RemoveRecoil(CUserCmd* pCmd)
{
	
	if (!Shonax::settingsxd.norecoil) return;

	IClientEntity* m_local = game::localdata.localplayer();
	if (m_local)
	{
		Vector AimPunch = m_local->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			auto weapon_recoil_scale = m_pCVar->FindVar(XorStr("weapon_recoil_scale"));
			*(int*)((DWORD)&weapon_recoil_scale->fnChangeCallback + 0xC) = 0;
			pCmd->viewangles -= AimPunch * weapon_recoil_scale->GetFloat();
			game::math.normalize_vector(pCmd->viewangles);
		}
	}
}

int Aimbot::GetTargetFOV()
{
	auto m_local = game::localdata.localplayer();

	int target = -1;

	float mfov = Shonax::settingsxd.aimbotfov;

	Vector viewoffset = m_local->GetOrigin() + m_local->GetViewOffset();
	Vector view; m_pEngine->GetViewAngles(view);

	for (int i = 0; i < m_pEntityList->GetMaxEntities(); i++)
	{
		IClientEntity* pEntity = m_pEntityList->GetClientEntity(i);

		if (IsViable(pEntity))
		{
			int newhb = HitScan(pEntity);
			if (newhb >= 0)
			{
				float fov = FovToPlayer(viewoffset, view, pEntity, 0);
				CPlayer* Player = plist.get_player(i);
				if (fov < mfov || (Player->Priority && fov <= 180))
				{
					mfov = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int Aimbot::GetTargetDistance()
{
	auto m_local = game::localdata.localplayer();

	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = m_local;
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; m_pEngine->GetViewAngles(View);

	for (int i = 0; i < m_pEntityList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = m_pEntityList->GetClientEntity(i);
		if (IsViable(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				CPlayer* Player = plist.get_player(i);
				if ((Distance < minDist || Player->Priority) && fov < Shonax::settingsxd.aimbotfov)
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

int Aimbot::GetTargetHealth()
{
	auto m_local = game::localdata.localplayer();

	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = m_local;
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; m_pEngine->GetViewAngles(View);

	for (int i = 0; i < m_pEntityList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = m_pEntityList->GetClientEntity(i);
		if (IsViable(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				CPlayer* Player = plist.get_player(i);
				if ((Health < minHealth || Player->Priority) && fov < Shonax::settingsxd.aimbotfov)
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

bool Aimbot::IsViable(IClientEntity* pEntity)
{
	auto m_local = game::localdata.localplayer();
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != m_local->GetIndex())
	{
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && m_pEngine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			
			if (pEntity->GetTeamNum() != m_local->GetTeamNum() || Shonax::settingsxd.friendlyfire)
			{
				if (!pEntity->m_bGunGameImmunity())
				{
					CPlayer* Player = plist.get_player(pEntity->GetIndex());
					if (!Player->Friendly)
						return true;
				}
			}
		}
	}

	return false;
}

void Aimbot::auto_revolver(CUserCmd* m_pcmd) {
	
	if (Shonax::settingsxd.revolver_mode == 1) {
		auto m_local = game::localdata.localplayer();
		auto m_weapon = m_local->GetWeapon();

		if (m_weapon) {
			if (*m_weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER) {
				m_pcmd->buttons |= IN_ATTACK;
				float flPostponeFireReady = m_weapon->GetFireReadyTime();
				if (flPostponeFireReady > 0 && flPostponeFireReady - 1 < m_pGlobals->curtime) {
					m_pcmd->buttons &= ~IN_ATTACK;
				}
			}
		}
	}
}

float Aimbot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	CONST FLOAT MaxDegrees = 180.0f;
	Vector Angles = View;
	Vector Origin = ViewOffSet;
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);
	game::math.angle_vectors(Angles, Forward);
	Vector AimPos = game::functions.get_hitbox_location(pEntity, aHitBox);
	game::math.vector_subtract(AimPos, Origin, Delta);
	game::math.normalize(Delta, Delta);
	FLOAT DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int Aimbot::HitScan(IClientEntity* m_entity)
{
	IClientEntity* m_local = game::localdata.localplayer();
	CPlayer* m_player = plist.get_player(m_entity->GetIndex());
	std::vector<int> hitboxes;
	std::vector<int> baim_hitboxes;
	
	bool AWall = Shonax::settingsxd.autowall;
	int hbox = Shonax::settingsxd.autofire_bone;
	
	bool bRevert = true;
	
	for (int i = 0; i < hitscanconfig.hitboxes.size(); i++)
	{
		if (!Shonax::settingsxd.multiHitboxes[i])
			bRevert = false;
	}

	std::vector<MultiBoxItem> custom_hitbones = hitscanconfig.hitboxes;

	baim_hitboxes.push_back((int)CSGOHitboxID::UpperChest);
	baim_hitboxes.push_back((int)CSGOHitboxID::Chest);
	baim_hitboxes.push_back((int)CSGOHitboxID::LowerChest);
	baim_hitboxes.push_back((int)CSGOHitboxID::Stomach);
	baim_hitboxes.push_back((int)CSGOHitboxID::Pelvis);

	bool canseebody = false;
	float bodydmg;

	for (auto HitBoxID : baim_hitboxes)
	{
		Vector Point = game::functions.get_hitbox_location(m_entity, HitBoxID);
		if (CanHit(Point, &bodydmg))
		{
			canseebody = true;
		}
	}

	if (bRevert)
	{
		switch (hbox)
		{
		case 0: hitboxes.push_back((int)CSGOHitboxID::Head);
		case 1: hitboxes.push_back((int)CSGOHitboxID::Neck);
		case 2: hitboxes.push_back((int)CSGOHitboxID::Chest);
		case 3: hitboxes.push_back((int)CSGOHitboxID::Stomach);
		case 4: hitboxes.push_back((int)CSGOHitboxID::Pelvis);
		case 5: hitboxes.push_back((int)CSGOHitboxID::LeftLowerArm);
		case 6: hitboxes.push_back((int)CSGOHitboxID::RightLowerArm);
		case 7: hitboxes.push_back((int)CSGOHitboxID::LeftShin);
		case 8: hitboxes.push_back((int)CSGOHitboxID::RightShin);
		}
	}
	else
	{
		if (custom_hitbones[0].bselected)
			hitboxes.push_back((int)CSGOHitboxID::Head);

		if (custom_hitbones[1].bselected)
			hitboxes.push_back((int)CSGOHitboxID::Neck);

		if (custom_hitbones[2].bselected)
			hitboxes.push_back((int)CSGOHitboxID::Chest);

		if (custom_hitbones[3].bselected)
			hitboxes.push_back((int)CSGOHitboxID::Stomach);

		if (custom_hitbones[4].bselected)
			hitboxes.push_back((int)CSGOHitboxID::Pelvis);

		if (custom_hitbones[5].bselected)
			hitboxes.push_back((int)CSGOHitboxID::LeftLowerArm);

		if (custom_hitbones[6].bselected)
			hitboxes.push_back((int)CSGOHitboxID::RightLowerArm);

		if (custom_hitbones[7].bselected)
			hitboxes.push_back((int)CSGOHitboxID::LeftShin);

		if (custom_hitbones[8].bselected)
			hitboxes.push_back((int)CSGOHitboxID::RightShin);
	}

	for (auto HitBoxID : hitboxes)
	{
		
		if ((HitBoxID == 0 || HitBoxID == 1 || HitBoxID == 2) && canseebody) {
			if (Shonax::settingsxd.preferbody)
				if (m_entity->GetVelocity().Length2D() < 5 || !(m_entity->GetFlags() & FL_ONGROUND))
					continue;
			if (Shonax::settingsxd.bodyaimiawp && Shonax::settingsxd.bodyaim_mode && *m_local->GetWeapon()->GetItemDefinitionIndex() == WEAPON_AWP)
				if (m_entity->GetVelocity().Length2D() < 5 || !(m_entity->GetFlags() & FL_ONGROUND))
					continue;
			if (Shonax::settingsxd.bodyaimiscout && Shonax::settingsxd.bodyaim_mode1 && *m_local->GetWeapon()->GetItemDefinitionIndex() == WEAPON_SSG08)
				if (m_entity->GetVelocity().Length2D() < 5 || !(m_entity->GetFlags() & FL_ONGROUND))
					continue;
			if (Shonax::settingsxd.bodyaimiawp && !Shonax::settingsxd.bodyaim_mode && *m_local->GetWeapon()->GetItemDefinitionIndex() == WEAPON_AWP)
				continue;
			if (Shonax::settingsxd.bodyaimiscout && !Shonax::settingsxd.bodyaim_mode1 && *m_local->GetWeapon()->GetItemDefinitionIndex() == WEAPON_SSG08)
				continue;
			
			if (Shonax::settingsxd.bodyaimonxp)
			{
				if (m_entity->GetHealth() <= Shonax::settingsxd.bodyaimonxp)
					continue;
			}
			
			if (Shonax::settingsxd.bodyaimifdead)
			{
				if (m_entity->GetHealth() <= bodydmg)
					continue;
			}
		}

		if (AWall) {
			
			Vector Point = game::functions.get_hitbox_location(m_entity, HitBoxID);
			float huipizda = Shonax::settingsxd.pointscale / 20;
			float pointscale = huipizda;
			Vector aimpoint = game::functions.get_hitbox_location(m_entity, HitBoxID) + Vector(0, 0, pointscale);
			float dmg = 0.f;
			if (CanHit(aimpoint, &dmg)) {
				
				if (dmg >= Shonax::settingsxd.autowalldamage) {
					return HitBoxID;
				}
			}
		}
		else {
			if (game::functions.visible(m_local, m_entity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

bool Aimbot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket, IClientEntity* pEntity)
{
	bool m_return = true;
	if (point.Length() == 0) return false;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	game::math.calculate_angle(src, point, angles);
	game::math.normalize_vector(angles);

	IsLocked = true;

	float m_fov = FovToPlayer(src, game::globals.m_last_angle_both, m_pEntityList->GetClientEntity(TargetID), 0);

	if (false)
	{
		/*Vector m_delta = angles - game::globals.m_last_angle_both;
		float m_dist = ragebotconfig.flAimStepValue;
		game::math.normalize_vector(m_delta);
		if (m_fov > m_dist)
		{
			game::math.normalize(m_delta, m_delta);
			m_delta *= m_dist;
			angles = game::globals.m_last_angle_both + m_delta;
			m_return = false;
		}
		else m_return = true;*/
	}
	else m_return = true;

	game::math.normalize_vector(angles);

	pCmd->viewangles = angles;
	
	if (!Shonax::settingsxd.aimbot_mode) {
		m_pEngine->SetViewAngles(angles);
	}

	return m_return;
}

inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;

	i += 127 << 23;
	i >>= 1;

	return *(float*)&i;
}

#define square( x ) ( x * x )

void ClampMovement(CUserCmd* pCommand, float fMaxSpeed)
{
	if (fMaxSpeed <= 0.f)
		return;

	float fSpeed = (float)(FastSqrt(square(pCommand->forwardmove) + square(pCommand->sidemove) + square(pCommand->upmove)));
	if (fSpeed <= 0.f)
		return;

	if (pCommand->buttons & IN_DUCK)
		fMaxSpeed *= 2.94117647f; // TO DO: Maybe look trough the leaked sdk for an exact value since this is straight out of my ass...

	if (fSpeed <= fMaxSpeed)
		return;

	float fRatio = fMaxSpeed / fSpeed;

	pCommand->forwardmove *= fRatio;
	pCommand->sidemove *= fRatio;
	pCommand->upmove *= fRatio;
}

void Aimbot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* m_local = game::localdata.localplayer();
	Vector Start = m_local->GetViewOffset() + m_local->GetOrigin();
	bool FindNewTarget = true;

	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)m_pEntityList->GetClientEntityFromHandle(m_local->GetActiveWeaponHandle()))->GetCSWpnData();

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)m_pEntityList->GetClientEntityFromHandle(m_local->GetActiveWeaponHandle());

	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || pWeapon->IsKnife() || pWeapon->IsC4() || pWeapon->IsGrenade())
			return;
	}
	else
		return;


	if (IsLocked && TargetID > -0 && HitBox >= 0)
	{
		pTarget = m_pEntityList->GetClientEntity(TargetID);
		if (pTarget && IsViable(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				
				Vector viewoff = m_local->GetOrigin() + m_local->GetViewOffset();
				Vector view; m_pEngine->GetViewAngles(view);
				float fov = FovToPlayer(viewoff, view, pTarget, HitBox);
				if (fov < Shonax::settingsxd.aimbotfov)
					FindNewTarget = false;
			}
		}
	}

	if (FindNewTarget)
	{
		game::globals.Shots = 0;
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;
		
		switch (Shonax::settingsxd.selection_mode)
		{
		case 0: TargetID = GetTargetFOV();
		case 1: TargetID = GetTargetDistance();
		case 2: TargetID = GetTargetHealth();
		}

		if (TargetID >= 0)
			pTarget = m_pEntityList->GetClientEntity(TargetID);
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	game::globals.Target = pTarget;

	if (TargetID >= 0 && pTarget)
	{
		HitBox = HitScan(pTarget);

		if (!game::functions.can_shoot())
			return;
		
		if (Shonax::settingsxd.autofire && Shonax::settingsxd.autofire_mode == 1)
		{
			
			int Key = Shonax::settingsxd.autofirehotkey;
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}
		
		if (Shonax::settingsxd.aimbot_mode == 2 && game::globals.choked_ticks >= 15)
			return;
		
		float huipizda = Shonax::settingsxd.pointscale / 20;
		float pointscale = huipizda;
		Vector point;
		Vector aimpoint = game::functions.get_hitbox_location(pTarget, HitBox) + Vector(0, 0, pointscale);
		if (Shonax::settingsxd.multipoint) point = GetBestPoint(pTarget, aimpoint); //Multipoint
		else point = aimpoint;
		
		if (Shonax::settingsxd.autoscope && pWeapon->m_bIsSniper() && !pWeapon->IsScoped()) pCmd->buttons |= IN_ATTACK2;
		else
		{
			if (Shonax::settingsxd.hitchance && hitchance() >= Shonax::settingsxd.hitchancescale * 1.5 || !Shonax::settingsxd.hitchance)
			{
				if (AimAtPoint(m_local, point, pCmd, bSendPacket, pTarget))
				{
					if (!Shonax::settingsxd.autofire && !(pCmd->buttons & IN_ATTACK))
						return;

					game::globals.aimbotting = true;
					if (Shonax::settingsxd.autofire)
					{
						pCmd->buttons |= IN_ATTACK;
						aimbotted_in_current_tick = true;
					}
					else
						return;
				}
			}
		}

		if (Shonax::settingsxd.accuracystanding && !(pCmd->buttons & IN_DUCK))
			ClampMovement(pCmd, 81.f);
	}
}
// Junk Code By Troll Face & Thaisen's Gen
void KCRoQQLCRqbTBsfIcknSkfbTuYwZaiH6922418() {     double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq81243440 = -268398065;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90661758 = -701289839;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq25187236 = -614785700;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq55698244 = -806899507;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq87406539 = -382131099;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq93658049 = -429151754;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq3866530 = -101121352;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq27503565 = -263222565;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq94505856 = -912729642;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq3045041 = -881839598;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq58524950 = -560795803;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq19913797 = -534257940;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31073958 = -554623494;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq64649481 = -383586390;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31300858 = 43646947;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq39642089 = -741946489;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq65932397 = 50091294;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq40929659 = -739707063;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq71884225 = -321768143;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46970028 = -49849618;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53473504 = -415536067;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq1919600 = -142285632;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq29006761 = -119304643;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq35860974 = -458312389;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq24091163 = -391392025;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq74264571 = -644489978;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq36251553 = -662024208;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq44222602 = -318876868;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq50663965 = -634242923;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq58634452 = -290141853;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46618981 = -903417660;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48117909 = -859814049;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq37901381 = -273461875;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq2229829 = -205568486;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq97364496 = -900337654;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq68813929 = 2204538;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq98324629 = 88203650;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33407203 = -487108586;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46075671 = 67073757;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48020039 = -754494658;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq21040699 = -28342375;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq10766446 = -6763916;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq62523413 = 57151847;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq9691950 = -581570338;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq87023484 = -140756981;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48521683 = -286993818;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq55999598 = -246394476;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq2304467 = -496612269;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq80041519 = -581376213;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq97487359 = -261304545;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq84420489 = -274726813;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76570228 = -887097634;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90843952 = -285059761;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33862999 = -264225592;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53448913 = -741683064;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq79323840 = -26112433;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq61654998 = -481985196;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq89326262 = -56473311;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31607081 = -315507483;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq13141968 = -737641122;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq57406497 = -767127546;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq59643927 = -782244485;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76839599 = -628979642;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq35871404 = -522587790;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq56426060 = -978421938;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq10407042 = -700981754;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq82012416 = -160796066;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq28844130 = -249055009;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq67284984 = -483248737;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq62486929 = -958557591;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq41317459 = -730150139;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq32525194 = -462800121;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq94853988 = -706780820;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq23864186 = -567273485;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq25929330 = 78492757;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq42707059 = -308772151;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq39396186 = -99437480;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq19314811 = -537734306;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48837489 = -217555409;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq75569480 = -4398207;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq18264973 = -298095503;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33947087 = -65411939;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq64181082 = -737500655;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53176605 = -272938379;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq74213962 = 84584960;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq70048752 = 83679973;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq57273956 = -474754289;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq4038383 = 90763716;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48780915 = -463885422;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq18040656 = -774225221;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq7158932 = -515810267;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq8998367 = -855323040;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq1800123 = -71601103;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq32933703 = -195285122;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90613542 = -987367112;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq61396771 = -246097891;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33926846 = -377784275;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq26652009 = -420260363;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53265890 = -603148400;    double GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76616442 = -268398065;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq81243440 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90661758;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90661758 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq25187236;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq25187236 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq55698244;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq55698244 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq87406539;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq87406539 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq93658049;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq93658049 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq3866530;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq3866530 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq27503565;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq27503565 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq94505856;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq94505856 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq3045041;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq3045041 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq58524950;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq58524950 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq19913797;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq19913797 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31073958;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31073958 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq64649481;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq64649481 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31300858;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31300858 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq39642089;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq39642089 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq65932397;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq65932397 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq40929659;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq40929659 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq71884225;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq71884225 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46970028;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46970028 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53473504;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53473504 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq1919600;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq1919600 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq29006761;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq29006761 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq35860974;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq35860974 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq24091163;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq24091163 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq74264571;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq74264571 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq36251553;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq36251553 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq44222602;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq44222602 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq50663965;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq50663965 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq58634452;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq58634452 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46618981;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46618981 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48117909;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48117909 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq37901381;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq37901381 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq2229829;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq2229829 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq97364496;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq97364496 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq68813929;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq68813929 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq98324629;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq98324629 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33407203;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33407203 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46075671;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq46075671 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48020039;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48020039 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq21040699;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq21040699 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq10766446;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq10766446 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq62523413;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq62523413 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq9691950;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq9691950 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq87023484;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq87023484 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48521683;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48521683 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq55999598;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq55999598 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq2304467;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq2304467 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq80041519;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq80041519 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq97487359;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq97487359 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq84420489;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq84420489 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76570228;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76570228 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90843952;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90843952 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33862999;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33862999 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53448913;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53448913 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq79323840;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq79323840 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq61654998;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq61654998 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq89326262;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq89326262 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31607081;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq31607081 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq13141968;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq13141968 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq57406497;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq57406497 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq59643927;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq59643927 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76839599;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76839599 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq35871404;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq35871404 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq56426060;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq56426060 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq10407042;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq10407042 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq82012416;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq82012416 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq28844130;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq28844130 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq67284984;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq67284984 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq62486929;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq62486929 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq41317459;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq41317459 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq32525194;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq32525194 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq94853988;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq94853988 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq23864186;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq23864186 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq25929330;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq25929330 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq42707059;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq42707059 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq39396186;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq39396186 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq19314811;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq19314811 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48837489;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48837489 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq75569480;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq75569480 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq18264973;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq18264973 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33947087;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33947087 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq64181082;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq64181082 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53176605;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53176605 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq74213962;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq74213962 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq70048752;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq70048752 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq57273956;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq57273956 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq4038383;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq4038383 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48780915;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq48780915 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq18040656;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq18040656 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq7158932;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq7158932 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq8998367;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq8998367 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq1800123;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq1800123 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq32933703;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq32933703 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90613542;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq90613542 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq61396771;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq61396771 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33926846;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq33926846 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq26652009;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq26652009 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53265890;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq53265890 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76616442;     GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq76616442 = GcFOSxLBCzWshKnuEtvBxSwlfaxusTmbjiUq81243440;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ioJTkebMLSpCnzgrVURmbFdjRfpUEms74325922() {     double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb6543034 = -96598920;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb44905212 = -95093077;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb67202013 = -299593387;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb52144333 = -823681138;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92148633 = -306328008;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb32637642 = 40683447;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb11893465 = -106326202;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb82735099 = -814785204;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10885486 = -766576411;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb5283001 = 1456911;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb99706011 = -80526377;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb42516759 = -771924185;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb6571574 = -289359332;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb95307165 = -141871229;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb89417059 = 24062461;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36502812 = -274885290;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb15469747 = -152308234;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb51820273 = -89509348;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb75240877 = -832749899;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb95465056 = -449606520;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb22453071 = -602764732;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92864219 = -50006929;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb4298798 = -632849051;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb14788988 = -730674634;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb12410857 = -866904018;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62677176 = -654689136;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb16723845 = -812845975;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40657352 = -341019484;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb35382577 = -854916331;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb68331827 = -814726265;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb38809185 = -487817106;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb8344838 = -891429502;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb73955042 = -978885314;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62533533 = -425900712;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92250927 = -124461617;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb58961920 = -404783353;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb28761971 = -50469557;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb69094610 = -140413393;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10692206 = -983284898;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb38982625 = -639843306;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb24929051 = -612855310;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb61621756 = -577477155;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62773916 = -329237777;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb84426548 = -784189419;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb26979659 = 44819625;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb50689088 = 73834742;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb26302306 = -870802281;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb66729566 = -435156293;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb46484128 = -30634232;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb70801931 = -371357604;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40982723 = -157069444;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb22371141 = -653529087;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb53095961 = -944827268;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb91756331 = -252913424;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb51872852 = 94568062;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb13678815 = 53408008;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40606414 = -462244027;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb52413026 = -568918753;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb39733476 = -956777121;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb29471458 = -651638872;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb15913797 = -146470579;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb71236112 = -765306718;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb47352523 = -959868874;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb42553659 = -951850146;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb66473815 = -510725983;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb91361173 = -189096875;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb68561717 = -793038871;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb44038040 = -863458620;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb3056238 = 82590387;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb30455140 = -571154187;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb7740842 = -124415734;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb46375137 = 88105158;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb41128068 = -106224451;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36258253 = -92906594;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb70536005 = -836751210;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb60831315 = 74712422;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb30090303 = -720769153;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb19872250 = -848659633;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb87809329 = -675494260;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb61721769 = -840738760;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36374870 = -783886856;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb49994278 = -277689683;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb94173224 = -210385252;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb64580645 = -383558727;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb27349104 = -557656822;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb16438045 = -834288020;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb55248876 = -946602234;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb82198710 = -625971890;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10660682 = -420468775;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb78572113 = -77869626;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb18355506 = -942539327;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb76348944 = -481550805;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb29361135 = -183636273;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb81220748 = -231646026;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb23068828 = -393372728;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb53692938 = -847548593;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb14269234 = -617608281;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb20220258 = -377387631;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb17952733 = -173463436;    double oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb35618485 = -96598920;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb6543034 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb44905212;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb44905212 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb67202013;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb67202013 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb52144333;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb52144333 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92148633;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92148633 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb32637642;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb32637642 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb11893465;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb11893465 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb82735099;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb82735099 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10885486;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10885486 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb5283001;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb5283001 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb99706011;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb99706011 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb42516759;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb42516759 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb6571574;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb6571574 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb95307165;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb95307165 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb89417059;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb89417059 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36502812;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36502812 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb15469747;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb15469747 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb51820273;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb51820273 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb75240877;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb75240877 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb95465056;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb95465056 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb22453071;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb22453071 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92864219;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92864219 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb4298798;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb4298798 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb14788988;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb14788988 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb12410857;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb12410857 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62677176;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62677176 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb16723845;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb16723845 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40657352;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40657352 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb35382577;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb35382577 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb68331827;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb68331827 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb38809185;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb38809185 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb8344838;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb8344838 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb73955042;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb73955042 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62533533;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62533533 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92250927;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb92250927 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb58961920;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb58961920 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb28761971;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb28761971 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb69094610;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb69094610 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10692206;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10692206 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb38982625;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb38982625 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb24929051;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb24929051 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb61621756;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb61621756 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62773916;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb62773916 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb84426548;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb84426548 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb26979659;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb26979659 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb50689088;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb50689088 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb26302306;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb26302306 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb66729566;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb66729566 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb46484128;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb46484128 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb70801931;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb70801931 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40982723;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40982723 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb22371141;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb22371141 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb53095961;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb53095961 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb91756331;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb91756331 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb51872852;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb51872852 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb13678815;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb13678815 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40606414;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb40606414 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb52413026;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb52413026 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb39733476;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb39733476 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb29471458;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb29471458 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb15913797;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb15913797 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb71236112;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb71236112 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb47352523;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb47352523 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb42553659;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb42553659 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb66473815;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb66473815 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb91361173;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb91361173 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb68561717;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb68561717 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb44038040;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb44038040 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb3056238;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb3056238 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb30455140;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb30455140 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb7740842;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb7740842 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb46375137;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb46375137 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb41128068;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb41128068 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36258253;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36258253 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb70536005;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb70536005 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb60831315;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb60831315 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb30090303;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb30090303 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb19872250;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb19872250 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb87809329;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb87809329 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb61721769;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb61721769 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36374870;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb36374870 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb49994278;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb49994278 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb94173224;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb94173224 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb64580645;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb64580645 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb27349104;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb27349104 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb16438045;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb16438045 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb55248876;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb55248876 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb82198710;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb82198710 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10660682;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb10660682 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb78572113;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb78572113 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb18355506;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb18355506 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb76348944;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb76348944 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb29361135;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb29361135 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb81220748;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb81220748 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb23068828;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb23068828 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb53692938;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb53692938 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb14269234;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb14269234 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb20220258;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb20220258 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb17952733;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb17952733 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb35618485;     oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb35618485 = oazXvVzTPHCBNgTLRPLlHSvwcLZCEpGIZZAb6543034;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fLkTvLeUMpAAapVItlLjcWAYpxLsqFu41729428() {     double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii31842626 = 75200224;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii99148664 = -588896316;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii9216792 = 15598927;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48590422 = -840462768;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii96890727 = -230524916;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii71617233 = -589481352;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii19920400 = -111531051;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii37966634 = -266347844;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii27265114 = -620423180;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii7520961 = -215246580;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii40887073 = -700256950;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii65119720 = 90409571;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82069189 = -24095169;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii25964850 = 99843931;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47533262 = 4477974;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii33363535 = -907824092;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii65007097 = -354707763;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii62710887 = -539311633;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78597529 = -243731656;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii43960084 = -849363422;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii91432637 = -789993397;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii83808838 = 42271775;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii79590834 = -46393459;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii93717001 = 96963120;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii730552 = -242416010;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii51089781 = -664888294;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii97196135 = -963667742;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii37092103 = -363162101;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20101189 = 24410262;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78029201 = -239310678;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii30999389 = -72216552;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii68571766 = -923044955;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii10008703 = -584308753;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii22837238 = -646232939;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii87137358 = -448585581;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49109910 = -811771244;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59199312 = -189142763;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii4782018 = -893718201;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii75308740 = -933643552;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29945211 = -525191954;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii28817403 = -97368245;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii12477067 = -48190393;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii63024419 = -715627401;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59161146 = -986808499;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii66935833 = -869603769;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii52856493 = -665336699;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii96605013 = -395210086;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii31154667 = -373700316;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii12926736 = -579892252;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii44116503 = -481410663;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii97544956 = -39412074;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii68172052 = -419960540;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15347970 = -504594776;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49649664 = -241601257;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii50296791 = -169180811;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48033788 = -967071551;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii19557831 = -442502857;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15499790 = 18635806;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47859870 = -498046758;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii45800947 = -565636623;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii74421097 = -625813611;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82828297 = -748368951;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii17865446 = -190758106;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49235913 = -281112503;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii76521571 = -43030029;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii72315306 = -777211996;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii55111018 = -325281676;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59231951 = -377862231;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii38827491 = -451570488;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii98423351 = -183750783;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii74164223 = -618681329;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii60225079 = -460989562;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii87402147 = -605668081;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48652319 = -718539702;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15142681 = -651995178;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78955570 = -641803004;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20784420 = -242100825;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20429688 = -59584960;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii26781169 = -33433111;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47874059 = -577079312;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii54484767 = -169678208;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii66041469 = -489967426;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii24165367 = -783269849;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii75984685 = -494179075;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii80484245 = -99898604;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii62827337 = -652256013;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii53223797 = -318450179;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii60359038 = -242707497;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii72540447 = -377052129;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii39103571 = -481514030;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29552080 = -269268387;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii43699522 = -107778569;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii56922147 = -295671443;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29507793 = -268006930;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii55524113 = -899378343;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii45989106 = -348999295;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii94611620 = -857432287;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii13788507 = -334514898;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82639574 = -843778471;    double RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii94620526 = 75200224;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii31842626 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii99148664;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii99148664 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii9216792;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii9216792 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48590422;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48590422 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii96890727;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii96890727 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii71617233;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii71617233 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii19920400;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii19920400 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii37966634;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii37966634 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii27265114;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii27265114 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii7520961;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii7520961 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii40887073;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii40887073 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii65119720;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii65119720 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82069189;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82069189 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii25964850;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii25964850 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47533262;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47533262 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii33363535;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii33363535 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii65007097;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii65007097 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii62710887;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii62710887 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78597529;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78597529 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii43960084;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii43960084 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii91432637;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii91432637 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii83808838;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii83808838 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii79590834;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii79590834 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii93717001;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii93717001 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii730552;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii730552 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii51089781;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii51089781 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii97196135;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii97196135 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii37092103;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii37092103 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20101189;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20101189 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78029201;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78029201 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii30999389;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii30999389 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii68571766;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii68571766 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii10008703;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii10008703 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii22837238;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii22837238 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii87137358;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii87137358 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49109910;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49109910 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59199312;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59199312 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii4782018;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii4782018 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii75308740;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii75308740 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29945211;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29945211 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii28817403;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii28817403 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii12477067;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii12477067 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii63024419;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii63024419 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59161146;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59161146 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii66935833;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii66935833 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii52856493;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii52856493 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii96605013;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii96605013 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii31154667;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii31154667 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii12926736;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii12926736 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii44116503;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii44116503 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii97544956;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii97544956 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii68172052;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii68172052 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15347970;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15347970 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49649664;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49649664 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii50296791;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii50296791 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48033788;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48033788 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii19557831;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii19557831 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15499790;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15499790 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47859870;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47859870 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii45800947;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii45800947 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii74421097;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii74421097 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82828297;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82828297 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii17865446;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii17865446 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49235913;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii49235913 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii76521571;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii76521571 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii72315306;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii72315306 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii55111018;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii55111018 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59231951;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii59231951 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii38827491;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii38827491 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii98423351;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii98423351 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii74164223;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii74164223 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii60225079;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii60225079 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii87402147;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii87402147 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48652319;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii48652319 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15142681;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii15142681 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78955570;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii78955570 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20784420;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20784420 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20429688;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii20429688 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii26781169;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii26781169 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47874059;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii47874059 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii54484767;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii54484767 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii66041469;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii66041469 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii24165367;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii24165367 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii75984685;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii75984685 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii80484245;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii80484245 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii62827337;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii62827337 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii53223797;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii53223797 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii60359038;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii60359038 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii72540447;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii72540447 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii39103571;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii39103571 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29552080;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29552080 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii43699522;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii43699522 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii56922147;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii56922147 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29507793;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii29507793 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii55524113;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii55524113 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii45989106;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii45989106 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii94611620;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii94611620 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii13788507;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii13788507 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82639574;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii82639574 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii94620526;     RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii94620526 = RScZxyeyoeBuZPMptvWHEKzLILMeyAmeGvii31842626;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void NiGxwBZuGTopqVKpAlebHFsAbnQroYL33464358() {     int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT67343974 = -865634579;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT24812558 = -737866291;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT70949299 = -255836443;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT48724586 = -842877707;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT13570135 = -535267346;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51427698 = -438802612;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT72812352 = -852326718;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85272471 = -371069821;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT7477281 = 99010083;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT619841 = -69809036;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69404581 = -801519039;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT53308129 = -982132049;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT47463686 = -212822725;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT22467522 = -934015884;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT74017563 = -884089155;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT46135412 = -79217738;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT93727490 = 29491942;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT37050528 = -760982971;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT48681420 = -197814289;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT97288513 = -999018331;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT99163030 = -374233843;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT46836030 = -163302110;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT60780373 = -892363919;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51581174 = -403541329;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT13224949 = -698395726;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94168425 = -921897310;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT81731499 = -464813327;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT56103997 = -442004552;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT11362920 = -881132260;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69504923 = 15384216;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT99071640 = -183540983;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT7293955 = 56712451;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT81799300 = -278581727;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85989957 = -932431817;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94375975 = -809853533;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT55315589 = -64625752;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT29201646 = -798287638;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT40286897 = -902822334;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT52862689 = -890744616;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT3785731 = -845687160;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT10844606 = -31624748;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT49424517 = -471479445;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT44371810 = 50889152;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT98372546 = -261616037;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT3774620 = -385932618;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT83723868 = -13750508;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85304065 = -579008546;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT95742779 = 86675741;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT32985819 = -747840068;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT59182189 = -445797416;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT4458539 = -993993181;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT97612682 = -408352406;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT2071161 = -631791744;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT86353914 = 64086142;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT34301328 = -67462240;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT20507944 = -602332470;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT64032185 = -845502372;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT19368126 = -852295114;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT35499637 = -44481981;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT19401710 = -613370037;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69696198 = -973989285;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT16708355 = -310322166;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT73909551 = -489937562;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT37972358 = -916374134;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT1548201 = -886268054;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT62110627 = -758231491;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT71508829 = -603550323;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT61473728 = -280390908;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT28091547 = -24162351;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT18701975 = -719463404;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT16933766 = -280930100;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT53440594 = -67685725;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT84187838 = -870238356;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT44895690 = -352127129;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT86443907 = -867393583;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT49738513 = -902754398;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT2464220 = -114191262;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT62407826 = -530747883;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT47806555 = 82391288;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT29501081 = -584645219;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT8864360 = -242888764;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85988720 = -451489068;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT23118179 = -694164484;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT52180731 = -335334845;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT65046384 = 9377397;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT1458958 = -775188577;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT5222795 = -311495805;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT95445385 = -242667869;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51688630 = -764969577;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT73868031 = -107521064;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT91283404 = -219123380;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT9833521 = -945992524;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT4787260 = -758340353;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT33460980 = -177374580;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT34089532 = -871697876;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94136251 = -721302582;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT75514966 = -981541884;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT6399452 = -32736715;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT96824346 = -375347983;    int pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT41663993 = -865634579;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT67343974 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT24812558;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT24812558 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT70949299;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT70949299 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT48724586;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT48724586 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT13570135;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT13570135 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51427698;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51427698 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT72812352;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT72812352 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85272471;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85272471 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT7477281;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT7477281 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT619841;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT619841 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69404581;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69404581 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT53308129;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT53308129 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT47463686;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT47463686 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT22467522;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT22467522 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT74017563;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT74017563 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT46135412;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT46135412 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT93727490;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT93727490 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT37050528;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT37050528 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT48681420;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT48681420 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT97288513;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT97288513 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT99163030;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT99163030 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT46836030;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT46836030 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT60780373;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT60780373 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51581174;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51581174 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT13224949;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT13224949 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94168425;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94168425 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT81731499;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT81731499 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT56103997;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT56103997 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT11362920;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT11362920 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69504923;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69504923 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT99071640;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT99071640 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT7293955;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT7293955 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT81799300;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT81799300 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85989957;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85989957 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94375975;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94375975 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT55315589;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT55315589 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT29201646;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT29201646 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT40286897;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT40286897 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT52862689;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT52862689 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT3785731;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT3785731 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT10844606;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT10844606 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT49424517;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT49424517 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT44371810;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT44371810 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT98372546;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT98372546 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT3774620;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT3774620 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT83723868;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT83723868 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85304065;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85304065 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT95742779;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT95742779 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT32985819;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT32985819 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT59182189;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT59182189 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT4458539;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT4458539 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT97612682;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT97612682 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT2071161;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT2071161 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT86353914;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT86353914 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT34301328;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT34301328 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT20507944;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT20507944 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT64032185;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT64032185 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT19368126;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT19368126 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT35499637;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT35499637 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT19401710;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT19401710 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69696198;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT69696198 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT16708355;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT16708355 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT73909551;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT73909551 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT37972358;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT37972358 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT1548201;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT1548201 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT62110627;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT62110627 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT71508829;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT71508829 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT61473728;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT61473728 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT28091547;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT28091547 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT18701975;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT18701975 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT16933766;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT16933766 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT53440594;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT53440594 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT84187838;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT84187838 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT44895690;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT44895690 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT86443907;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT86443907 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT49738513;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT49738513 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT2464220;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT2464220 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT62407826;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT62407826 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT47806555;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT47806555 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT29501081;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT29501081 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT8864360;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT8864360 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85988720;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT85988720 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT23118179;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT23118179 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT52180731;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT52180731 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT65046384;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT65046384 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT1458958;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT1458958 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT5222795;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT5222795 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT95445385;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT95445385 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51688630;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT51688630 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT73868031;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT73868031 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT91283404;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT91283404 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT9833521;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT9833521 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT4787260;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT4787260 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT33460980;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT33460980 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT34089532;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT34089532 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94136251;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT94136251 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT75514966;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT75514966 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT6399452;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT6399452 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT96824346;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT96824346 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT41663993;     pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT41663993 = pWEZFbiQwRXgpjKoRRGtUurwIpGVJliFhHyT67343974;}
// Junk Finished
