#include "Ragebot.h"
//#include <chrono>
#include "shonaxsettings.h"
#include "AntiAntiAim.h"

bool is_viable_target(IClientEntity* pEntity)
{
	IClientEntity* m_local = game::localdata.localplayer();
	if (!pEntity) return false;
	if (pEntity->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer) return false;
	if (pEntity == m_local) return false;
	if (pEntity->GetTeamNum() == m_local->GetTeamNum()) return false;
	if (pEntity->m_bGunGameImmunity()) return false;
	if (!pEntity->IsAlive() || pEntity->IsDormant()) return false;
	return true;
}

void LegitAA(CUserCmd *pCmd, bool& bSendPacket)
{
	IClientEntity* pLocal = game::localdata.localplayer();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;

	//for the memes
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	
	if (Shonax::settingsxd.legitantiaim && Shonax::settingsxd.legitchoicelist!=0 && !pCmd->buttons & IN_ATTACK)
	{
		switch (Shonax::settingsxd.legitchoicelist)
		{
		case 1:
		{//you dont need brackets but for some shit you do like statics //wrapzii C++ class 101
			static bool kFlip = true;
			static int ChokedPackets = -1;
			static bool yFlip = true;
			if (1 > ChokedPackets)
			{
				bSendPacket = true;
				ChokedPackets++;
			}
			else
			{
				bSendPacket = false;
				//pCmd->viewangles.y += yFlip ? 90.f : -90.f;
				pCmd->viewangles.y += 180.f;
				ChokedPackets = -1;
			}
		}
		break;
		case 2:
		{
			static bool kFlip = true;
			static int ChokedPackets = -1;
			static bool yFlip = true;
			bool flip_bool2;
			if (1 > ChokedPackets)
			{
				bSendPacket = true;
				ChokedPackets++;
			}
			else
			{
				if (GetKeyState(0xA4) & 1) // f
					flip_bool2 = !flip_bool2;

				if (flip_bool2)
				{
					bSendPacket = false;
					//pCmd->viewangles.y += yFlip ? 90.f : -90.f;
					pCmd->viewangles.y += 90.f;
					ChokedPackets = 0;

				}
				else
				{
					bSendPacket = false;
					//pCmd->viewangles.y += yFlip ? 90.f : -90.f;
					pCmd->viewangles.y -= 90.f;
					ChokedPackets = 0;
				}
			}
		}
		break;
		case 3:

			//put my p ass vel check one
			//Sideways-switch
		{
			static int ChokedPackets = -1;//we choking 2 cuz 1 is too main stream
			if (1 > ChokedPackets) {
				bSendPacket = false;
				static bool dir = false;
				static bool dir2 = false;
				int i = 0; i < m_pEntityList->GetHighestEntityIndex(); ++i;
				auto entity = m_pEntityList->GetClientEntity(i);
				//if (pCmd->forwardmove > 1 || (IsVisible(pLocal, pEntity, 0) && pEntity->GetTeamNum() != pLocal->GetTeamNum()))// was trying to make a vis check to make it -180 if their visible //didnt seem to work
				//dir2 = true;
				//else {
				dir2 = false;
				if (pCmd->sidemove > 1) dir = true;
				else if (pCmd->sidemove < -1) dir = false;
				pCmd->viewangles.y = (dir) ? (pCmd->viewangles.y - 180) - 270.f : (pCmd->viewangles.y - 180) - 90.f;
				//}
				//if (dir2 = true)
				//pCmd->viewangles.y = pCmd->viewangles.y - 180;
				ChokedPackets++;
			}
			else
			{
				bSendPacket = true;
				ChokedPackets = -1;

			}
		}
		break;
		}

	}
	else
		pCmd->viewangles.y += 0;

	
}

void normalize_angle(float& flAngle)
{
	if (std::isnan(flAngle)) flAngle = 0.0f;
	if (std::isinf(flAngle)) flAngle = 0.0f;

	float flRevolutions = flAngle / 360;

	if (flAngle > 180 || flAngle < -180)
	{
		if (flRevolutions < 0)
			flRevolutions = -flRevolutions;

		flRevolutions = round(flRevolutions);

		if (flAngle < 0)
			flAngle = (flAngle + 360 * flRevolutions);
		else
			flAngle = (flAngle - 360 * flRevolutions);
	}
}

void Pitch_AntiAims::down(float& angle)
{
	angle = 77.0f;
}
void Pitch_AntiAims::fake_down(float& angle)
{
	angle = -180.990005f;
}
void Pitch_AntiAims::up(float& angle)
{
	angle = -90.f;
}
void Pitch_AntiAims::fake_up(float& angle)
{
	angle = -270.f;
}
void Pitch_AntiAims::random(float& angle)
{
	angle = game::math.random_float(-89, 89);
}
void Yaw_AntiAims::sideways(float& angle)
{
	angle += 90;
}
void Yaw_AntiAims::backwards(float& angle)
{
	angle -= 180;
}
void Yaw_AntiAims::crooked(float& angle)
{
	angle += 145;
}
void Yaw_AntiAims::jitter(float& angle, CUserCmd* m_pcmd)
{
	
	static bool flip = false; flip = !flip;
	float range = Shonax::settingsxd.jitterrange / 2;
	if (!flip)
		angle += 180 - range;
	else
		angle -= 180 - range;
}
void Yaw_AntiAims::swap(float& angle, CUserCmd* pCmd)

{

	int W, H;
	m_pEngine->GetScreenSize(W, H);

	static bool side = false;
	static bool side2 = false;
	static bool back = false;
	static bool flip = false; flip = !flip;
	static int Ticks;
	Ticks += 2;
	if (Ticks > 210)
		Ticks = 150;
	float range = Shonax::settingsxd.jitterrange / 2;
	
	if (Shonax::settingsxd.manualangeflip && GetAsyncKeyState(Shonax::settingsxd.rightflip))
	{
		side = true;
		side2 = false;
		back = false;
	}
	else if (Shonax::settingsxd.manualangeflip && GetAsyncKeyState(Shonax::settingsxd.leftflip))
	{
		side = false;
		side2 = true;
		back = false;
	}
	else if (Shonax::settingsxd.manualangeflip && GetAsyncKeyState(Shonax::settingsxd.backflip))
	{
		side = false;
		side2 = false;
		back = true;
	}
	if (side)
		angle -= 90 + Ticks;
	else if (side2)
		angle += 90 - Ticks;
	else if (back)
		angle += 180;
}
void Yaw_AntiAims::rotate(float& angle)
{
	
	angle += m_pGlobals->curtime * (Shonax::settingsxd.speedseed * 1000);
	normalize_angle(angle);
}
void Yaw_AntiAims::corruption(float& angle, CUserCmd* m_pcmd)
{
	static bool side = false;
	static bool side2 = false;
	static bool back = false;
	static bool flip = false; flip = !flip;
	float range = Shonax::settingsxd.jitterrange / 2;
	if (Shonax::settingsxd.manualangeflip && GetAsyncKeyState(Shonax::settingsxd.rightflip))
	{
		side = true;
		side2 = false;
		back = false;
	}
	else if (Shonax::settingsxd.manualangeflip && GetAsyncKeyState(Shonax::settingsxd.leftflip))
	{
		side = false;
		side2 = true;
		back = false;
	}
	else if (Shonax::settingsxd.manualangeflip && GetAsyncKeyState(Shonax::settingsxd.backflip))
	{
		side = false;
		side2 = false;
		back = true;
	}
	if (side)
		angle -= 90;
	else if (side2)
		angle += 90;
	else if (back)
		angle += 180;
}
void Yaw_AntiAims::lowerbody(float& angle)
{
	auto m_local = game::localdata.localplayer();
	angle = m_local->GetLowerBodyYaw() + rand() % 180 - rand() % 50;
}
enum ADAPTIVE_SIDE {
	ADAPTIVE_UNKNOWN,
	ADAPTIVE_LEFT,
	ADAPTIVE_RIGHT
};

float AntiAim::curtime_fixed(CUserCmd* ucmd) {
	auto local_player = game::localdata.localplayer();
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = local_player->GetTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * m_pGlobals->interval_per_tick;
	return curtime;
}

bool AntiAim::next_lby_update_func(CUserCmd* m_pcmd, const float yaw_to_break) {
	auto m_local = game::localdata.localplayer();

	if (m_local) {
		static float last_attempted_yaw;
		static float old_lby;
		static float next_lby_update_time;
		const float current_time = curtime_fixed(m_pcmd); // Fixes curtime to the frame so it breaks perfectly every time if delta is in range

		if (old_lby != m_local->GetLowerBodyYaw() && last_attempted_yaw != m_local->GetLowerBodyYaw()) {
			old_lby = m_local->GetLowerBodyYaw();
			if (m_local->GetVelocity().Length2D() < 0.1) {
				auto latency = (m_pEngine->GetNetChannelInfo()->GetAvgLatency(FLOW_INCOMING) + m_pEngine->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING));
				next_lby_update_time = current_time + 1.1f;
			}
		}

		if (m_local->GetVelocity().Length2D() < 0.1) {
			if ((next_lby_update_time < current_time) && m_local->GetFlags() & FL_ONGROUND) {
				last_attempted_yaw = yaw_to_break;
				next_lby_update_time = current_time + 1.1f;
				return true;
			}
		}
	}

	return false;
}

void AntiAim::Manage(CUserCmd* pCmd, bool& bSendPacket)
{
	static int ChokedPackets = -1;
	auto m_local = game::localdata.localplayer();
	auto m_weapon = m_local->GetWeapon();
	if (!m_local)
		return;

	auto cs_Grenade = reinterpret_cast<CBaseCSGrenade*>(m_weapon);
	if (m_weapon->IsGrenade())
		if (cs_Grenade->throw_time() > 0.f)
			return;

	if (m_weapon->IsKnife() && pCmd->buttons & IN_ATTACK || m_weapon->IsKnife() && pCmd->buttons & IN_ATTACK2)
		return;

	if (m_weapon->IsC4() && pCmd->buttons & IN_ATTACK)
		return;

	if (pCmd->buttons & IN_USE)
		return;

	if (m_local->GetMoveType() == 8 || m_local->GetMoveType() == 9)
		return;

	if (ChokedPackets < 1 && m_local->IsAlive() && pCmd->buttons & IN_ATTACK && game::functions.can_shoot() && !m_weapon->IsKnife())
		bSendPacket = false;
	else
	{
		if (m_local->IsAlive())
		{
			int MoveType = m_local->GetMoveType();
			
			if (Shonax::settingsxd.dormantcheck)
			{
				bool dormant = true;
				for (int i = 1; i < m_pGlobals->maxClients; i++)
				{
					IClientEntity* ent = m_pEntityList->GetClientEntity(i);
					if (!ent || ent->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer || ent->GetTeamNum() == m_local->GetTeamNum() || !ent->IsAlive()) continue;
					if (ent->IsDormant() == false)
						dormant = false;
				}

				if (dormant)
					return;
			}

			PitchOverrideTick(pCmd);

			if (!game::globals.fakelag)
				bSendPacket = pCmd->command_number % 2;

			if (!freestanding(pCmd, bSendPacket)) {
				if (bSendPacket) {
					FakeYawOverride(pCmd);
				}
				else {
					RealYawOverride(pCmd);
				}

				bool clean_up;
				
				if (game::localdata.localplayer()->GetVelocity().Length() < 6)
					clean_up = Shonax::settingsxd.standaacleanup;
				else
					clean_up = Shonax::settingsxd.moveaacleanup;

				if (clean_up)
				{
					static float last_fake;
					static float last_real;

					if (bSendPacket)
						last_fake = pCmd->viewangles.y;
					else
						last_real = pCmd->viewangles.y;

					if (game::math.is_close(last_real, last_fake, 35) && !bSendPacket)
						pCmd->viewangles.y -= 90;
				}
			}
			
			if (!Shonax::settingsxd.lbybreaker) {
				if (next_lby_update_func(pCmd, pCmd->viewangles.y + Shonax::settingsxd.lbydelta)) {
					pCmd->viewangles.y += Shonax::settingsxd.lbydelta;
				}
			}
		}
		ChokedPackets = -1;
	}
}

void AntiAim::PitchOverrideTick(CUserCmd* pCmd)
{
	float pitch;

	int type;
	
	if (game::localdata.localplayer()->GetVelocity().Length() < 6)
		type = Shonax::settingsxd.standaapitch;
	else
		type = Shonax::settingsxd.moveaapitch;

	if (type == 1) pitches.down(pitch);
	else if (type == 3) pitches.fake_down(pitch);
	else if (type == 4) pitches.up(pitch);
	else if (type == 5) pitches.fake_up(pitch);
	else if (type == 5) pitches.random(pitch);
	else return;

	pCmd->viewangles.x = pitch;
}

void AntiAim::RealYawOverride(CUserCmd* pCmd)
{
	float yaw = 0;

	if (Shonax::settingsxd.autodirection_mode == 0)
		yaw = pCmd->viewangles.y;
	else if (Shonax::settingsxd.autodirection_mode == 1)
		yaw = 0;
	else if (Shonax::settingsxd.autodirection_mode == 2)
	{
		auto m_local = game::localdata.localplayer();
		int CurrentTarget = 0;
		float LastDistance = 999999999999.0f;

		for (int i = 1; i < 65; i++)
		{
			auto pEntity = static_cast<IClientEntity*>(m_pEntityList->GetClientEntity(i));
			if (is_viable_target(pEntity))
			{
				float CurrentDistance = (pEntity->GetOrigin() - m_local->GetOrigin()).Length();
				if (!CurrentTarget || CurrentDistance < LastDistance)
				{
					CurrentTarget = i;
					LastDistance = CurrentDistance;
				}
			}
		}

		if (!CurrentTarget)
			yaw = pCmd->viewangles.y;
		else
		{
			auto pEntity = static_cast<IClientEntity*>(m_pEntityList->GetClientEntity(CurrentTarget));
			Vector LookAtAngle = (pEntity->GetOrigin() - m_local->GetOrigin()).Angle();
			yaw = LookAtAngle.y;
		}
	}

	int type;
	
	if (game::localdata.localplayer()->GetVelocity().Length() < 6)
		type = Shonax::settingsxd.standaayaw;
	else
		type = Shonax::settingsxd.moveaayaw;

	if (type == 1) yaws.sideways(yaw);
	else if (type == 2) yaws.backwards(yaw);
	else if (type == 3) yaws.crooked(yaw);
	else if (type == 4) yaws.jitter(yaw, pCmd);
	//else if (type == 5) yaws.swap(yaw, pCmd);
	else if (type == 5) yaws.rotate(yaw);
	else if (type == 6) yaws.lowerbody(yaw);
	else if (type == 7) yaws.corruption(yaw, pCmd);
	else return;
	
	if (game::localdata.localplayer()->GetVelocity().Length() < 6)
		yaw += Shonax::settingsxd.yawoffset;
	else
		yaw += Shonax::settingsxd.moveyawoffset;

	pCmd->viewangles.y = yaw;
}

void AntiAim::FakeYawOverride(CUserCmd* pCmd)
{
	float yaw = 0;

	if (Shonax::settingsxd.autodirection_mode == 0)
		yaw = pCmd->viewangles.y;
	else if (Shonax::settingsxd.autodirection_mode == 1)
		yaw = 0;
	else if (Shonax::settingsxd.autodirection_mode == 2)
	{
		auto m_local = game::localdata.localplayer();
		int CurrentTarget = 0;
		float LastDistance = 999999999999.0f;

		for (int i = 1; i < 65; i++)
		{
			auto pEntity = static_cast< IClientEntity* >(m_pEntityList->GetClientEntity(i));
			if (is_viable_target(pEntity))
			{
				float CurrentDistance = (pEntity->GetOrigin() - m_local->GetOrigin()).Length();
				if (!CurrentTarget || CurrentDistance < LastDistance)
				{
					CurrentTarget = i;
					LastDistance = CurrentDistance;
				}
			}
		}

		if (!CurrentTarget)
			yaw = pCmd->viewangles.y;
		else
		{
			auto pEntity = static_cast< IClientEntity* >(m_pEntityList->GetClientEntity(CurrentTarget));
			Vector LookAtAngle = (pEntity->GetOrigin() - m_local->GetOrigin()).Angle();
			yaw = LookAtAngle.y;
		}
	}

	int type;
	
	if (game::localdata.localplayer()->GetVelocity().Length() < 6)
		type = Shonax::settingsxd.fakestandaayaw;
	else
		type = Shonax::settingsxd.movefakeaayaw;

	if (type == 1) yaws.sideways(yaw);
	else if (type == 2) yaws.backwards(yaw);
	else if (type == 3) yaws.crooked(yaw);
	else if (type == 4) yaws.jitter(yaw, pCmd);
	else if (type == 5) yaws.swap(yaw, pCmd);
	else if (type == 6) yaws.rotate(yaw);
	else if (type == 7) yaws.lowerbody(yaw);
	else if (type == 8) yaws.corruption(yaw, pCmd);
	else return;
	
	if (game::localdata.localplayer()->GetVelocity().Length() < 6)
		yaw += Shonax::settingsxd.fakeyawoffset;
	else
		yaw += Shonax::settingsxd.movefakeyawoffset;

	pCmd->viewangles.y = yaw;
}

bool AntiAim::freestanding(CUserCmd* m_pcmd, bool packet)
{
	IClientEntity* m_local = game::localdata.localplayer();

	if (Shonax::settingsxd.edge_mode == 1 && m_local->GetVelocity().Length() < 300) {

		auto fov_to_player = [](Vector view_offset, Vector view, IClientEntity* m_entity, int hitbox)
		{
			CONST FLOAT MaxDegrees = 180.0f;
			Vector Angles = view;
			Vector Origin = view_offset;
			Vector Delta(0, 0, 0);
			Vector Forward(0, 0, 0);
			game::math.angle_vectors(Angles, Forward);
			Vector AimPos = game::functions.get_hitbox_location(m_entity, hitbox);
			game::math.vector_subtract(AimPos, Origin, Delta);
			game::math.normalize(Delta, Delta);
			FLOAT DotProduct = Forward.Dot(Delta);
			return (acos(DotProduct) * (MaxDegrees / PI));
		};

		int target = -1;
		float mfov = 360;

		Vector viewoffset = m_local->GetOrigin() + m_local->GetViewOffset();
		Vector view; m_pEngine->GetViewAngles(view);

		for (int i = 0; i < m_pGlobals->maxClients; i++) {
			IClientEntity* m_entity = m_pEntityList->GetClientEntity(i);

			if (is_viable_target(m_entity)) {

				float fov = fov_to_player(viewoffset, view, m_entity, 0);
				if (fov < mfov) {
					mfov = fov;
					target = i;
				}
			}
		}

		Vector at_target_angle;

		if (target) {
			auto m_entity = m_pEntityList->GetClientEntity(target);

			if (is_viable_target(m_entity)) {
				Vector head_pos_screen;

				if (game::functions.world_to_screen(m_entity->GetHeadPos(), head_pos_screen)) {

					float pitch = m_pcmd->viewangles.x;
					int type = Shonax::settingsxd.edgeaapitch;
					if (type == 1) pitches.down(pitch);
					else if (type == 3) pitches.fake_down(pitch);
					else if (type == 4) pitches.up(pitch);
					else if (type == 5) pitches.fake_up(pitch);
					else if (type == 5) pitches.random(pitch);
					m_pcmd->viewangles.x = pitch;

					float yaw = m_pcmd->viewangles.y;
					type = packet ? Shonax::settingsxd.edgefakeyaw : Shonax::settingsxd.edgeaayaw;
					if (type == 1) yaws.sideways(yaw);
					else if (type == 2) yaws.backwards(yaw);
					else if (type == 3) yaws.crooked(yaw);
					else if (type == 4) yaws.jitter(yaw, m_pcmd);
					else if (type == 5) yaws.swap(yaw, m_pcmd);
					else if (type == 6) yaws.rotate(yaw);
					if (game::localdata.localplayer()->GetVelocity().Length() < 6) yaw += Shonax::settingsxd.standaayaw;
					else yaw += Shonax::settingsxd.moveaayaw;
					m_pcmd->viewangles.y = yaw;

					game::math.calculate_angle(m_local->GetOrigin(), m_entity->GetOrigin(), at_target_angle);
					at_target_angle.x = 0;

					Vector src3D, dst3D, forward, right, up, src, dst;
					float back_two, right_two, left_two;
					trace_t tr;
					Ray_t ray, ray2, ray3, ray4, ray5;
					CTraceFilter filter;

					const Vector to_convert = at_target_angle;
					game::math.angle_vectors(to_convert, &forward, &right, &up);

					filter.pSkip = m_local;
					src3D = m_local->GetEyePosition();
					dst3D = src3D + (forward * 384); //Might want to experiment with other numbers, incase you don't know what the number does, its how far the trace will go. Lower = shorter.

					ray.Init(src3D, dst3D);
					m_pTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
					back_two = (tr.endpos - tr.startpos).Length();

					ray2.Init(src3D + right * 35, dst3D + right * 35);
					m_pTrace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
					right_two = (tr.endpos - tr.startpos).Length();

					ray3.Init(src3D - right * 35, dst3D - right * 35);
					m_pTrace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
					left_two = (tr.endpos - tr.startpos).Length();

					if (right_two > left_two) {
						m_pcmd->viewangles.y -= 91;
						//Body should be right
					}
					else if (left_two > right_two) {
						m_pcmd->viewangles.y += 91;
						//Body should be left
					}
					//if (packet) m_pcmd->viewangles.y += 180;
					return true;
				}
			}
		}
	}
	return false;
}


// Junk Code By Troll Face & Thaisen's Gen
void ZmVRXVVJZN3385594() {     float YvCRllSZic99805392 = 6966042;    float YvCRllSZic30293182 = -91034294;    float YvCRllSZic63516304 = -587708803;    float YvCRllSZic52757018 = -726347681;    float YvCRllSZic24644487 = -965985941;    float YvCRllSZic6556008 = -924360717;    float YvCRllSZic45337240 = -296138076;    float YvCRllSZic62392198 = -475721894;    float YvCRllSZic15883637 = -954265151;    float YvCRllSZic92302833 = -501662840;    float YvCRllSZic855858 = -446089050;    float YvCRllSZic31419582 = -53459967;    float YvCRllSZic88685402 = -947891476;    float YvCRllSZic57492595 = -663819160;    float YvCRllSZic12343090 = -82347517;    float YvCRllSZic54710617 = -343840241;    float YvCRllSZic68153118 = -78390971;    float YvCRllSZic8654712 = -560656094;    float YvCRllSZic95772293 = -509055711;    float YvCRllSZic74193895 = -331016487;    float YvCRllSZic42371586 = -836838475;    float YvCRllSZic85385425 = 74776592;    float YvCRllSZic87604983 = -734291486;    float YvCRllSZic17006506 = -910973613;    float YvCRllSZic20156632 = -748934459;    float YvCRllSZic49884068 = 64465979;    float YvCRllSZic29984553 = 61920273;    float YvCRllSZic41335800 = 7407691;    float YvCRllSZic64014628 = -235010569;    float YvCRllSZic72087052 = -192136673;    float YvCRllSZic4106002 = -38300321;    float YvCRllSZic39028652 = -48059877;    float YvCRllSZic4843811 = -847429368;    float YvCRllSZic12772046 = -687973799;    float YvCRllSZic81909627 = -884542628;    float YvCRllSZic16103574 = -904253587;    float YvCRllSZic52225390 = -126164960;    float YvCRllSZic2107650 = 48754490;    float YvCRllSZic95916306 = -391204702;    float YvCRllSZic51399627 = 15178853;    float YvCRllSZic2376608 = -302680288;    float YvCRllSZic26660956 = 92659628;    float YvCRllSZic21320999 = -728177957;    float YvCRllSZic30965879 = -928998751;    float YvCRllSZic75233846 = -811524690;    float YvCRllSZic18118139 = -38970904;    float YvCRllSZic58546603 = -989237011;    float YvCRllSZic13063986 = -571600956;    float YvCRllSZic21116999 = -584937720;    float YvCRllSZic5577414 = -833049861;    float YvCRllSZic72921767 = -839482188;    float YvCRllSZic76725853 = -28226659;    float YvCRllSZic92034312 = -638175725;    float YvCRllSZic75975002 = -98523996;    float YvCRllSZic61014008 = 84311527;    float YvCRllSZic14419968 = 32189450;    float YvCRllSZic42688199 = -356742809;    float YvCRllSZic46509798 = -676735190;    float YvCRllSZic32600387 = -977413222;    float YvCRllSZic74760418 = -930451920;    float YvCRllSZic76571454 = -886280991;    float YvCRllSZic4001440 = -203545767;    float YvCRllSZic98377569 = -140711326;    float YvCRllSZic43796584 = -662128478;    float YvCRllSZic88196832 = -363362520;    float YvCRllSZic61827205 = -298029174;    float YvCRllSZic26575772 = -206030599;    float YvCRllSZic75913356 = -159917677;    float YvCRllSZic75582968 = -779276532;    float YvCRllSZic96239515 = -178093931;    float YvCRllSZic2485227 = -117675281;    float YvCRllSZic66045469 = -27145461;    float YvCRllSZic12738405 = -69451393;    float YvCRllSZic44372667 = -424234565;    float YvCRllSZic71817288 = 71663801;    float YvCRllSZic15710630 = -829498103;    float YvCRllSZic64064426 = -197045452;    float YvCRllSZic56639105 = -805292736;    float YvCRllSZic41772660 = 551077;    float YvCRllSZic2038493 = -609963555;    float YvCRllSZic91337465 = 53702990;    float YvCRllSZic16920567 = -366478772;    float YvCRllSZic20218801 = -407654589;    float YvCRllSZic58437214 = -401960708;    float YvCRllSZic99165285 = -352654485;    float YvCRllSZic27380149 = 89926338;    float YvCRllSZic46994340 = -409884153;    float YvCRllSZic28868809 = -648905373;    float YvCRllSZic51758037 = -672285327;    float YvCRllSZic67489660 = -816732079;    float YvCRllSZic73415375 = -447510778;    float YvCRllSZic5715593 = -449429771;    float YvCRllSZic69507262 = 26167712;    float YvCRllSZic21155888 = -460752782;    float YvCRllSZic74828172 = -98540156;    float YvCRllSZic98375167 = 865479;    float YvCRllSZic28283386 = -766629046;    float YvCRllSZic77524415 = 33950521;    float YvCRllSZic42769047 = -465636231;    float YvCRllSZic13406642 = 6966042;     YvCRllSZic99805392 = YvCRllSZic30293182;     YvCRllSZic30293182 = YvCRllSZic63516304;     YvCRllSZic63516304 = YvCRllSZic52757018;     YvCRllSZic52757018 = YvCRllSZic24644487;     YvCRllSZic24644487 = YvCRllSZic6556008;     YvCRllSZic6556008 = YvCRllSZic45337240;     YvCRllSZic45337240 = YvCRllSZic62392198;     YvCRllSZic62392198 = YvCRllSZic15883637;     YvCRllSZic15883637 = YvCRllSZic92302833;     YvCRllSZic92302833 = YvCRllSZic855858;     YvCRllSZic855858 = YvCRllSZic31419582;     YvCRllSZic31419582 = YvCRllSZic88685402;     YvCRllSZic88685402 = YvCRllSZic57492595;     YvCRllSZic57492595 = YvCRllSZic12343090;     YvCRllSZic12343090 = YvCRllSZic54710617;     YvCRllSZic54710617 = YvCRllSZic68153118;     YvCRllSZic68153118 = YvCRllSZic8654712;     YvCRllSZic8654712 = YvCRllSZic95772293;     YvCRllSZic95772293 = YvCRllSZic74193895;     YvCRllSZic74193895 = YvCRllSZic42371586;     YvCRllSZic42371586 = YvCRllSZic85385425;     YvCRllSZic85385425 = YvCRllSZic87604983;     YvCRllSZic87604983 = YvCRllSZic17006506;     YvCRllSZic17006506 = YvCRllSZic20156632;     YvCRllSZic20156632 = YvCRllSZic49884068;     YvCRllSZic49884068 = YvCRllSZic29984553;     YvCRllSZic29984553 = YvCRllSZic41335800;     YvCRllSZic41335800 = YvCRllSZic64014628;     YvCRllSZic64014628 = YvCRllSZic72087052;     YvCRllSZic72087052 = YvCRllSZic4106002;     YvCRllSZic4106002 = YvCRllSZic39028652;     YvCRllSZic39028652 = YvCRllSZic4843811;     YvCRllSZic4843811 = YvCRllSZic12772046;     YvCRllSZic12772046 = YvCRllSZic81909627;     YvCRllSZic81909627 = YvCRllSZic16103574;     YvCRllSZic16103574 = YvCRllSZic52225390;     YvCRllSZic52225390 = YvCRllSZic2107650;     YvCRllSZic2107650 = YvCRllSZic95916306;     YvCRllSZic95916306 = YvCRllSZic51399627;     YvCRllSZic51399627 = YvCRllSZic2376608;     YvCRllSZic2376608 = YvCRllSZic26660956;     YvCRllSZic26660956 = YvCRllSZic21320999;     YvCRllSZic21320999 = YvCRllSZic30965879;     YvCRllSZic30965879 = YvCRllSZic75233846;     YvCRllSZic75233846 = YvCRllSZic18118139;     YvCRllSZic18118139 = YvCRllSZic58546603;     YvCRllSZic58546603 = YvCRllSZic13063986;     YvCRllSZic13063986 = YvCRllSZic21116999;     YvCRllSZic21116999 = YvCRllSZic5577414;     YvCRllSZic5577414 = YvCRllSZic72921767;     YvCRllSZic72921767 = YvCRllSZic76725853;     YvCRllSZic76725853 = YvCRllSZic92034312;     YvCRllSZic92034312 = YvCRllSZic75975002;     YvCRllSZic75975002 = YvCRllSZic61014008;     YvCRllSZic61014008 = YvCRllSZic14419968;     YvCRllSZic14419968 = YvCRllSZic42688199;     YvCRllSZic42688199 = YvCRllSZic46509798;     YvCRllSZic46509798 = YvCRllSZic32600387;     YvCRllSZic32600387 = YvCRllSZic74760418;     YvCRllSZic74760418 = YvCRllSZic76571454;     YvCRllSZic76571454 = YvCRllSZic4001440;     YvCRllSZic4001440 = YvCRllSZic98377569;     YvCRllSZic98377569 = YvCRllSZic43796584;     YvCRllSZic43796584 = YvCRllSZic88196832;     YvCRllSZic88196832 = YvCRllSZic61827205;     YvCRllSZic61827205 = YvCRllSZic26575772;     YvCRllSZic26575772 = YvCRllSZic75913356;     YvCRllSZic75913356 = YvCRllSZic75582968;     YvCRllSZic75582968 = YvCRllSZic96239515;     YvCRllSZic96239515 = YvCRllSZic2485227;     YvCRllSZic2485227 = YvCRllSZic66045469;     YvCRllSZic66045469 = YvCRllSZic12738405;     YvCRllSZic12738405 = YvCRllSZic44372667;     YvCRllSZic44372667 = YvCRllSZic71817288;     YvCRllSZic71817288 = YvCRllSZic15710630;     YvCRllSZic15710630 = YvCRllSZic64064426;     YvCRllSZic64064426 = YvCRllSZic56639105;     YvCRllSZic56639105 = YvCRllSZic41772660;     YvCRllSZic41772660 = YvCRllSZic2038493;     YvCRllSZic2038493 = YvCRllSZic91337465;     YvCRllSZic91337465 = YvCRllSZic16920567;     YvCRllSZic16920567 = YvCRllSZic20218801;     YvCRllSZic20218801 = YvCRllSZic58437214;     YvCRllSZic58437214 = YvCRllSZic99165285;     YvCRllSZic99165285 = YvCRllSZic27380149;     YvCRllSZic27380149 = YvCRllSZic46994340;     YvCRllSZic46994340 = YvCRllSZic28868809;     YvCRllSZic28868809 = YvCRllSZic51758037;     YvCRllSZic51758037 = YvCRllSZic67489660;     YvCRllSZic67489660 = YvCRllSZic73415375;     YvCRllSZic73415375 = YvCRllSZic5715593;     YvCRllSZic5715593 = YvCRllSZic69507262;     YvCRllSZic69507262 = YvCRllSZic21155888;     YvCRllSZic21155888 = YvCRllSZic74828172;     YvCRllSZic74828172 = YvCRllSZic98375167;     YvCRllSZic98375167 = YvCRllSZic28283386;     YvCRllSZic28283386 = YvCRllSZic77524415;     YvCRllSZic77524415 = YvCRllSZic42769047;     YvCRllSZic42769047 = YvCRllSZic13406642;     YvCRllSZic13406642 = YvCRllSZic99805392;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void zLNxkXTLHo36978200() {     double QKBuhEQYhy34705936 = -420598083;    double QKBuhEQYhy82113248 = -363046568;    double QKBuhEQYhy16547561 = -103810952;    double QKBuhEQYhy56268970 = -925334569;    double QKBuhEQYhy34049731 = -989423377;    double QKBuhEQYhy17108646 = -538210581;    double QKBuhEQYhy24312156 = -677658161;    double QKBuhEQYhy66538479 = 46637094;    double QKBuhEQYhy34021587 = -730549866;    double QKBuhEQYhy80085673 = -657020032;    double QKBuhEQYhy52091396 = -947758008;    double QKBuhEQYhy77974987 = -34538680;    double QKBuhEQYhy61047895 = -297561976;    double QKBuhEQYhy81140953 = -617397557;    double QKBuhEQYhy83888015 = -922423867;    double QKBuhEQYhy67402184 = -807555196;    double QKBuhEQYhy11762003 = -894295852;    double QKBuhEQYhy41963257 = -806990764;    double QKBuhEQYhy39249946 = -145670399;    double QKBuhEQYhy98550710 = -384163703;    double QKBuhEQYhy32060202 = -524238179;    double QKBuhEQYhy11097445 = -167852670;    double QKBuhEQYhy27430048 = -392912591;    double QKBuhEQYhy77927177 = -286983703;    double QKBuhEQYhy38122313 = -635563640;    double QKBuhEQYhy45281022 = -699196207;    double QKBuhEQYhy46130729 = -312781706;    double QKBuhEQYhy76329075 = -729391268;    double QKBuhEQYhy39008748 = -585723870;    double QKBuhEQYhy38190492 = 92869203;    double QKBuhEQYhy96151014 = -527599168;    double QKBuhEQYhy53477948 = -753027009;    double QKBuhEQYhy75890192 = -6341521;    double QKBuhEQYhy55388064 = -227678658;    double QKBuhEQYhy69964891 = -662391137;    double QKBuhEQYhy52720319 = -738073768;    double QKBuhEQYhy39338384 = -207876768;    double QKBuhEQYhy33717134 = -652555695;    double QKBuhEQYhy3065586 = -900758823;    double QKBuhEQYhy36283198 = -161976979;    double QKBuhEQYhy73591022 = 85103105;    double QKBuhEQYhy58322201 = -337650988;    double QKBuhEQYhy92238224 = -978664722;    double QKBuhEQYhy60516058 = -787809238;    double QKBuhEQYhy3164798 = -542321603;    double QKBuhEQYhy54757696 = -887053019;    double QKBuhEQYhy63564688 = -594540017;    double QKBuhEQYhy92647794 = -111327192;    double QKBuhEQYhy37955463 = -920310488;    double QKBuhEQYhy29025312 = -474086711;    double QKBuhEQYhy20025724 = -824016787;    double QKBuhEQYhy68201474 = -854292061;    double QKBuhEQYhy98115574 = -793851177;    double QKBuhEQYhy49455155 = -773440895;    double QKBuhEQYhy4917998 = -325696980;    double QKBuhEQYhy23608492 = -152745413;    double QKBuhEQYhy54683200 = -970133977;    double QKBuhEQYhy38620383 = -816827250;    double QKBuhEQYhy18146657 = -189770930;    double QKBuhEQYhy88768709 = -190227170;    double QKBuhEQYhy70977917 = -125428875;    double QKBuhEQYhy47983081 = -948266893;    double QKBuhEQYhy27529731 = -367639036;    double QKBuhEQYhy95831094 = -723419069;    double QKBuhEQYhy83934658 = -29420864;    double QKBuhEQYhy98613448 = -94731000;    double QKBuhEQYhy2084795 = 71802841;    double QKBuhEQYhy5659831 = 30116681;    double QKBuhEQYhy11176063 = -955006421;    double QKBuhEQYhy31167697 = -84350100;    double QKBuhEQYhy28063800 = -499678428;    double QKBuhEQYhy78044868 = -141740158;    double QKBuhEQYhy38897671 = -906231941;    double QKBuhEQYhy2966748 = -983693420;    double QKBuhEQYhy24959688 = -369266809;    double QKBuhEQYhy73738001 = -86587192;    double QKBuhEQYhy18859220 = -189187949;    double QKBuhEQYhy66913989 = -605103354;    double QKBuhEQYhy74762380 = -744662101;    double QKBuhEQYhy83364616 = -748510622;    double QKBuhEQYhy81716333 = -4656191;    double QKBuhEQYhy53482935 = -101454515;    double QKBuhEQYhy38373613 = -809080781;    double QKBuhEQYhy9983437 = -11637160;    double QKBuhEQYhy18164768 = -83114011;    double QKBuhEQYhy27949541 = -673307108;    double QKBuhEQYhy55362373 = -959175832;    double QKBuhEQYhy26435038 = -232900626;    double QKBuhEQYhy50470067 = -901981678;    double QKBuhEQYhy46356399 = -409645725;    double QKBuhEQYhy98037118 = -767939792;    double QKBuhEQYhy718001 = -391049519;    double QKBuhEQYhy15570478 = -362784765;    double QKBuhEQYhy14296877 = -610531654;    double QKBuhEQYhy65305281 = 63451896;    double QKBuhEQYhy25607942 = 33369997;    double QKBuhEQYhy30792471 = -970011952;    double QKBuhEQYhy96407129 = -155245654;    double QKBuhEQYhy76581400 = -658388374;    double QKBuhEQYhy4551350 = -420598083;     QKBuhEQYhy34705936 = QKBuhEQYhy82113248;     QKBuhEQYhy82113248 = QKBuhEQYhy16547561;     QKBuhEQYhy16547561 = QKBuhEQYhy56268970;     QKBuhEQYhy56268970 = QKBuhEQYhy34049731;     QKBuhEQYhy34049731 = QKBuhEQYhy17108646;     QKBuhEQYhy17108646 = QKBuhEQYhy24312156;     QKBuhEQYhy24312156 = QKBuhEQYhy66538479;     QKBuhEQYhy66538479 = QKBuhEQYhy34021587;     QKBuhEQYhy34021587 = QKBuhEQYhy80085673;     QKBuhEQYhy80085673 = QKBuhEQYhy52091396;     QKBuhEQYhy52091396 = QKBuhEQYhy77974987;     QKBuhEQYhy77974987 = QKBuhEQYhy61047895;     QKBuhEQYhy61047895 = QKBuhEQYhy81140953;     QKBuhEQYhy81140953 = QKBuhEQYhy83888015;     QKBuhEQYhy83888015 = QKBuhEQYhy67402184;     QKBuhEQYhy67402184 = QKBuhEQYhy11762003;     QKBuhEQYhy11762003 = QKBuhEQYhy41963257;     QKBuhEQYhy41963257 = QKBuhEQYhy39249946;     QKBuhEQYhy39249946 = QKBuhEQYhy98550710;     QKBuhEQYhy98550710 = QKBuhEQYhy32060202;     QKBuhEQYhy32060202 = QKBuhEQYhy11097445;     QKBuhEQYhy11097445 = QKBuhEQYhy27430048;     QKBuhEQYhy27430048 = QKBuhEQYhy77927177;     QKBuhEQYhy77927177 = QKBuhEQYhy38122313;     QKBuhEQYhy38122313 = QKBuhEQYhy45281022;     QKBuhEQYhy45281022 = QKBuhEQYhy46130729;     QKBuhEQYhy46130729 = QKBuhEQYhy76329075;     QKBuhEQYhy76329075 = QKBuhEQYhy39008748;     QKBuhEQYhy39008748 = QKBuhEQYhy38190492;     QKBuhEQYhy38190492 = QKBuhEQYhy96151014;     QKBuhEQYhy96151014 = QKBuhEQYhy53477948;     QKBuhEQYhy53477948 = QKBuhEQYhy75890192;     QKBuhEQYhy75890192 = QKBuhEQYhy55388064;     QKBuhEQYhy55388064 = QKBuhEQYhy69964891;     QKBuhEQYhy69964891 = QKBuhEQYhy52720319;     QKBuhEQYhy52720319 = QKBuhEQYhy39338384;     QKBuhEQYhy39338384 = QKBuhEQYhy33717134;     QKBuhEQYhy33717134 = QKBuhEQYhy3065586;     QKBuhEQYhy3065586 = QKBuhEQYhy36283198;     QKBuhEQYhy36283198 = QKBuhEQYhy73591022;     QKBuhEQYhy73591022 = QKBuhEQYhy58322201;     QKBuhEQYhy58322201 = QKBuhEQYhy92238224;     QKBuhEQYhy92238224 = QKBuhEQYhy60516058;     QKBuhEQYhy60516058 = QKBuhEQYhy3164798;     QKBuhEQYhy3164798 = QKBuhEQYhy54757696;     QKBuhEQYhy54757696 = QKBuhEQYhy63564688;     QKBuhEQYhy63564688 = QKBuhEQYhy92647794;     QKBuhEQYhy92647794 = QKBuhEQYhy37955463;     QKBuhEQYhy37955463 = QKBuhEQYhy29025312;     QKBuhEQYhy29025312 = QKBuhEQYhy20025724;     QKBuhEQYhy20025724 = QKBuhEQYhy68201474;     QKBuhEQYhy68201474 = QKBuhEQYhy98115574;     QKBuhEQYhy98115574 = QKBuhEQYhy49455155;     QKBuhEQYhy49455155 = QKBuhEQYhy4917998;     QKBuhEQYhy4917998 = QKBuhEQYhy23608492;     QKBuhEQYhy23608492 = QKBuhEQYhy54683200;     QKBuhEQYhy54683200 = QKBuhEQYhy38620383;     QKBuhEQYhy38620383 = QKBuhEQYhy18146657;     QKBuhEQYhy18146657 = QKBuhEQYhy88768709;     QKBuhEQYhy88768709 = QKBuhEQYhy70977917;     QKBuhEQYhy70977917 = QKBuhEQYhy47983081;     QKBuhEQYhy47983081 = QKBuhEQYhy27529731;     QKBuhEQYhy27529731 = QKBuhEQYhy95831094;     QKBuhEQYhy95831094 = QKBuhEQYhy83934658;     QKBuhEQYhy83934658 = QKBuhEQYhy98613448;     QKBuhEQYhy98613448 = QKBuhEQYhy2084795;     QKBuhEQYhy2084795 = QKBuhEQYhy5659831;     QKBuhEQYhy5659831 = QKBuhEQYhy11176063;     QKBuhEQYhy11176063 = QKBuhEQYhy31167697;     QKBuhEQYhy31167697 = QKBuhEQYhy28063800;     QKBuhEQYhy28063800 = QKBuhEQYhy78044868;     QKBuhEQYhy78044868 = QKBuhEQYhy38897671;     QKBuhEQYhy38897671 = QKBuhEQYhy2966748;     QKBuhEQYhy2966748 = QKBuhEQYhy24959688;     QKBuhEQYhy24959688 = QKBuhEQYhy73738001;     QKBuhEQYhy73738001 = QKBuhEQYhy18859220;     QKBuhEQYhy18859220 = QKBuhEQYhy66913989;     QKBuhEQYhy66913989 = QKBuhEQYhy74762380;     QKBuhEQYhy74762380 = QKBuhEQYhy83364616;     QKBuhEQYhy83364616 = QKBuhEQYhy81716333;     QKBuhEQYhy81716333 = QKBuhEQYhy53482935;     QKBuhEQYhy53482935 = QKBuhEQYhy38373613;     QKBuhEQYhy38373613 = QKBuhEQYhy9983437;     QKBuhEQYhy9983437 = QKBuhEQYhy18164768;     QKBuhEQYhy18164768 = QKBuhEQYhy27949541;     QKBuhEQYhy27949541 = QKBuhEQYhy55362373;     QKBuhEQYhy55362373 = QKBuhEQYhy26435038;     QKBuhEQYhy26435038 = QKBuhEQYhy50470067;     QKBuhEQYhy50470067 = QKBuhEQYhy46356399;     QKBuhEQYhy46356399 = QKBuhEQYhy98037118;     QKBuhEQYhy98037118 = QKBuhEQYhy718001;     QKBuhEQYhy718001 = QKBuhEQYhy15570478;     QKBuhEQYhy15570478 = QKBuhEQYhy14296877;     QKBuhEQYhy14296877 = QKBuhEQYhy65305281;     QKBuhEQYhy65305281 = QKBuhEQYhy25607942;     QKBuhEQYhy25607942 = QKBuhEQYhy30792471;     QKBuhEQYhy30792471 = QKBuhEQYhy96407129;     QKBuhEQYhy96407129 = QKBuhEQYhy76581400;     QKBuhEQYhy76581400 = QKBuhEQYhy4551350;     QKBuhEQYhy4551350 = QKBuhEQYhy34705936;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void hexkZOIFgAQLDqqGejLKQGewOqnTRTh36796742() {     double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13796404 = -668353931;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR95465567 = 6013863;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR48015002 = -878170318;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR61402338 = -797135649;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR28283866 = -686234717;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR70979014 = -222510416;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR30105404 = -58093076;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR40823399 = -122313392;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR94066799 = -617764249;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10833864 = -435757567;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR72746878 = -880225287;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR39490256 = -675979398;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR28966254 = -868959007;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR75903192 = -784220665;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR22942341 = -104958442;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR96014031 = -893691186;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR66201575 = -732148981;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12775120 = -418003915;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR89931263 = -744469666;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98754739 = -817263784;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18794484 = -166603025;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR99915457 = -315975060;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90655029 = -60515170;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33575584 = -279847083;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR50886977 = -234730502;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR68279056 = -858555923;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR93067674 = -874273362;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR59024202 = -745993891;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR6827682 = -685851123;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR87537797 = -44929104;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR83890135 = -465221619;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98531332 = -861419604;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR9651979 = -43033693;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR30780400 = -497375191;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR62157846 = -331756438;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR29091462 = -41002508;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98797448 = -371113758;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR32643621 = -688822152;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24844233 = -421808481;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR69641807 = -861200899;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR91505657 = -728262122;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR52086992 = -794712578;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR96923120 = -638039644;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR15300911 = -323682873;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR31048983 = -588728825;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR59987920 = 43069566;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR89641659 = -203102662;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24820772 = -972368473;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12293093 = -81807911;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR25740700 = -97273674;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR4238826 = 56817990;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR73558789 = -282992061;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR79148 = -261195029;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR41997787 = -210807217;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18002259 = -108229175;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13880947 = -252378872;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR4810538 = -933470967;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR14439419 = -498323236;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10515361 = -462405148;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR60004810 = -827678795;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR77911340 = -348237055;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR71081202 = -312099186;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33995717 = -436462270;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR6529002 = -472835146;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR26943729 = -970535948;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR74215546 = 81194316;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR29838277 = -532945706;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98185853 = -271583817;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13745346 = -352464227;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR93850878 = 36044065;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR97216582 = -422577429;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33557955 = 56673171;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR87930887 = -996195435;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR20289457 = -883268768;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR7249083 = 10998338;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR66707491 = -371890448;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR2992337 = -677935416;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR75354119 = -736832297;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR2526601 = -691118259;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90899057 = -177800068;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR78637396 = -555453261;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR68246902 = -901904889;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR46731109 = -564185981;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR81086982 = -488577449;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR83298971 = -1747094;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10331346 = -82229558;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98452184 = -500224576;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR67654191 = -832226477;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12778142 = -289146017;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR48276899 = 20622433;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24280925 = -107531541;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR84358030 = -872790523;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR22128261 = -126417005;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR64839422 = -594129687;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR91730467 = -412963845;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR20424456 = -316162937;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18091275 = -258250308;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90394118 = -65204499;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR88357182 = -353146925;    double wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR56833436 = -668353931;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13796404 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR95465567;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR95465567 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR48015002;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR48015002 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR61402338;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR61402338 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR28283866;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR28283866 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR70979014;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR70979014 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR30105404;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR30105404 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR40823399;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR40823399 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR94066799;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR94066799 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10833864;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10833864 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR72746878;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR72746878 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR39490256;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR39490256 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR28966254;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR28966254 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR75903192;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR75903192 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR22942341;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR22942341 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR96014031;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR96014031 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR66201575;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR66201575 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12775120;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12775120 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR89931263;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR89931263 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98754739;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98754739 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18794484;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18794484 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR99915457;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR99915457 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90655029;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90655029 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33575584;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33575584 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR50886977;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR50886977 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR68279056;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR68279056 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR93067674;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR93067674 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR59024202;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR59024202 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR6827682;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR6827682 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR87537797;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR87537797 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR83890135;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR83890135 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98531332;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98531332 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR9651979;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR9651979 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR30780400;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR30780400 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR62157846;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR62157846 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR29091462;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR29091462 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98797448;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98797448 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR32643621;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR32643621 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24844233;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24844233 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR69641807;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR69641807 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR91505657;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR91505657 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR52086992;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR52086992 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR96923120;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR96923120 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR15300911;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR15300911 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR31048983;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR31048983 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR59987920;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR59987920 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR89641659;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR89641659 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24820772;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24820772 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12293093;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12293093 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR25740700;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR25740700 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR4238826;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR4238826 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR73558789;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR73558789 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR79148;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR79148 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR41997787;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR41997787 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18002259;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18002259 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13880947;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13880947 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR4810538;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR4810538 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR14439419;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR14439419 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10515361;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10515361 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR60004810;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR60004810 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR77911340;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR77911340 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR71081202;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR71081202 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33995717;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33995717 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR6529002;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR6529002 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR26943729;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR26943729 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR74215546;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR74215546 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR29838277;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR29838277 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98185853;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98185853 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13745346;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13745346 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR93850878;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR93850878 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR97216582;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR97216582 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33557955;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR33557955 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR87930887;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR87930887 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR20289457;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR20289457 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR7249083;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR7249083 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR66707491;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR66707491 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR2992337;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR2992337 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR75354119;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR75354119 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR2526601;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR2526601 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90899057;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90899057 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR78637396;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR78637396 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR68246902;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR68246902 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR46731109;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR46731109 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR81086982;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR81086982 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR83298971;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR83298971 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10331346;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR10331346 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98452184;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR98452184 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR67654191;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR67654191 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12778142;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR12778142 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR48276899;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR48276899 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24280925;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR24280925 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR84358030;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR84358030 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR22128261;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR22128261 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR64839422;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR64839422 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR91730467;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR91730467 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR20424456;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR20424456 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18091275;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR18091275 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90394118;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR90394118 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR88357182;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR88357182 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR56833436;     wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR56833436 = wuJDlrkgBsySpWutgjufJrYlbABVjbonMTTR13796404;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LHZNNjHuxFilnNRinsCvglgrvgZttia64833102() {     double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg74561437 = 9692455;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg24408783 = -325745856;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26103044 = -271516466;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg44991921 = -814527521;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg80471127 = -247675149;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg11376046 = -315590299;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg80242409 = -203487193;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg88972443 = -613932855;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg29223870 = -146296355;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31335022 = -680341185;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31789069 = -242491518;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg48369689 = 57711948;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg10845602 = -34048875;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5857520 = -173715862;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg94080949 = -665255092;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg1851509 = -829645944;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15722102 = -541908492;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg425393 = -544162647;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg73409976 = -954032577;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg69013223 = 68442699;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21191489 = -300640005;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15985336 = -900340767;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg99594049 = -852733920;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg62646434 = -82113410;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg18781933 = -307533840;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg50815755 = -649125959;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63738958 = -530579556;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg60783852 = -328941693;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg25536061 = -834549018;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26678713 = -928589313;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg61250892 = -214508317;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg11857423 = -274184710;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg72471227 = -694108893;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg20549695 = -905719498;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg40494693 = -347666728;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg27972107 = -582789958;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66705239 = -274829626;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg49628752 = -329519862;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg84537732 = -410361995;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg53003033 = -602380407;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg40989950 = -994030073;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg6609769 = -766179025;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26273642 = -18479800;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg70934949 = -473669920;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg87003563 = -856403978;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg67688685 = -922980836;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg51591738 = -830216205;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31588603 = -468677734;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg82969977 = -731038949;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3539438 = -11328663;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28312414 = -121246191;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg88297915 = -980930112;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg55503957 = -784954082;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5632696 = -959083698;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg43641613 = -961568916;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg58576101 = -89966778;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg24814733 = -473011937;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63456610 = -89403057;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26209988 = -406993682;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg29655372 = -598549190;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg47637088 = -785010743;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg19458557 = -874545500;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63436383 = -779383838;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg2545157 = -217707043;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg70084130 = -365832868;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg19931647 = -968306809;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg75898461 = -248179159;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg90295907 = -128329377;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg65362827 = -826049135;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66108843 = 17534866;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg35146269 = -454816318;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66093350 = -112388630;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15887661 = -33800652;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg20406944 = -251652171;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28023273 = 62472772;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg14581721 = -534460981;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg89711693 = -781860967;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28659101 = -279064000;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg75642871 = -225709432;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg51093248 = -384553004;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg99224016 = -818909754;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg32150356 = 38098177;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg77813874 = -597902745;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21996624 = -723220355;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg98366299 = -707343123;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg72952976 = -233578206;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg56353465 = -489230628;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66838531 = -735025196;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg76908081 = -944150583;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg81918591 = -157699950;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3157374 = -9778021;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3248630 = -85426570;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg23418764 = -922526181;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg54882360 = -811812805;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5365945 = -817369665;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21531393 = -19484574;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg43173386 = -986795187;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg23728486 = -800772758;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg850819 = -7837053;    double TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg67071916 = 9692455;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg74561437 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg24408783;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg24408783 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26103044;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26103044 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg44991921;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg44991921 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg80471127;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg80471127 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg11376046;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg11376046 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg80242409;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg80242409 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg88972443;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg88972443 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg29223870;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg29223870 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31335022;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31335022 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31789069;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31789069 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg48369689;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg48369689 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg10845602;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg10845602 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5857520;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5857520 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg94080949;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg94080949 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg1851509;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg1851509 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15722102;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15722102 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg425393;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg425393 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg73409976;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg73409976 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg69013223;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg69013223 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21191489;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21191489 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15985336;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15985336 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg99594049;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg99594049 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg62646434;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg62646434 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg18781933;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg18781933 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg50815755;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg50815755 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63738958;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63738958 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg60783852;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg60783852 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg25536061;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg25536061 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26678713;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26678713 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg61250892;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg61250892 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg11857423;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg11857423 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg72471227;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg72471227 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg20549695;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg20549695 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg40494693;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg40494693 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg27972107;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg27972107 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66705239;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66705239 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg49628752;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg49628752 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg84537732;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg84537732 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg53003033;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg53003033 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg40989950;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg40989950 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg6609769;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg6609769 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26273642;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26273642 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg70934949;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg70934949 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg87003563;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg87003563 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg67688685;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg67688685 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg51591738;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg51591738 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31588603;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg31588603 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg82969977;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg82969977 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3539438;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3539438 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28312414;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28312414 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg88297915;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg88297915 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg55503957;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg55503957 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5632696;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5632696 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg43641613;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg43641613 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg58576101;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg58576101 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg24814733;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg24814733 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63456610;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63456610 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26209988;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg26209988 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg29655372;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg29655372 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg47637088;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg47637088 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg19458557;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg19458557 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63436383;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg63436383 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg2545157;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg2545157 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg70084130;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg70084130 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg19931647;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg19931647 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg75898461;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg75898461 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg90295907;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg90295907 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg65362827;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg65362827 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66108843;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66108843 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg35146269;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg35146269 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66093350;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66093350 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15887661;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg15887661 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg20406944;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg20406944 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28023273;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28023273 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg14581721;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg14581721 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg89711693;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg89711693 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28659101;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg28659101 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg75642871;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg75642871 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg51093248;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg51093248 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg99224016;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg99224016 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg32150356;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg32150356 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg77813874;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg77813874 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21996624;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21996624 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg98366299;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg98366299 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg72952976;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg72952976 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg56353465;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg56353465 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66838531;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg66838531 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg76908081;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg76908081 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg81918591;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg81918591 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3157374;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3157374 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3248630;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg3248630 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg23418764;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg23418764 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg54882360;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg54882360 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5365945;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg5365945 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21531393;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg21531393 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg43173386;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg43173386 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg23728486;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg23728486 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg850819;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg850819 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg67071916;     TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg67071916 = TEZiBXWhvxoFJPFJgoJDckljZbhShJNBPfPg74561437;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kEyKvDZSvudBBKCoETJTNlAOAJYlQTQ32236607() {     double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN99861030 = -918508398;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN78652236 = -819549089;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN68117821 = 43675847;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN41438010 = -831309155;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN85213221 = -171872049;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN50355638 = -945755098;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN88269344 = -208692042;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44203979 = -65495480;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45603498 = -143123;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33572982 = -897044676;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72970130 = -862222091;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN70972650 = -179954287;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN86343217 = -868784712;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN36515204 = 67999299;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN52197151 = -684839560;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN98712231 = -362584746;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN65259451 = -744308020;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN11316007 = -993964928;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN76766628 = -365014335;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17508251 = -331314203;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN90171055 = -487868665;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN6929956 = -808062064;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN74886086 = -266278327;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN41574448 = -354475655;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN7101628 = -783045820;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN39228360 = -659325121;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44211250 = -681401323;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN57218602 = -351084304;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN10254673 = 44777560;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN36376088 = -353173726;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN53441096 = -898907763;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72084350 = -305800165;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN8524888 = -299532335;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80853399 = -26051724;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN35381124 = -671790695;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN18120097 = -989777867;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN97142580 = -413502832;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN85316159 = 17175334;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN49154267 = -360720662;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN43965619 = -487729055;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44878302 = -478543008;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN57465079 = -236892284;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26524145 = -404869426;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45669547 = -676289001;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26959738 = -670827373;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN69856090 = -562152295;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN21894447 = -354624010;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN96013702 = -407221759;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN49412586 = -180296993;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN76854010 = -121381717;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN84874647 = -3588821;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34098828 = -747361569;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17755966 = -344721587;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN63526028 = -947771530;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN42065552 = -125317794;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN92931074 = -10446335;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN3766150 = -453270762;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26543373 = -601848499;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34336383 = 51736665;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45984862 = -512546928;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN6144389 = -164353775;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN31050742 = -857607739;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33949306 = -10273040;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN9227411 = -646969398;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80131886 = -998136913;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN885780 = -456421926;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN62447762 = -880421952;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN5489818 = -742732988;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN1134081 = -260210007;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34077055 = -695061693;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN1569651 = -949081914;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN79943292 = -661483354;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN62161740 = -533244266;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN32801009 = -877285280;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72629948 = -852771195;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN32705977 = -150976382;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80405810 = -303192638;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN29216540 = -589989327;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN14614711 = -683648282;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN37245537 = -120893526;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17333914 = -204701111;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN48197547 = -174179565;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN7806017 = -70787311;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33400663 = -833840724;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN51501441 = -249584905;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN19342269 = -51546194;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN54328385 = -961078579;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44998859 = -351760806;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN38787848 = -900733931;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN42450049 = -561344360;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN14353948 = -436507106;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN70599207 = -811654334;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN50979776 = 65438668;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN3169405 = -848173734;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN37821231 = -223375280;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN13827560 = -620935269;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN23515773 = -126619244;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17296735 = -757900028;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN65537661 = -678152088;    double QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26073959 = -918508398;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN99861030 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN78652236;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN78652236 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN68117821;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN68117821 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN41438010;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN41438010 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN85213221;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN85213221 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN50355638;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN50355638 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN88269344;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN88269344 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44203979;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44203979 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45603498;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45603498 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33572982;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33572982 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72970130;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72970130 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN70972650;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN70972650 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN86343217;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN86343217 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN36515204;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN36515204 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN52197151;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN52197151 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN98712231;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN98712231 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN65259451;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN65259451 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN11316007;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN11316007 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN76766628;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN76766628 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17508251;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17508251 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN90171055;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN90171055 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN6929956;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN6929956 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN74886086;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN74886086 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN41574448;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN41574448 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN7101628;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN7101628 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN39228360;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN39228360 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44211250;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44211250 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN57218602;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN57218602 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN10254673;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN10254673 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN36376088;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN36376088 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN53441096;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN53441096 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72084350;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72084350 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN8524888;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN8524888 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80853399;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80853399 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN35381124;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN35381124 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN18120097;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN18120097 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN97142580;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN97142580 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN85316159;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN85316159 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN49154267;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN49154267 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN43965619;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN43965619 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44878302;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44878302 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN57465079;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN57465079 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26524145;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26524145 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45669547;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45669547 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26959738;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26959738 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN69856090;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN69856090 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN21894447;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN21894447 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN96013702;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN96013702 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN49412586;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN49412586 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN76854010;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN76854010 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN84874647;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN84874647 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34098828;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34098828 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17755966;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17755966 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN63526028;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN63526028 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN42065552;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN42065552 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN92931074;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN92931074 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN3766150;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN3766150 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26543373;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26543373 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34336383;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34336383 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45984862;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN45984862 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN6144389;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN6144389 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN31050742;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN31050742 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33949306;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33949306 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN9227411;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN9227411 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80131886;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80131886 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN885780;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN885780 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN62447762;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN62447762 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN5489818;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN5489818 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN1134081;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN1134081 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34077055;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN34077055 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN1569651;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN1569651 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN79943292;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN79943292 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN62161740;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN62161740 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN32801009;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN32801009 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72629948;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN72629948 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN32705977;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN32705977 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80405810;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN80405810 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN29216540;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN29216540 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN14614711;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN14614711 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN37245537;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN37245537 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17333914;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17333914 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN48197547;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN48197547 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN7806017;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN7806017 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33400663;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN33400663 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN51501441;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN51501441 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN19342269;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN19342269 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN54328385;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN54328385 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44998859;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN44998859 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN38787848;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN38787848 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN42450049;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN42450049 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN14353948;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN14353948 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN70599207;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN70599207 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN50979776;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN50979776 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN3169405;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN3169405 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN37821231;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN37821231 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN13827560;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN13827560 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN23515773;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN23515773 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17296735;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN17296735 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN65537661;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN65537661 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26073959;     QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN26073959 = QoNUmVkFAjfMEeevclZBqnaJyEjKlfnCbeMN99861030;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MARfWkCctCDAXTdXwBafwapYDQvYfAL99640111() {     double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY25160623 = -746709256;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY32895689 = -213352333;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10132600 = -741131840;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37884098 = -848090782;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89955316 = -96068965;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89335229 = -475919897;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY96296279 = -213896892;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY99435513 = -617058135;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY61983127 = -953989894;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY35810942 = -13748167;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY14151191 = -381952666;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY93575611 = -417620540;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY61840833 = -603520549;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY67172888 = -790285542;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10313354 = -704424065;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY95572954 = -995523547;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY14796802 = -946707548;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY22206621 = -343767217;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY80123281 = -875996091;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY66003278 = -731071105;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY59150622 = -675097335;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY97874574 = -715783360;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50178123 = -779822735;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20502463 = -626837900;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY95421321 = -158557825;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27640965 = -669524274;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY24683541 = -832223090;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY53653353 = -373226926;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY94973284 = -175895832;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY46073463 = -877758138;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY45631300 = -483307209;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY32311279 = -337415615;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY44578548 = 95044229;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY41157104 = -246383951;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY30267555 = -995914655;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8268088 = -296765740;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27579922 = -552176039;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY21003566 = -736129477;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY13770801 = -311079304;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY34928205 = -373077703;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY48766654 = 36944057;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8320390 = -807605502;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY26774648 = -791259048;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20404145 = -878908081;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY66915912 = -485250766;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY72023495 = -201323717;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY92197153 = -979031815;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY60438803 = -345765781;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY15855195 = -729554988;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50168582 = -231434781;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY41436881 = -985931451;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY79899739 = -513793018;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY80007973 = 95510902;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY21419361 = -936459363;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY40489490 = -389066662;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27286049 = 69074104;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY82717565 = -433529599;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89630136 = -14293941;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY42462777 = -589532957;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY62314351 = -426544691;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY64651689 = -643696808;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY42642927 = -840669966;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY4462229 = -341162303;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY15909665 = 23768244;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY90179642 = -530440958;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY81839911 = 55462949;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY48997064 = -412664770;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20683729 = -257136599;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY36905334 = -794370887;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY2045266 = -307658326;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY67993033 = -343347509;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY93793235 = -110578072;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8435821 = 67312087;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY45195076 = -402918388;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY17236624 = -668015163;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50830232 = -867491834;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY71099927 = -924524313;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY29773979 = -900914654;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY53586550 = -41587134;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY23397827 = -957234109;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY35443812 = -690492459;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY64244738 = -386457309;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37798158 = -643671939;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY44804703 = -944461051;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY4636582 = -891826687;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY65731560 = -969514191;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY52303306 = -332926518;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY23159187 = 31503591;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY667614 = -857317289;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY2981507 = -964988760;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY25550522 = -863236141;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37949785 = -437882098;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY78540789 = -46596520;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY51456450 = -884534614;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY70276515 = -729380896;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY6123728 = -122385978;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY3858161 = -366443199;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10864983 = -715027293;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY30224503 = -248467123;    double ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY85076000 = -746709256;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY25160623 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY32895689;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY32895689 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10132600;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10132600 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37884098;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37884098 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89955316;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89955316 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89335229;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89335229 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY96296279;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY96296279 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY99435513;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY99435513 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY61983127;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY61983127 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY35810942;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY35810942 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY14151191;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY14151191 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY93575611;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY93575611 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY61840833;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY61840833 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY67172888;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY67172888 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10313354;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10313354 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY95572954;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY95572954 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY14796802;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY14796802 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY22206621;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY22206621 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY80123281;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY80123281 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY66003278;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY66003278 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY59150622;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY59150622 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY97874574;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY97874574 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50178123;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50178123 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20502463;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20502463 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY95421321;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY95421321 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27640965;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27640965 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY24683541;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY24683541 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY53653353;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY53653353 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY94973284;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY94973284 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY46073463;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY46073463 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY45631300;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY45631300 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY32311279;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY32311279 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY44578548;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY44578548 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY41157104;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY41157104 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY30267555;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY30267555 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8268088;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8268088 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27579922;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27579922 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY21003566;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY21003566 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY13770801;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY13770801 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY34928205;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY34928205 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY48766654;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY48766654 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8320390;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8320390 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY26774648;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY26774648 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20404145;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20404145 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY66915912;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY66915912 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY72023495;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY72023495 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY92197153;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY92197153 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY60438803;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY60438803 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY15855195;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY15855195 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50168582;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50168582 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY41436881;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY41436881 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY79899739;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY79899739 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY80007973;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY80007973 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY21419361;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY21419361 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY40489490;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY40489490 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27286049;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY27286049 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY82717565;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY82717565 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89630136;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY89630136 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY42462777;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY42462777 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY62314351;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY62314351 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY64651689;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY64651689 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY42642927;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY42642927 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY4462229;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY4462229 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY15909665;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY15909665 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY90179642;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY90179642 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY81839911;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY81839911 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY48997064;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY48997064 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20683729;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY20683729 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY36905334;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY36905334 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY2045266;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY2045266 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY67993033;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY67993033 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY93793235;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY93793235 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8435821;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY8435821 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY45195076;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY45195076 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY17236624;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY17236624 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50830232;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY50830232 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY71099927;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY71099927 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY29773979;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY29773979 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY53586550;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY53586550 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY23397827;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY23397827 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY35443812;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY35443812 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY64244738;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY64244738 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37798158;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37798158 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY44804703;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY44804703 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY4636582;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY4636582 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY65731560;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY65731560 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY52303306;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY52303306 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY23159187;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY23159187 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY667614;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY667614 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY2981507;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY2981507 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY25550522;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY25550522 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37949785;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY37949785 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY78540789;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY78540789 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY51456450;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY51456450 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY70276515;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY70276515 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY6123728;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY6123728 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY3858161;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY3858161 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10864983;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY10864983 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY30224503;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY30224503 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY85076000;     ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY85076000 = ScbBcpgdnetLvvcKZiANYudXmLtbFoexsqlY25160623;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ldJQUHTPbZJUygFKCbtSXFKrpYhYqoP23971538() {     int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne35362378 = -759343201;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4316130 = -968519064;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne29850330 = -227759523;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne41572174 = -833724093;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1892629 = -476614479;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne30166102 = -795076357;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne41161297 = -949487709;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91509815 = -170217457;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne25815665 = -380709860;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26671863 = -751607132;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1487640 = -963484179;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne59161059 = -152495907;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne51737714 = 42487732;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne33017876 = -965860515;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne78681453 = -473406689;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne11484108 = -633978391;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne93979845 = -360108315;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne85655647 = -115636266;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne46850518 = -319096968;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne70836680 = -480969112;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne97901448 = -72109111;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne69957146 = 86364052;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne56075625 = -12248788;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne99438620 = -854980104;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne19596025 = -139025536;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne82307004 = -916334137;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne28746613 = -182546908;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne76230497 = -429926755;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1516404 = -860764962;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne27851809 = -98478831;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne21513348 = 89767806;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne10806540 = -426042759;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne80315485 = 6194691;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne44006119 = -312250602;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne42619740 = 66941353;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne24325776 = -242632375;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne67144914 = 77352293;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne20821039 = 8071201;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26708216 = -317821726;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne17806139 = -808224261;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26905505 = -412799511;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne94412530 = -660181335;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7871536 = -738352874;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne84880948 = 48903462;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne63798524 = -187156222;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne723466 = 89433896;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne10593499 = -538422471;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne60601816 = 53154298;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne69471669 = -348244810;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91919695 = -85768469;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91788229 = -958169928;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne63539458 = -735753435;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4479157 = -471918555;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne230279 = -642084131;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26070089 = -23599223;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne65405231 = -745707254;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne48240504 = -856270277;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne30411709 = -372779419;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne21976149 = -594698558;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne19585625 = -560280342;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1419490 = -512529450;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne64930799 = -419560955;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne89993411 = -309452495;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne97963855 = -182231029;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne5158516 = -741374938;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne90681099 = -437441421;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne78845574 = -58690599;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7731595 = -645261666;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne90398136 = -932801869;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne54355678 = -130774315;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne44339193 = -611330684;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne73158807 = -268179516;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne58947432 = -797814541;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne29044380 = -510872707;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne43931175 = 31830399;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne3488919 = -411927777;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne62085611 = -175283075;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne71194677 = 38847750;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne35640097 = -567823883;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne18872560 = -128459432;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne71713505 = -277911667;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne68144797 = -135701207;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne6758828 = 18318054;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne9596709 = -674996494;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne36063580 = -140308904;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne57973889 = -174478759;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne6327383 = -954124205;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne80085206 = -351721178;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne17936030 = -188651380;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne77214509 = -187351394;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne76085272 = -386362099;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne36733206 = -549868289;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne98844889 = -397230242;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7122592 = -757541384;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne16386649 = -195694812;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne61974705 = -993238557;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4419119 = -250728841;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne9907680 = -456121845;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne79722432 = -209721600;    int WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne73117424 = -759343201;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne35362378 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4316130;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4316130 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne29850330;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne29850330 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne41572174;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne41572174 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1892629;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1892629 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne30166102;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne30166102 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne41161297;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne41161297 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91509815;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91509815 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne25815665;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne25815665 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26671863;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26671863 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1487640;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1487640 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne59161059;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne59161059 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne51737714;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne51737714 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne33017876;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne33017876 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne78681453;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne78681453 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne11484108;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne11484108 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne93979845;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne93979845 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne85655647;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne85655647 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne46850518;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne46850518 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne70836680;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne70836680 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne97901448;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne97901448 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne69957146;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne69957146 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne56075625;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne56075625 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne99438620;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne99438620 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne19596025;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne19596025 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne82307004;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne82307004 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne28746613;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne28746613 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne76230497;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne76230497 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1516404;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1516404 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne27851809;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne27851809 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne21513348;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne21513348 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne10806540;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne10806540 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne80315485;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne80315485 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne44006119;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne44006119 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne42619740;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne42619740 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne24325776;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne24325776 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne67144914;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne67144914 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne20821039;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne20821039 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26708216;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26708216 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne17806139;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne17806139 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26905505;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26905505 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne94412530;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne94412530 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7871536;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7871536 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne84880948;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne84880948 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne63798524;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne63798524 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne723466;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne723466 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne10593499;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne10593499 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne60601816;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne60601816 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne69471669;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne69471669 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91919695;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91919695 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91788229;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne91788229 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne63539458;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne63539458 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4479157;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4479157 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne230279;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne230279 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26070089;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne26070089 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne65405231;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne65405231 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne48240504;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne48240504 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne30411709;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne30411709 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne21976149;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne21976149 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne19585625;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne19585625 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1419490;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne1419490 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne64930799;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne64930799 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne89993411;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne89993411 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne97963855;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne97963855 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne5158516;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne5158516 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne90681099;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne90681099 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne78845574;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne78845574 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7731595;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7731595 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne90398136;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne90398136 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne54355678;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne54355678 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne44339193;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne44339193 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne73158807;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne73158807 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne58947432;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne58947432 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne29044380;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne29044380 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne43931175;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne43931175 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne3488919;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne3488919 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne62085611;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne62085611 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne71194677;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne71194677 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne35640097;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne35640097 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne18872560;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne18872560 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne71713505;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne71713505 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne68144797;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne68144797 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne6758828;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne6758828 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne9596709;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne9596709 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne36063580;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne36063580 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne57973889;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne57973889 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne6327383;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne6327383 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne80085206;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne80085206 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne17936030;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne17936030 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne77214509;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne77214509 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne76085272;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne76085272 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne36733206;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne36733206 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne98844889;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne98844889 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7122592;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne7122592 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne16386649;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne16386649 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne61974705;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne61974705 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4419119;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne4419119 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne9907680;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne9907680 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne79722432;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne79722432 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne73117424;     WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne73117424 = WqyHELPChSROnkNqpGyAPBjAfTRavZvZnKne35362378;}
// Junk Finished
