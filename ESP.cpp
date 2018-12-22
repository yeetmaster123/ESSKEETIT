
#include "ESP.h"
#include "Interfaces.h"
#include "DrawManager.h"
#include <iostream>
#include "Bullet_Tracer.h"
#include "shonaxsettings.h"
#include "DrawManager.h"
#define minimum(a,b)            (((a) < (b)) ? (a) : (b))
#define ccsplayer 38

CEsp esp;
DrawManager xdlol;
//CVisuals visuals;

float flPlayerAlpha[65];

void CEsp::paint() {
	auto m_local = game::localdata.localplayer();

	

	if (m_local) {
		for (int i = 0; i < m_pEntityList->GetHighestEntityIndex(); i++) {

			

			auto m_entity = static_cast<IClientEntity*>(m_pEntityList->GetClientEntity(i));
			if (!m_entity) continue;

			if (m_entity->GetClientClass()->m_ClassID == ccsplayer) {
				if (m_entity->IsDormant() && flPlayerAlpha[i] > 0) flPlayerAlpha[i] -= 5;
				else if (!(m_entity->IsDormant()) && flPlayerAlpha[i] < 255) flPlayerAlpha[i] += 5;
				float alpha = flPlayerAlpha[i];
				game::math.clamp(alpha, 0, 255);

				if (m_entity->IsAlive()) {

					Color plc = Color(get_player_colors(m_entity).r(), get_player_colors(m_entity).g(), get_player_colors(m_entity).b(), alpha);

					Box box; if (!get_box(m_entity, box, Shonax::settingsxd.esptype)) {
						player.direction_arrow( m_entity->GetOrigin( ) );
						continue;
					}

					if (!Shonax::settingsxd.espteam && m_entity->GetTeamNum() == m_local->GetTeamNum() || m_entity == m_local) continue;

					if (Shonax::settingsxd.esptype == 1 && !game::functions.visible(m_local, m_entity, 0)) continue;

					player.paint_player(m_entity, box, plc);
				}
			}
		}
		if (Shonax::settingsxd.noflash) {
			DWORD m_flFlashMaxAlpha = NetVar.GetNetVar(0xFE79FB98);
			*(float*)((DWORD)m_local + m_flFlashMaxAlpha) = 0;
		}
		if ( m_local->IsAlive() && Shonax::settingsxd.antiaimlines) antiaim_lines( );
	}
}

void CEsp::antiaim_lines() {
	auto m_local = game::localdata.localplayer();

	static float line_length = 80.f;

	Vector lby, fake, real;
	Vector start, end, forward, start_2d, end_2d;

	lby = Vector(0.f, m_local->GetLowerBodyYaw(), 0.f);
	fake = Vector(0.f, game::globals.aa_line_data.fake_angle, 0.f);
	real = Vector(0.f, game::globals.aa_line_data.real_angle, 0.f);

	start = m_local->GetOrigin();
	game::math.angle_vectors(lby, forward);
	forward *= line_length;
	end = start + forward;

	if (!game::functions.world_to_screen(start, start_2d) || !game::functions.world_to_screen(end, end_2d))
		return;

	draw.line(start_2d.x, start_2d.y, end_2d.x, end_2d.y, Color(0, 255, 0, 255));
	draw.text(end_2d.x, end_2d.y, "REAL", draw.fonts.esp_extra, Color(0, 255, 0, 255));

	game::math.angle_vectors(fake, forward);
	forward *= line_length;
	end = start + forward;

	if (!game::functions.world_to_screen(start, start_2d) || !game::functions.world_to_screen(end, end_2d))
		return;

	draw.line(start_2d.x, start_2d.y, end_2d.x, end_2d.y, Color(255, 0, 0, 255));
	draw.text(end_2d.x, end_2d.y, "FAKE", draw.fonts.esp_extra, Color(255, 0, 0, 255));

	game::math.angle_vectors(real, forward);
	forward *= line_length;
	end = start + forward;

	if (!game::functions.world_to_screen(start, start_2d) || !game::functions.world_to_screen(end, end_2d))
		return;

	draw.line(start_2d.x, start_2d.y, end_2d.x, end_2d.y, Color(255, 255, 0, 255));
	draw.text(end_2d.x, end_2d.y, "LBY", draw.fonts.esp_extra, Color(255, 255, 0, 255));
}

void CEsp::CVisualsPlayer::draw_box(IClientEntity* m_entity, Box box, Color color) {
	float alpha = flPlayerAlpha[m_entity->GetIndex()];

	if (Shonax::settingsxd.espboxfill && Shonax::settingsxd.espboxfillopacity > 0) {
		Color color_fill = Color(color.r(), color.g(), color.b(), (alpha / 255) * Shonax::settingsxd.espboxfillopacity * 2.55);
		draw.rect(box.x, box.y, box.w, box.h, color_fill);
	}

	if (Shonax::settingsxd.espbox) {
		if (!Shonax::settingsxd.espboxtype) {
			draw.outline(box.x, box.y, box.w, box.h, color);
			if (Shonax::settingsxd.espoutline) {
				draw.outline(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(21, 21, 21, alpha));
				draw.outline(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(21, 21, 21, alpha));
			}
		}
		else if (Shonax::settingsxd.espboxtype == 1) {

			float width_corner = box.w / 4;
			float height_corner = width_corner;

			if (Shonax::settingsxd.espoutline) {
				draw.rect(box.x - 1, box.y - 1, width_corner + 2, 3, Color(21, 21, 21, alpha));
				draw.rect(box.x - 1, box.y - 1, 3, height_corner + 2, Color(21, 21, 21, alpha));

				draw.rect((box.x + box.w) - width_corner - 1, box.y - 1, width_corner + 2, 3, Color(21, 21, 21, alpha));
				draw.rect(box.x + box.w - 1, box.y - 1, 3, height_corner + 2, Color(21, 21, 21, alpha));

				draw.rect(box.x - 1, box.y + box.h - 4, width_corner + 2, 3, Color(21, 21, 21, alpha));
				draw.rect(box.x - 1, (box.y + box.h) - height_corner - 4, 3, height_corner + 2, Color(21, 21, 21, alpha));

				draw.rect((box.x + box.w) - width_corner - 1, box.y + box.h - 4, width_corner + 2, 3, Color(21, 21, 21, alpha));
				draw.rect(box.x + box.w - 1, (box.y + box.h) - height_corner - 4, 3, height_corner + 3, Color(21, 21, 21, alpha));
			}

			draw.rect(box.x, box.y, width_corner, 1, color);
			draw.rect(box.x, box.y, 1, height_corner, color);

			draw.rect((box.x + box.w) - width_corner, box.y, width_corner, 1, color);
			draw.rect(box.x + box.w, box.y, 1, height_corner, color);

			draw.rect(box.x, box.y + box.h - 3, width_corner, 1, color);
			draw.rect(box.x, (box.y + box.h) - height_corner - 3, 1, height_corner, color);

			draw.rect((box.x + box.w) - width_corner, box.y + box.h - 3, width_corner, 1, color);
			draw.rect(box.x + box.w, (box.y + box.h) - height_corner - 3, 1, height_corner + 1, color);
		}
	}
}
Color GetPlayerColor(IClientEntity* pEntity)
{
	auto m_local = game::localdata.localplayer();
	int TeamNum = pEntity->GetTeamNum();
	bool IsVis = game::functions.visible(m_local, pEntity, (int)CSGOHitboxID::Head);

	Color color;

	if (TeamNum == TEAM_CS_T)
	{
		if (IsVis)
			color = Color(Shonax::settingsxd.espboxcolor[0] * 255, Shonax::settingsxd.espboxcolor[1] * 255, Shonax::settingsxd.espboxcolor[2] * 255, Shonax::settingsxd.espboxcolor[3] * 255);
		else
			color = Color(Shonax::settingsxd.espboxcolor[0] * 255, Shonax::settingsxd.espboxcolor[1] * 255, Shonax::settingsxd.espboxcolor[2] * 255, Shonax::settingsxd.espboxcolor[3] * 255);
	}
	else
	{

		if (IsVis)
			color = Color(Shonax::settingsxd.espboxcolor[0] * 255, Shonax::settingsxd.espboxcolor[1] * 255, Shonax::settingsxd.espboxcolor[2] * 255, Shonax::settingsxd.espboxcolor[3] * 255);
		else
			color = Color(Shonax::settingsxd.espboxcolor[0] * 255, Shonax::settingsxd.espboxcolor[1] * 255, Shonax::settingsxd.espboxcolor[2] * 255, Shonax::settingsxd.espboxcolor[3] * 255);
	}

	return color;
}
void CEsp::CVisualsPlayer::draw_health(IClientEntity* m_entity, Box box) {
	if (Shonax::settingsxd.esphealth && Shonax::settingsxd.esphealthtype==0) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];
		int player_health = m_entity->GetHealth() > 100 ? 100 : m_entity->GetHealth();

		if (player_health) {
			int color[3] = { 0, 0, 0 };

			if (player_health >= 85) {
				color[0] = 83; color[1] = 200; color[2] = 84;
			}
			else if (player_health >= 70) {
				color[0] = 107; color[1] = 142; color[2] = 35;
			}
			else if (player_health >= 55) {
				color[0] = 173; color[1] = 255; color[2] = 47;
			}
			else if (player_health >= 40) {
				color[0] = 255; color[1] = 215; color[2] = 0;
			}
			else if (player_health >= 25) {
				color[0] = 255; color[1] = 127; color[2] = 80;
			}
			else if (player_health >= 10) {
				color[0] = 205; color[1] = 92; color[2] = 92;
			}
			else if (player_health >= 0) {
				color[0] = 178; color[1] = 34; color[2] = 34;
			}

			if (Shonax::settingsxd.espoutline)
				draw.outline(box.x - 7, box.y - 1, 4, box.h + 2, Color(21, 21, 21, alpha));

			int health_height = player_health * box.h / 100;
			int add_space = box.h - health_height;

			Color hec = Color(color[0], color[1], color[2], alpha);

			draw.rect(box.x - 6, box.y, 2, box.h, Color(21, 21, 21, alpha));
			draw.rect(box.x - 6, box.y + add_space, 2, health_height, hec);

			if (Shonax::settingsxd.esphealthtext && player_health < 100) {
				RECT text_size = draw.get_text_size(std::to_string(player_health).c_str(), draw.fonts.esp_small);
				draw.text(box.x - 5 - (text_size.right / 2), box.y + add_space - (text_size.bottom / 2), std::to_string(player_health).c_str(), draw.fonts.esp_small, Color(255, 255, 255, alpha));
			}
		}
	}


	else
		if (Shonax::settingsxd.esphealth && Shonax::settingsxd.esphealthtype == 1)
		{
			Box HealthBar = box;
			HealthBar.y += (HealthBar.h + 6);
			HealthBar.h = 4;

			float HealthValue = m_entity->GetHealth();
			float HealthPerc = HealthValue / 100.f;
			float Width = (box.w * HealthPerc);
			HealthBar.w = Width;

			// --  Main Bar -- //

			Vertex_t Verts[4];
			Verts[0].Init(Vector2D(HealthBar.x, HealthBar.y));
			Verts[1].Init(Vector2D(HealthBar.x + box.w + 5, HealthBar.y));
			Verts[2].Init(Vector2D(HealthBar.x + box.w, HealthBar.y + 5));
			Verts[3].Init(Vector2D(HealthBar.x - 5, HealthBar.y + 5));

			xdlol.polygon_outlined(4, Verts, Color(10, 10, 10, 255), Color(255, 255, 255, 170));

			Vertex_t Verts2[4];
			Verts2[0].Init(Vector2D(HealthBar.x + 1, HealthBar.y + 1));
			Verts2[1].Init(Vector2D(HealthBar.x + HealthBar.w + 4, HealthBar.y + 1));
			Verts2[2].Init(Vector2D(HealthBar.x + HealthBar.w, HealthBar.y + 5));
			Verts2[3].Init(Vector2D(HealthBar.x - 4, HealthBar.y + 5));

			Color c = GetPlayerColor(m_entity);
			xdlol.polygon(4, Verts2, c);

			Verts2[0].Init(Vector2D(HealthBar.x + 1, HealthBar.y + 1));
			Verts2[1].Init(Vector2D(HealthBar.x + HealthBar.w + 2, HealthBar.y + 1));
			Verts2[2].Init(Vector2D(HealthBar.x + HealthBar.w, HealthBar.y + 2));
			Verts2[3].Init(Vector2D(HealthBar.x - 2, HealthBar.y + 2));

			xdlol.polygon(4, Verts2, Color(255, 255, 255, 40));
		}
}

void CEsp::CVisualsPlayer::draw_armor(IClientEntity* m_entity, Box box) {
	if (Shonax::settingsxd.esparmor) {
		int player_armor = m_entity->ArmorValue() > 100 ? 100 : m_entity->ArmorValue();
		float alpha = flPlayerAlpha[m_entity->GetIndex()];

		if (player_armor) {
			Color arc = Color(Shonax::settingsxd.esparmorcolor[0]*255, Shonax::settingsxd.esparmorcolor[1] * 255, Shonax::settingsxd.esparmorcolor[2] * 255, alpha);

			if (Shonax::settingsxd.espoutline)
				draw.outline(box.x - 1, box.y + box.h + 2, box.w + 2, 4, Color(21, 21, 21, alpha));

			int armor_width = player_armor * box.w / 100;

			draw.rect(box.x, box.y + box.h + 3, box.w, 2, Color(21, 21, 21, alpha));
			draw.rect(box.x, box.y + box.h + 3, armor_width, 2, arc);
		}
	}
}

void CEsp::CVisualsPlayer::draw_name(IClientEntity* m_entity, Box box) {
	if (Shonax::settingsxd.espname) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];

		player_info_t player_info;
		if (m_pEngine->GetPlayerInfo(m_entity->GetIndex(), &player_info)) {
			RECT name_size = draw.get_text_size(player_info.name, draw.fonts.esp);
			draw.text(box.x + (box.w / 2) - (name_size.right / 2), box.y - 14, player_info.name, draw.fonts.esp, Color(225, 225, 225, alpha));
		}
	}
}

void CEsp::CVisualsPlayer::draw_weapon(IClientEntity* m_entity, Box box) {
	float alpha = flPlayerAlpha[m_entity->GetIndex()];

	int bottom_pos = 0;
	bool wa_enabled = false;
	CBaseCombatWeapon* weapon = m_entity->GetWeapon();

	if (weapon) {
		int player_armor = m_entity->ArmorValue() > 100 ? 100 : m_entity->ArmorValue();
		bool armor = Shonax::settingsxd.esparmor && player_armor;
		if (Shonax::settingsxd.espweapon || Shonax::settingsxd.espammo) {
			char buffer[128];
			char* format = "";
			if (Shonax::settingsxd.espweapon && !Shonax::settingsxd.espammo) {
				format = XorStr("%s");
				sprintf_s(buffer, format, weapon->GetGunName());
				wa_enabled = true;
			} else if (!Shonax::settingsxd.espweapon && Shonax::settingsxd.espammo) {
				format = XorStr("%1.0f");
				float ammo = weapon->GetAmmoInClip();
				sprintf_s(buffer, format, ammo);
				wa_enabled = true;
			} else if (Shonax::settingsxd.espweapon && Shonax::settingsxd.espammo) {
				format = XorStr("%s | %1.0f");
				float ammo = weapon->GetAmmoInClip();
				sprintf_s(buffer, format, weapon->GetGunName(), ammo);
				wa_enabled = true;
			}

			RECT size = draw.get_text_size(buffer, draw.fonts.esp_extra);
			draw.text(box.x + (box.w / 2) - (size.right / 2), box.y + box.h + (armor ? 5 : 2), buffer, draw.fonts.esp_extra, Color(225, 225, 225, alpha));
			bottom_pos += 1;
		}
	}
}

void CEsp::CVisualsPlayer::draw_hit_angle(IClientEntity* m_entity, Box box) {
	
	if (Shonax::settingsxd.hitangle) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];
		CPlayer* m_player = plist.get_player(m_entity->GetIndex());
		RECT size = draw.get_text_size(XorStr("HA"), draw.fonts.esp_extra );

		bool draw_scoped = (Shonax::settingsxd.espscoped && m_entity->IsScoped( ));
		draw.text( ( box.x + box.w ) + 3, box.y + (draw_scoped ? 13 : 0), XorStr("HA"), draw.fonts.esp_extra, m_player->resolver_data.has_hit_angle ? Color(55, 255, 55, alpha) : Color(255, 255, 255, alpha));
	}
}

void CEsp::CVisualsPlayer::draw_skeletons(IClientEntity* m_entity, Box box) {
	if (Shonax::settingsxd.espsceleton) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];
		studiohdr_t* studio_hdr = m_pModelInfo->GetStudioModel(m_entity->GetModel());

		if (!studio_hdr)
			return;

		Vector vParent, vChild, sParent, sChild;

		for (int j = 0; j < studio_hdr->numbones; j++) {
			mstudiobone_t* pBone = studio_hdr->GetBone(j);

			if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1)) {
				vChild = m_entity->GetBonePos(j);
				vParent = m_entity->GetBonePos(pBone->parent);

				if (game::functions.world_to_screen(vParent, sParent) && game::functions.world_to_screen(vChild, sChild)) {
					draw.line(sParent[0], sParent[1], sChild[0], sChild[1], Color(Shonax::settingsxd.espsceletoncolor[0]*255, Shonax::settingsxd.espsceletoncolor[1] * 255, Shonax::settingsxd.espsceletoncolor[2] * 255, alpha));
				}
			}
		}
	}
}

void CEsp::CVisualsPlayer::draw_snaplines(IClientEntity* m_entity, Box box) {
	if (Shonax::settingsxd.espsnaplines) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];
		if (box.x >= 0 && box.y >= 0) {
			int width = 0;
			int height = 0;

			Vector to = Vector(box.x + (box.w / 2), box.y + box.h, 0);
			m_pEngine->GetScreenSize(width, height);

			Vector From((width / 2), (height / 2), 0);
			draw.line(From.x, From.y, to.x, to.y, Color(Shonax::settingsxd.espsnaplinescolor[0]*255, Shonax::settingsxd.espsnaplinescolor[1] * 255, Shonax::settingsxd.espsnaplinescolor[2] * 255,  alpha));
		}
	}
}

void CEsp::CVisualsPlayer::draw_hitbones(IClientEntity* m_entity) {
	if (Shonax::settingsxd.esphitbones) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];
		for (int i = 0; i < 19; i++) {
			Vector screen_position;
			Vector hitbone_pos = game::functions.get_hitbox_location(m_entity, i);
			if (game::functions.world_to_screen(hitbone_pos, screen_position)) {
				draw.rect(screen_position.x, screen_position.y, 4, 4, Color(21, 21, 21, alpha));
				draw.rect(screen_position.x + 1, screen_position.y + 1, 2, 2, Color(Shonax::settingsxd.esphitbonescolor[0]*255, Shonax::settingsxd.esphitbonescolor[1] * 255, Shonax::settingsxd.esphitbonescolor[2] * 255,  alpha));
			}
		}
	}
}

void CEsp::CVisualsPlayer::draw_scoped(IClientEntity* m_entity, Box box) {
	if (Shonax::settingsxd.espscoped) {
		float alpha = flPlayerAlpha[m_entity->GetIndex()];

		if (m_entity->IsScoped()) {
			draw.text((box.x + box.w) + 3, box.y, "SCOPED", draw.fonts.esp_extra, Color(255, 99, 71, alpha));
		}
	}
}

void CEsp::CVisualsPlayer::direction_arrow( const Vector& origin ) {
	if (Shonax::settingsxd.directionarrow) {
		constexpr float radius = 360.0f;
		int width, height;
		m_pEngine->GetScreenSize( width, height );

		Vector vRealAngles;
		m_pEngine->GetViewAngles( vRealAngles );

		Vector vForward, vRight, vUp( 0.0f, 0.0f, 1.0f );

		game::math.angle_vectors( vRealAngles, &vForward, NULL, NULL );

		vForward.z = 0.0f;
		VectorNormalize( vForward );
		CrossProduct( vUp, vForward, vRight );

		float flFront = DotProduct( origin, vForward );
		float flSide = DotProduct( origin, vRight );
		float flXPosition = radius * -flSide;
		float flYPosition = radius * -flFront;

		float rotation = game::globals.UserCmd->viewangles.y;

		rotation = atan2( flXPosition, flYPosition ) + M_PI;
		rotation *= 180.0f / M_PI;

		float flYawRadians = -( rotation )* M_PI / 180.0f;
		float flCosYaw = cos( flYawRadians );
		float flSinYaw = sin( flYawRadians );

		flXPosition = ( int )( ( width / 2.0f ) + ( radius * flSinYaw ) );
		flYPosition = ( int )( ( height / 2.0f ) - ( radius * flCosYaw ) );

		draw.rect( flXPosition, flYPosition, 10, 10, Color(Shonax::settingsxd.directionarrowcolor[0]*255, Shonax::settingsxd.directionarrowcolor[1] * 255, Shonax::settingsxd.directionarrowcolor[2] * 255, Shonax::settingsxd.directionarrowcolor[3] * 255) );
	}
}

void CEsp::CGlow::shutdown() {
	for (auto i = 0; i < m_pGlowObjManager->size; i++) {
		auto& glow_object = m_pGlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<IClientEntity*>(glow_object.m_pEntity);

		if (glow_object.IsUnused())
			continue;

		if (!entity || entity->IsDormant())
			continue;

		glow_object.m_flGlowAlpha = 0.0f;
	}
}

void CEsp::CGlow::paint()
{
	auto m_local = game::localdata.localplayer();
	for (auto i = 0; i < m_pGlowObjManager->size; i++) {
		auto glow_object = &m_pGlowObjManager->m_GlowObjectDefinitions[i];

		IClientEntity *m_entity = glow_object->m_pEntity;

		if (!glow_object->m_pEntity || glow_object->IsUnused())
			continue;

		if (m_entity->GetClientClass()->m_ClassID == 38) {
			if (m_entity->GetTeamNum() == m_local->GetTeamNum() && !Shonax::settingsxd.espteam || Shonax::settingsxd.espteam) continue;

			bool m_visible = game::functions.visible(m_local, m_entity, 0);
			float m_flRed = Shonax::settingsxd.espglowcolor[0], m_flGreen = Shonax::settingsxd.espglowcolor[1], m_flBlue = Shonax::settingsxd.espglowcolor[2];
			bool m_teammate = m_entity->GetTeamNum() == m_local->GetTeamNum();

			glow_object->m_vGlowColor = Vector(m_flRed / 255, m_flGreen / 255, m_flBlue / 255);
			glow_object->m_flGlowAlpha = (Shonax::settingsxd.espglowcolor[3] * 2.55) / 255;
			glow_object->m_bRenderWhenOccluded = true;
			glow_object->m_bRenderWhenUnoccluded = false;
		}
	}
}
void cbullet_tracer::log(IGameEvent* event)
{

	ConVar* m_pConVar;
	
	//if we receive bullet_impact event
	if (Shonax::settingsxd.bullettracer)
	{
		//get the user who fired the bullet
		auto index = m_pEngine->GetPlayerForUserID(event->GetInt("userid"));

		//return if the userid is not valid or we werent the entity who was fireing
		if (index != m_pEngine->GetLocalPlayer())
			return;

		//get local player
		auto local = static_cast<IClientEntity*>(m_pEntityList->GetClientEntity(index));
		if (!local)
			return;

		//get the bullet impact's position
		Vector position(m_pConVar->GetFloat(), m_pConVar->GetFloat(), m_pConVar->GetFloat());

		Ray_t ray;
		ray.Init(local->GetEyePosition(), position);

		//skip local player
		CTraceFilter filter;
		filter.pSkip = local;

		//trace a ray
		trace_t tr;
		m_pTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		//use different color when we hit a player
		auto color = (tr.m_pEnt && reinterpret_cast<IClientEntity*>(tr.m_pEnt)->IsPlayer()) ? Color(140, 29, 29, 220) : Color(255, 255, 0, 220);

		//push info to our vector
		logs.push_back(cbullet_tracer_info(local->GetEyePosition(), position, m_pGlobals->curtime, color));
	}
}

void cbullet_tracer::render()
{
	if (!Shonax::settingsxd.bullettracer)
		return;

	//get local player
	auto local = static_cast<IClientEntity*>(m_pEntityList->GetClientEntity(m_pEngine->GetLocalPlayer()));
	if (!local)
		return;

	//loop through our vector
	for (size_t i = 0; i < logs.size(); i++)
	{
		//get the current item
		auto current = logs.at(i);

		//draw a line from local player's head position to the hit point
		m_pDebugOverlay->AddLineOverlay(current.src, current.dst, current.color.r(), current.color.g(), current.color.b(), true, -1.f);
		//draw a box at the hit point
		m_pDebugOverlay->AddBoxOverlay(current.dst, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 127, -1.f);

		//if the item is older than 5 seconds, delete it
		if (fabs(m_pGlobals->curtime - current.time) > 5.f)
			logs.erase(logs.begin() + i);
	}
}


// Junk Code By Troll Face & Thaisen's Gen
void IlPIStcndC31421953() {     double cgSSDkkqFK60570426 = -414987570;    double cgSSDkkqFK59236398 = -422794008;    double cgSSDkkqFK41604346 = 18945049;    double cgSSDkkqFK36346601 = -743739556;    double cgSSDkkqFK76831748 = -527426364;    double cgSSDkkqFK46953039 = 82559400;    double cgSSDkkqFK95474245 = -441532192;    double cgSSDkkqFK10541243 = -967341342;    double cgSSDkkqFK51040707 = -482797256;    double cgSSDkkqFK12803992 = -746246459;    double cgSSDkkqFK59898048 = -908355279;    double cgSSDkkqFK40299015 = -419768611;    double cgSSDkkqFK70564749 = -112981343;    double cgSSDkkqFK87446923 = -53314356;    double cgSSDkkqFK83481698 = -642644148;    double cgSSDkkqFK60548093 = -279794999;    double cgSSDkkqFK17673645 = -988150482;    double cgSSDkkqFK96304984 = -686814822;    double cgSSDkkqFK79251005 = -718618624;    double cgSSDkkqFK44452379 = -545310003;    double cgSSDkkqFK44768591 = -970875450;    double cgSSDkkqFK1455304 = -509589116;    double cgSSDkkqFK96544003 = -426510236;    double cgSSDkkqFK46077357 = -713239940;    double cgSSDkkqFK88051587 = -821737785;    double cgSSDkkqFK32420767 = -826104061;    double cgSSDkkqFK655838 = -694385922;    double cgSSDkkqFK43095451 = -675540105;    double cgSSDkkqFK82723008 = -383708479;    double cgSSDkkqFK11227969 = 24203118;    double cgSSDkkqFK81466758 = -887587019;    double cgSSDkkqFK52354742 = -560824985;    double cgSSDkkqFK67663058 = -398504572;    double cgSSDkkqFK2541340 = 3681893;    double cgSSDkkqFK60246474 = -900452921;    double cgSSDkkqFK14984219 = -346041056;    double cgSSDkkqFK20133181 = -29880829;    double cgSSDkkqFK19092781 = -691943217;    double cgSSDkkqFK55609805 = -379758229;    double cgSSDkkqFK34760853 = -826000655;    double cgSSDkkqFK51860900 = -568448238;    double cgSSDkkqFK81183732 = -978806840;    double cgSSDkkqFK50671520 = -108618115;    double cgSSDkkqFK86599917 = 21014202;    double cgSSDkkqFK31188428 = 20800155;    double cgSSDkkqFK25818904 = 94978676;    double cgSSDkkqFK20496682 = -516350554;    double cgSSDkkqFK19831817 = -67910217;    double cgSSDkkqFK91793884 = -134168783;    double cgSSDkkqFK83376152 = -747104844;    double cgSSDkkqFK96995355 = 82453631;    double cgSSDkkqFK91464979 = -726164715;    double cgSSDkkqFK47459122 = -61934775;    double cgSSDkkqFK39609910 = -846800477;    double cgSSDkkqFK86653362 = -769028220;    double cgSSDkkqFK59115122 = -905398454;    double cgSSDkkqFK62692394 = -996283773;    double cgSSDkkqFK95526989 = -267815012;    double cgSSDkkqFK48295014 = -922001771;    double cgSSDkkqFK44410981 = -701322304;    double cgSSDkkqFK46297202 = -223054679;    double cgSSDkkqFK52378795 = -765992087;    double cgSSDkkqFK27818235 = -483632864;    double cgSSDkkqFK39812739 = -407000374;    double cgSSDkkqFK31337234 = -858659440;    double cgSSDkkqFK7543307 = -247530295;    double cgSSDkkqFK72635956 = 78735960;    double cgSSDkkqFK68023410 = -16663237;    double cgSSDkkqFK27200449 = -152861436;    double cgSSDkkqFK68497480 = -196603093;    double cgSSDkkqFK40414913 = -149914171;    double cgSSDkkqFK98580864 = -196207266;    double cgSSDkkqFK40695179 = -207056594;    double cgSSDkkqFK44490153 = -892617969;    double cgSSDkkqFK92591479 = -976861766;    double cgSSDkkqFK63584859 = -992068611;    double cgSSDkkqFK50783783 = -300971001;    double cgSSDkkqFK9944087 = -347524438;    double cgSSDkkqFK14888930 = -634040095;    double cgSSDkkqFK62232683 = -816716461;    double cgSSDkkqFK11924086 = -209753507;    double cgSSDkkqFK80824020 = -526475705;    double cgSSDkkqFK51301566 = -441371323;    double cgSSDkkqFK99346855 = -636603635;    double cgSSDkkqFK14232614 = 41749486;    double cgSSDkkqFK90001779 = -61422305;    double cgSSDkkqFK4895620 = -398890211;    double cgSSDkkqFK28053148 = -551704095;    double cgSSDkkqFK15887977 = -227289888;    double cgSSDkkqFK1131353 = -995054467;    double cgSSDkkqFK52291824 = -349757283;    double cgSSDkkqFK24606191 = -762065818;    double cgSSDkkqFK70797766 = -769941446;    double cgSSDkkqFK11198825 = -678435925;    double cgSSDkkqFK88463650 = -502945976;    double cgSSDkkqFK99482104 = -802456152;    double cgSSDkkqFK53365497 = -395173976;    double cgSSDkkqFK10858782 = -701617741;    double cgSSDkkqFK55262684 = -120326358;    double cgSSDkkqFK23645121 = -414987570;     cgSSDkkqFK60570426 = cgSSDkkqFK59236398;     cgSSDkkqFK59236398 = cgSSDkkqFK41604346;     cgSSDkkqFK41604346 = cgSSDkkqFK36346601;     cgSSDkkqFK36346601 = cgSSDkkqFK76831748;     cgSSDkkqFK76831748 = cgSSDkkqFK46953039;     cgSSDkkqFK46953039 = cgSSDkkqFK95474245;     cgSSDkkqFK95474245 = cgSSDkkqFK10541243;     cgSSDkkqFK10541243 = cgSSDkkqFK51040707;     cgSSDkkqFK51040707 = cgSSDkkqFK12803992;     cgSSDkkqFK12803992 = cgSSDkkqFK59898048;     cgSSDkkqFK59898048 = cgSSDkkqFK40299015;     cgSSDkkqFK40299015 = cgSSDkkqFK70564749;     cgSSDkkqFK70564749 = cgSSDkkqFK87446923;     cgSSDkkqFK87446923 = cgSSDkkqFK83481698;     cgSSDkkqFK83481698 = cgSSDkkqFK60548093;     cgSSDkkqFK60548093 = cgSSDkkqFK17673645;     cgSSDkkqFK17673645 = cgSSDkkqFK96304984;     cgSSDkkqFK96304984 = cgSSDkkqFK79251005;     cgSSDkkqFK79251005 = cgSSDkkqFK44452379;     cgSSDkkqFK44452379 = cgSSDkkqFK44768591;     cgSSDkkqFK44768591 = cgSSDkkqFK1455304;     cgSSDkkqFK1455304 = cgSSDkkqFK96544003;     cgSSDkkqFK96544003 = cgSSDkkqFK46077357;     cgSSDkkqFK46077357 = cgSSDkkqFK88051587;     cgSSDkkqFK88051587 = cgSSDkkqFK32420767;     cgSSDkkqFK32420767 = cgSSDkkqFK655838;     cgSSDkkqFK655838 = cgSSDkkqFK43095451;     cgSSDkkqFK43095451 = cgSSDkkqFK82723008;     cgSSDkkqFK82723008 = cgSSDkkqFK11227969;     cgSSDkkqFK11227969 = cgSSDkkqFK81466758;     cgSSDkkqFK81466758 = cgSSDkkqFK52354742;     cgSSDkkqFK52354742 = cgSSDkkqFK67663058;     cgSSDkkqFK67663058 = cgSSDkkqFK2541340;     cgSSDkkqFK2541340 = cgSSDkkqFK60246474;     cgSSDkkqFK60246474 = cgSSDkkqFK14984219;     cgSSDkkqFK14984219 = cgSSDkkqFK20133181;     cgSSDkkqFK20133181 = cgSSDkkqFK19092781;     cgSSDkkqFK19092781 = cgSSDkkqFK55609805;     cgSSDkkqFK55609805 = cgSSDkkqFK34760853;     cgSSDkkqFK34760853 = cgSSDkkqFK51860900;     cgSSDkkqFK51860900 = cgSSDkkqFK81183732;     cgSSDkkqFK81183732 = cgSSDkkqFK50671520;     cgSSDkkqFK50671520 = cgSSDkkqFK86599917;     cgSSDkkqFK86599917 = cgSSDkkqFK31188428;     cgSSDkkqFK31188428 = cgSSDkkqFK25818904;     cgSSDkkqFK25818904 = cgSSDkkqFK20496682;     cgSSDkkqFK20496682 = cgSSDkkqFK19831817;     cgSSDkkqFK19831817 = cgSSDkkqFK91793884;     cgSSDkkqFK91793884 = cgSSDkkqFK83376152;     cgSSDkkqFK83376152 = cgSSDkkqFK96995355;     cgSSDkkqFK96995355 = cgSSDkkqFK91464979;     cgSSDkkqFK91464979 = cgSSDkkqFK47459122;     cgSSDkkqFK47459122 = cgSSDkkqFK39609910;     cgSSDkkqFK39609910 = cgSSDkkqFK86653362;     cgSSDkkqFK86653362 = cgSSDkkqFK59115122;     cgSSDkkqFK59115122 = cgSSDkkqFK62692394;     cgSSDkkqFK62692394 = cgSSDkkqFK95526989;     cgSSDkkqFK95526989 = cgSSDkkqFK48295014;     cgSSDkkqFK48295014 = cgSSDkkqFK44410981;     cgSSDkkqFK44410981 = cgSSDkkqFK46297202;     cgSSDkkqFK46297202 = cgSSDkkqFK52378795;     cgSSDkkqFK52378795 = cgSSDkkqFK27818235;     cgSSDkkqFK27818235 = cgSSDkkqFK39812739;     cgSSDkkqFK39812739 = cgSSDkkqFK31337234;     cgSSDkkqFK31337234 = cgSSDkkqFK7543307;     cgSSDkkqFK7543307 = cgSSDkkqFK72635956;     cgSSDkkqFK72635956 = cgSSDkkqFK68023410;     cgSSDkkqFK68023410 = cgSSDkkqFK27200449;     cgSSDkkqFK27200449 = cgSSDkkqFK68497480;     cgSSDkkqFK68497480 = cgSSDkkqFK40414913;     cgSSDkkqFK40414913 = cgSSDkkqFK98580864;     cgSSDkkqFK98580864 = cgSSDkkqFK40695179;     cgSSDkkqFK40695179 = cgSSDkkqFK44490153;     cgSSDkkqFK44490153 = cgSSDkkqFK92591479;     cgSSDkkqFK92591479 = cgSSDkkqFK63584859;     cgSSDkkqFK63584859 = cgSSDkkqFK50783783;     cgSSDkkqFK50783783 = cgSSDkkqFK9944087;     cgSSDkkqFK9944087 = cgSSDkkqFK14888930;     cgSSDkkqFK14888930 = cgSSDkkqFK62232683;     cgSSDkkqFK62232683 = cgSSDkkqFK11924086;     cgSSDkkqFK11924086 = cgSSDkkqFK80824020;     cgSSDkkqFK80824020 = cgSSDkkqFK51301566;     cgSSDkkqFK51301566 = cgSSDkkqFK99346855;     cgSSDkkqFK99346855 = cgSSDkkqFK14232614;     cgSSDkkqFK14232614 = cgSSDkkqFK90001779;     cgSSDkkqFK90001779 = cgSSDkkqFK4895620;     cgSSDkkqFK4895620 = cgSSDkkqFK28053148;     cgSSDkkqFK28053148 = cgSSDkkqFK15887977;     cgSSDkkqFK15887977 = cgSSDkkqFK1131353;     cgSSDkkqFK1131353 = cgSSDkkqFK52291824;     cgSSDkkqFK52291824 = cgSSDkkqFK24606191;     cgSSDkkqFK24606191 = cgSSDkkqFK70797766;     cgSSDkkqFK70797766 = cgSSDkkqFK11198825;     cgSSDkkqFK11198825 = cgSSDkkqFK88463650;     cgSSDkkqFK88463650 = cgSSDkkqFK99482104;     cgSSDkkqFK99482104 = cgSSDkkqFK53365497;     cgSSDkkqFK53365497 = cgSSDkkqFK10858782;     cgSSDkkqFK10858782 = cgSSDkkqFK55262684;     cgSSDkkqFK55262684 = cgSSDkkqFK23645121;     cgSSDkkqFK23645121 = cgSSDkkqFK60570426;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void YhhCEdOFQAhAlWNjNIhbkbDpPPlduqQ78997963() {     double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp10319032 = -308368407;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2689918 = -982781751;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51727495 = -140580163;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp96331395 = -985138195;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp83676103 = -639288730;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp56017737 = -703888775;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51099053 = -116206351;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp72454493 = -678931601;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp74210810 = -749712898;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp69879130 = -89272473;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2482087 = -916252417;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp33069178 = -541494760;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp86457580 = -572256959;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp12211975 = -776012623;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp60083934 = -12215874;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp47124034 = -703118925;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp10113286 = -915574171;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp64409809 = -989922547;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91211835 = -306623566;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp31369355 = -908751937;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp6969203 = -811453063;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp73372816 = -479004927;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24834398 = -202998117;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91925191 = -417583704;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36497950 = -770115481;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp69442305 = -75542293;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp85631986 = -250255639;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp98169276 = -588299507;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70005982 = -132123633;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp23657500 = -996558885;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp42865196 = -186549921;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp57195730 = -605692982;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp98917782 = -180214137;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43015813 = 47137408;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp48105627 = -157451077;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp8520431 = -748430597;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp11442364 = -282057648;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp894076 = -517060103;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20608145 = -243855107;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91349867 = -333401252;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp41993151 = -577888445;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp17733853 = 88534583;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp44040017 = -295616835;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp88661171 = -29869961;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp71008619 = -960994060;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp97026994 = -21191224;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45007064 = -859702377;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp42235424 = -772320438;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp52914577 = -817666314;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp68473604 = 33724200;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36138412 = -104728706;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24146542 = -841938689;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp29050004 = -665022661;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp12129577 = -413128444;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp72028761 = -905602050;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36946216 = -829363480;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp77855520 = -679783634;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp59802303 = -722996460;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp59833445 = -115022714;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp14233798 = -463746438;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70385751 = -353633136;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp52929776 = -527906844;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2448511 = -446807968;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp50553310 = -753154013;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp27013934 = -902722553;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45286356 = -210559435;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp34151395 = -261280624;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43441768 = -519394368;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp64106347 = -518561546;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51563503 = -263785278;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp35681670 = -321061278;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp9219210 = -298514069;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43801664 = -646067441;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp99861967 = -973222315;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89376203 = -230863492;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89235349 = -799987647;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp29332799 = -83388093;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36173226 = -73128156;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20916573 = -456589644;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp39470956 = -648924258;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24435241 = -215839917;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43396563 = -477935201;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45254700 = -770633194;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp1532379 = -65847834;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp87519088 = -791830180;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp18718655 = -344611232;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp28145726 = -940670322;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp86788206 = -767085693;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70987051 = -47260542;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp11159412 = -328087598;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp30664911 = 31353037;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51640060 = -559061188;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp41060631 = -302037389;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp6374347 = -780108669;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20964116 = -979768116;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89063375 = 50018398;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp15285343 = -464657449;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp93486706 = -542462822;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp61647238 = -127147409;    double LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp25722263 = -308368407;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp10319032 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2689918;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2689918 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51727495;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51727495 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp96331395;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp96331395 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp83676103;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp83676103 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp56017737;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp56017737 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51099053;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51099053 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp72454493;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp72454493 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp74210810;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp74210810 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp69879130;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp69879130 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2482087;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2482087 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp33069178;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp33069178 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp86457580;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp86457580 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp12211975;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp12211975 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp60083934;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp60083934 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp47124034;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp47124034 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp10113286;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp10113286 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp64409809;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp64409809 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91211835;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91211835 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp31369355;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp31369355 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp6969203;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp6969203 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp73372816;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp73372816 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24834398;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24834398 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91925191;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91925191 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36497950;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36497950 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp69442305;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp69442305 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp85631986;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp85631986 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp98169276;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp98169276 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70005982;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70005982 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp23657500;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp23657500 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp42865196;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp42865196 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp57195730;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp57195730 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp98917782;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp98917782 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43015813;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43015813 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp48105627;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp48105627 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp8520431;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp8520431 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp11442364;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp11442364 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp894076;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp894076 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20608145;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20608145 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91349867;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp91349867 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp41993151;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp41993151 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp17733853;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp17733853 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp44040017;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp44040017 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp88661171;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp88661171 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp71008619;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp71008619 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp97026994;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp97026994 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45007064;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45007064 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp42235424;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp42235424 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp52914577;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp52914577 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp68473604;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp68473604 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36138412;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36138412 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24146542;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24146542 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp29050004;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp29050004 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp12129577;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp12129577 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp72028761;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp72028761 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36946216;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36946216 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp77855520;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp77855520 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp59802303;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp59802303 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp59833445;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp59833445 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp14233798;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp14233798 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70385751;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70385751 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp52929776;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp52929776 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2448511;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp2448511 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp50553310;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp50553310 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp27013934;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp27013934 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45286356;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45286356 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp34151395;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp34151395 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43441768;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43441768 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp64106347;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp64106347 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51563503;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51563503 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp35681670;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp35681670 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp9219210;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp9219210 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43801664;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43801664 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp99861967;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp99861967 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89376203;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89376203 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89235349;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89235349 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp29332799;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp29332799 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36173226;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp36173226 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20916573;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20916573 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp39470956;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp39470956 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24435241;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp24435241 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43396563;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp43396563 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45254700;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp45254700 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp1532379;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp1532379 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp87519088;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp87519088 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp18718655;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp18718655 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp28145726;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp28145726 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp86788206;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp86788206 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70987051;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp70987051 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp11159412;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp11159412 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp30664911;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp30664911 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51640060;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp51640060 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp41060631;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp41060631 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp6374347;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp6374347 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20964116;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp20964116 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89063375;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp89063375 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp15285343;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp15285343 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp93486706;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp93486706 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp61647238;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp61647238 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp25722263;     LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp25722263 = LKCxeYdKqwvAlqheIfNUSRwrwesnoslLZkWp10319032;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DDgIXeRQvKWDuNvJMaysauFrbaPXUsX7034324() {     double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk71084064 = -730322021;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31633133 = -214541471;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk29815537 = -633926312;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk79920978 = 97469934;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35863364 = -200729162;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk96414769 = -796968658;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1236059 = -261600467;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20603538 = -70551064;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9367880 = -278245004;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk90380288 = -333856091;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk61524276 = -278518647;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk41948610 = -907803413;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk68336928 = -837346827;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42166302 = -165507820;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31222544 = -572512524;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk52961511 = -639073683;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk59633811 = -725333682;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk52060082 = -16081279;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74690547 = -516186478;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1627838 = -23045454;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9366208 = -945490043;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk89442694 = 36629365;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk33773418 = -995216867;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20996043 = -219850030;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk4392906 = -842918819;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk51979004 = -966112329;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk56303270 = 93438166;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99928927 = -171247310;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk88714362 = -280821528;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk62798416 = -780219095;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20225954 = 64163381;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70521819 = -18458088;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk61737031 = -831289337;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk32785107 = -361206899;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26442474 = -173361367;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk7401076 = -190218047;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk79350154 = -185773516;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk17879207 = -157757812;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk80301644 = -232408621;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74711092 = -74580760;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk91477443 = -843656396;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk72256629 = -982931864;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk73390537 = -776056991;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk44295210 = -179857009;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26963200 = -128669214;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk4727760 = -987241626;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk6957143 = -386815920;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk49003254 = -268629699;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk23591462 = -366897352;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk46272342 = -980330788;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk60211999 = -282792886;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk38885668 = -439876740;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk84474812 = -88781714;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk75764485 = -61404925;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk97668115 = -658941792;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk81641370 = -666951386;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk97859715 = -219324604;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk8819495 = -314076282;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk75528072 = -59611248;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk83884360 = -234616834;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk40111499 = -790406824;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1307132 = 9646842;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31889176 = -789729536;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk46569464 = -498025910;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70154335 = -298019472;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk91002457 = -160060560;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk80211579 = 23485923;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35551822 = -376139928;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk15723829 = -992146454;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk23821468 = -282294478;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk73611357 = -353300167;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk41754604 = -467575870;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk71758438 = -783672658;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99979454 = -341605718;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk10150395 = -179389059;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk37109579 = -962558180;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk16052157 = -187313644;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk89478207 = -715359859;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk94032842 = 8819183;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99665146 = -855677194;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk45021862 = -479296409;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk7300016 = -637932135;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk76337465 = -804349958;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42442020 = -300490740;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk2586417 = -397426209;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk81340285 = -495959879;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk86047006 = -929676374;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk85972546 = -669884413;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35116991 = -702265108;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk44801104 = -506409981;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9541361 = -970893443;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70530659 = -871697235;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42351135 = 1853435;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk96417284 = -997791788;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk34599594 = -284173936;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk90170312 = -753303238;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk40367453 = -93202328;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26821074 = -178031081;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74140875 = -881837537;    double aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35960743 = -730322021;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk71084064 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31633133;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31633133 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk29815537;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk29815537 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk79920978;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk79920978 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35863364;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35863364 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk96414769;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk96414769 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1236059;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1236059 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20603538;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20603538 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9367880;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9367880 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk90380288;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk90380288 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk61524276;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk61524276 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk41948610;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk41948610 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk68336928;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk68336928 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42166302;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42166302 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31222544;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31222544 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk52961511;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk52961511 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk59633811;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk59633811 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk52060082;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk52060082 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74690547;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74690547 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1627838;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1627838 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9366208;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9366208 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk89442694;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk89442694 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk33773418;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk33773418 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20996043;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20996043 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk4392906;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk4392906 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk51979004;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk51979004 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk56303270;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk56303270 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99928927;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99928927 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk88714362;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk88714362 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk62798416;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk62798416 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20225954;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk20225954 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70521819;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70521819 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk61737031;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk61737031 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk32785107;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk32785107 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26442474;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26442474 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk7401076;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk7401076 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk79350154;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk79350154 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk17879207;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk17879207 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk80301644;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk80301644 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74711092;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74711092 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk91477443;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk91477443 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk72256629;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk72256629 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk73390537;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk73390537 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk44295210;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk44295210 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26963200;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26963200 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk4727760;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk4727760 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk6957143;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk6957143 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk49003254;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk49003254 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk23591462;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk23591462 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk46272342;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk46272342 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk60211999;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk60211999 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk38885668;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk38885668 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk84474812;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk84474812 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk75764485;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk75764485 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk97668115;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk97668115 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk81641370;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk81641370 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk97859715;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk97859715 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk8819495;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk8819495 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk75528072;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk75528072 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk83884360;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk83884360 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk40111499;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk40111499 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1307132;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk1307132 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31889176;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk31889176 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk46569464;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk46569464 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70154335;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70154335 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk91002457;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk91002457 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk80211579;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk80211579 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35551822;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35551822 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk15723829;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk15723829 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk23821468;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk23821468 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk73611357;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk73611357 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk41754604;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk41754604 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk71758438;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk71758438 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99979454;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99979454 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk10150395;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk10150395 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk37109579;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk37109579 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk16052157;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk16052157 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk89478207;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk89478207 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk94032842;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk94032842 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99665146;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk99665146 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk45021862;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk45021862 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk7300016;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk7300016 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk76337465;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk76337465 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42442020;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42442020 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk2586417;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk2586417 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk81340285;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk81340285 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk86047006;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk86047006 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk85972546;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk85972546 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35116991;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35116991 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk44801104;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk44801104 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9541361;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk9541361 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70530659;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk70530659 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42351135;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk42351135 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk96417284;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk96417284 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk34599594;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk34599594 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk90170312;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk90170312 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk40367453;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk40367453 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26821074;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk26821074 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74140875;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk74140875 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35960743;     aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk35960743 = aTlfyFxfeugyOhYAXUXpPbmmIxphwPtvDnwk71084064;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void JuGctpuaCdWBmHJORIBxWIieTQyIbdj74437828() {     double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96383657 = -558522876;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO85876586 = -708344709;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71830314 = -318733998;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO76367067 = 80688303;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40605459 = -124926070;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO35394361 = -327133457;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9262994 = -266805317;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO75835073 = -622113703;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO25747509 = -132091773;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO92618248 = -550559582;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO2705338 = -898249221;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO64551572 = -45469658;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO43834544 = -572082664;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO72823986 = 76207340;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO89338745 = -592097011;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO49822234 = -172012485;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9171161 = -927733210;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO62950696 = -465883564;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO78047199 = 72831766;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO50122865 = -422802356;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO78345774 = -32718708;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO80387313 = -971091931;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9065455 = -408761275;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO99924056 = -492212275;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO92712600 = -218430812;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40391609 = -976311487;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO36775562 = -57383601;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96363677 = -193389926;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO73432973 = -501494935;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO72495790 = -204803507;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12416158 = -620236065;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO30748748 = -50073540;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO97790691 = -436712776;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO93088811 = -581539125;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO21328905 = -497485330;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO97549066 = -597205938;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9787496 = -324446723;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53566614 = -911062620;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO44918178 = -182767276;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO65673678 = 40070592;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO95365795 = -328169331;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO23111940 = -453645102;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO73641040 = -62446615;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO19029808 = -382476089;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO66919374 = 56907392;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6895165 = -626413066;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO77259849 = 88776275;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO13428355 = -207173723;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO90034070 = -916155372;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO19586914 = 9616152;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO16774233 = -165135516;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO84686579 = -206308193;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO46726821 = -748549222;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO33657818 = -50092757;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96092054 = -922690665;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO15996344 = -587430945;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO76811132 = -199583435;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71906258 = -826521723;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO83654466 = -700880885;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO213850 = -148614584;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO98618799 = -169749857;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12899316 = 26584609;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO2402100 = -20618768;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53251718 = -927288267;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO80202090 = -930323518;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71956590 = -748175681;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO66760880 = -608756882;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO50745732 = -990543539;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO51495082 = -426307330;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO91789678 = -994891073;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40034739 = -847565763;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55604547 = 83329409;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO18032518 = -183116289;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12373522 = -967238827;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO54757070 = 5366974;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55233834 = -579073606;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6746274 = -808645317;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO90035646 = 73714814;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO33004682 = -449119668;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO85817435 = -592017746;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO63131759 = -965087762;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO23347208 = -850209879;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6329608 = -277234555;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53846059 = -411111088;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55721558 = 60332009;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO27729578 = -313927872;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO84021927 = -301524319;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO64132874 = -286620019;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96996757 = -658848461;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO5332562 = -910054385;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20737935 = -297622503;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO37881238 = -497925000;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO69912147 = -110181735;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO44704329 = 65847308;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO67054879 = -790179552;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO82466479 = -254753940;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20709841 = -333026334;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20389323 = -135158349;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO38827717 = -452152572;    double SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO94962784 = -558522876;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96383657 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO85876586;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO85876586 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71830314;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71830314 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO76367067;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO76367067 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40605459;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40605459 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO35394361;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO35394361 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9262994;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9262994 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO75835073;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO75835073 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO25747509;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO25747509 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO92618248;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO92618248 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO2705338;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO2705338 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO64551572;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO64551572 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO43834544;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO43834544 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO72823986;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO72823986 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO89338745;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO89338745 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO49822234;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO49822234 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9171161;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9171161 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO62950696;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO62950696 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO78047199;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO78047199 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO50122865;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO50122865 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO78345774;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO78345774 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO80387313;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO80387313 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9065455;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9065455 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO99924056;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO99924056 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO92712600;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO92712600 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40391609;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40391609 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO36775562;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO36775562 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96363677;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96363677 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO73432973;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO73432973 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO72495790;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO72495790 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12416158;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12416158 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO30748748;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO30748748 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO97790691;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO97790691 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO93088811;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO93088811 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO21328905;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO21328905 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO97549066;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO97549066 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9787496;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO9787496 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53566614;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53566614 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO44918178;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO44918178 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO65673678;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO65673678 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO95365795;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO95365795 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO23111940;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO23111940 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO73641040;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO73641040 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO19029808;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO19029808 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO66919374;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO66919374 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6895165;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6895165 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO77259849;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO77259849 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO13428355;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO13428355 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO90034070;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO90034070 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO19586914;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO19586914 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO16774233;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO16774233 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO84686579;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO84686579 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO46726821;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO46726821 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO33657818;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO33657818 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96092054;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96092054 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO15996344;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO15996344 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO76811132;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO76811132 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71906258;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71906258 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO83654466;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO83654466 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO213850;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO213850 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO98618799;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO98618799 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12899316;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12899316 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO2402100;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO2402100 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53251718;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53251718 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO80202090;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO80202090 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71956590;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO71956590 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO66760880;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO66760880 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO50745732;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO50745732 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO51495082;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO51495082 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO91789678;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO91789678 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40034739;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO40034739 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55604547;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55604547 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO18032518;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO18032518 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12373522;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO12373522 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO54757070;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO54757070 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55233834;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55233834 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6746274;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6746274 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO90035646;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO90035646 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO33004682;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO33004682 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO85817435;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO85817435 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO63131759;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO63131759 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO23347208;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO23347208 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6329608;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO6329608 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53846059;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO53846059 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55721558;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO55721558 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO27729578;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO27729578 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO84021927;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO84021927 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO64132874;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO64132874 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96996757;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96996757 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO5332562;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO5332562 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20737935;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20737935 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO37881238;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO37881238 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO69912147;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO69912147 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO44704329;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO44704329 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO67054879;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO67054879 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO82466479;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO82466479 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20709841;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20709841 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20389323;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO20389323 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO38827717;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO38827717 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO94962784;     SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO94962784 = SnmHAOEpFfxMmIxfwwJiwtCEBLbJdqmzYuDO96383657;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ocpcTRisBRqKUdttzENvzkuIUQNsweL41841333() {     double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq21683251 = -386723732;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq40120040 = -102147948;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq13845092 = -3541685;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq72813155 = 63906673;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq45347553 = -49122978;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74373953 = -957298256;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq17289929 = -272010166;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq31066608 = -73676343;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq42127138 = 14061458;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq94856207 = -767263073;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq43886399 = -417979795;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87154533 = -283135902;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq19332160 = -306818501;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq3481672 = -782077499;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47454947 = -611681498;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq46682958 = -804951287;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58708511 = -30132738;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73841310 = -915685850;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81403852 = -438149991;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq98617892 = -822559259;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47325340 = -219947373;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71331933 = -878813228;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq84357491 = -922305683;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq78852070 = -764574520;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81032294 = -693942805;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq28804214 = -986510645;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq17247854 = -208205368;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92798428 = -215532543;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58151585 = -722168343;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq82193165 = -729387920;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq4606362 = -204635510;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90975676 = -81688993;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq33844352 = -42136215;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53392516 = -801871352;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq16215337 = -821609294;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87697057 = 95806171;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq40224837 = -463119929;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq89254021 = -564367427;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq9534713 = -133125930;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq56636264 = -945278056;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99254147 = -912682266;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73967250 = 75641659;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73891543 = -448836239;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq93764405 = -585095170;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq6875549 = -857516002;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq9062570 = -265584507;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47562557 = -535631530;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq77853454 = -145717746;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq56476678 = -365413391;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92901486 = -100436907;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73336466 = -47478147;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq30487492 = 27260354;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq8978830 = -308316730;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq91551150 = -38780590;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq94515993 = -86439538;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq50351318 = -507910504;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq55762549 = -179842266;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq34993022 = -238967165;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq91780861 = -242150523;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq16543339 = -62612334;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq57126100 = -649092889;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq24491501 = 43522376;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq72915022 = -351508001;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq59933972 = -256550623;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90249846 = -462627563;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq52910722 = -236290802;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53310181 = -140999688;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65939643 = -504947150;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87266335 = -960468206;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq59757890 = -607487669;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq6458121 = -241831358;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq69454490 = -465765312;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq64306597 = -682559920;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq24767588 = -492871935;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99363745 = -909876993;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73358090 = -195589033;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq97440389 = -329976990;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90593085 = -237210513;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71976521 = -907058519;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71969725 = -328358299;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81241656 = -350879115;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq39394399 = 37512378;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq36321750 = -850119152;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65250099 = -521731436;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq8856699 = -581909774;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74118870 = -131895865;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81996847 = -773372264;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq42293202 = 96644374;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58876523 = -615431814;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65864018 = -213698790;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq31934509 = -724351563;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq5231816 = -124152764;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq97473159 = -222216905;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92991373 = 29486404;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99510164 = -196185168;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74762647 = -856204643;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq1052228 = -572850340;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq13957571 = -92285616;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq3514560 = -22467607;    double GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53964826 = -386723732;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq21683251 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq40120040;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq40120040 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq13845092;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq13845092 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq72813155;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq72813155 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq45347553;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq45347553 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74373953;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74373953 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq17289929;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq17289929 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq31066608;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq31066608 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq42127138;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq42127138 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq94856207;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq94856207 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq43886399;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq43886399 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87154533;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87154533 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq19332160;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq19332160 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq3481672;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq3481672 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47454947;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47454947 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq46682958;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq46682958 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58708511;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58708511 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73841310;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73841310 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81403852;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81403852 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq98617892;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq98617892 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47325340;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47325340 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71331933;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71331933 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq84357491;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq84357491 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq78852070;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq78852070 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81032294;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81032294 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq28804214;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq28804214 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq17247854;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq17247854 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92798428;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92798428 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58151585;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58151585 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq82193165;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq82193165 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq4606362;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq4606362 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90975676;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90975676 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq33844352;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq33844352 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53392516;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53392516 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq16215337;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq16215337 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87697057;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87697057 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq40224837;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq40224837 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq89254021;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq89254021 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq9534713;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq9534713 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq56636264;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq56636264 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99254147;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99254147 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73967250;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73967250 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73891543;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73891543 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq93764405;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq93764405 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq6875549;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq6875549 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq9062570;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq9062570 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47562557;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq47562557 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq77853454;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq77853454 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq56476678;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq56476678 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92901486;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92901486 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73336466;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73336466 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq30487492;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq30487492 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq8978830;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq8978830 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq91551150;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq91551150 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq94515993;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq94515993 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq50351318;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq50351318 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq55762549;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq55762549 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq34993022;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq34993022 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq91780861;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq91780861 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq16543339;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq16543339 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq57126100;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq57126100 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq24491501;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq24491501 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq72915022;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq72915022 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq59933972;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq59933972 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90249846;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90249846 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq52910722;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq52910722 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53310181;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53310181 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65939643;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65939643 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87266335;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq87266335 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq59757890;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq59757890 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq6458121;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq6458121 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq69454490;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq69454490 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq64306597;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq64306597 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq24767588;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq24767588 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99363745;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99363745 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73358090;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq73358090 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq97440389;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq97440389 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90593085;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq90593085 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71976521;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71976521 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71969725;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq71969725 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81241656;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81241656 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq39394399;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq39394399 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq36321750;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq36321750 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65250099;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65250099 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq8856699;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq8856699 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74118870;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74118870 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81996847;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq81996847 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq42293202;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq42293202 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58876523;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq58876523 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65864018;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq65864018 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq31934509;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq31934509 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq5231816;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq5231816 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq97473159;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq97473159 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92991373;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq92991373 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99510164;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq99510164 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74762647;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq74762647 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq1052228;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq1052228 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq13957571;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq13957571 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq3514560;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq3514560 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53964826;     GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq53964826 = GUJKPNEupOZnFzqYQstSGcyADhgICEqwXFjq21683251;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void QxFrpdiiCnkXPKvjIDniubDKcZzVSTX18415291() {     int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul61226867 = -753732690;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul81439278 = 71733490;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul54907116 = -105003522;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul21649805 = -652129076;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul44674646 = -14617475;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60010495 = -174306377;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul12323387 = -713361741;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul35512580 = -84195908;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul42834786 = -132957251;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul59390181 = -840833558;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul9294292 = -924081452;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul21485087 = -537725848;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul61254567 = -872931636;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39323845 = -401777316;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul78275137 = -193626989;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul4630018 = -106218194;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul99891487 = -453962945;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39997375 = 4539671;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86851578 = -892045192;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul16738349 = -642115412;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul10609838 = -518746387;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60315006 = -255372394;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul25189581 = -45846432;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul67588800 = -181236341;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69525298 = -325199693;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69446750 = 56758014;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul83271721 = -564151124;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul42996872 = -376075598;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul45230664 = -658749555;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul889286 = -167144916;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6829091 = -270220044;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul9683333 = -233840733;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul72088351 = -385968356;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91159393 = -80890051;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul32901323 = -171120428;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86589676 = -950599644;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul47939712 = -844651769;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6196685 = -31316324;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul79252435 = -896821119;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul16283793 = -372247938;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul5175382 = 33649146;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul17274061 = -201337166;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul66304831 = -968306265;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul10964807 = -242273098;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91822154 = -724034463;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul71784673 = -28534391;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul67525491 = -460233009;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul87785838 = 96571273;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul23310091 = -662103080;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul46270536 = -358786608;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul68757860 = -51699510;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86802963 = -607626085;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul53822703 = -840002156;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul90385034 = -715443713;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul7805454 = -466930457;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul911861 = -398360297;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul56249698 = -882420079;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul87318315 = -923767181;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul52124506 = -226929384;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul75227896 = 28624511;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul76738774 = -610155253;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69326514 = -237286143;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul90281916 = -425446353;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul41945500 = -965812335;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul52561090 = -470613514;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul99610958 = -590240720;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul49396736 = -51757492;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul70095174 = -692041585;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6422523 = -130656889;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91685460 = -243027345;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul56690306 = -261566426;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul93694803 = -322646621;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60744939 = -98639210;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul70567786 = -419797254;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul11562967 = -575764558;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul93335776 = -217409221;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul94010174 = -287066129;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul14224775 = -803573334;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul75766646 = -457201878;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul97740625 = -196665302;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul1921259 = -483008978;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul95485882 = -560722397;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul19686782 = -713972519;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul98960127 = -199962948;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul32131426 = -15445407;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul20026128 = -662593959;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul55860630 = -393838577;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul81703317 = -670524644;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul83353940 = -613959595;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul31989462 = -772760131;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul30339979 = 31820434;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60621396 = -920884588;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul54072178 = -804386941;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul4024540 = -825445631;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39545018 = -762092685;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul35848868 = -729064711;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul26992144 = -775890814;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul24359332 = 97506070;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul58403716 = -771659585;    int plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul92211196 = -753732690;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul61226867 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul81439278;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul81439278 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul54907116;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul54907116 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul21649805;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul21649805 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul44674646;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul44674646 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60010495;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60010495 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul12323387;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul12323387 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul35512580;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul35512580 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul42834786;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul42834786 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul59390181;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul59390181 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul9294292;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul9294292 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul21485087;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul21485087 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul61254567;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul61254567 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39323845;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39323845 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul78275137;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul78275137 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul4630018;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul4630018 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul99891487;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul99891487 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39997375;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39997375 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86851578;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86851578 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul16738349;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul16738349 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul10609838;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul10609838 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60315006;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60315006 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul25189581;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul25189581 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul67588800;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul67588800 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69525298;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69525298 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69446750;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69446750 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul83271721;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul83271721 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul42996872;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul42996872 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul45230664;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul45230664 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul889286;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul889286 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6829091;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6829091 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul9683333;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul9683333 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul72088351;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul72088351 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91159393;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91159393 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul32901323;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul32901323 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86589676;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86589676 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul47939712;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul47939712 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6196685;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6196685 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul79252435;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul79252435 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul16283793;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul16283793 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul5175382;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul5175382 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul17274061;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul17274061 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul66304831;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul66304831 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul10964807;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul10964807 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91822154;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91822154 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul71784673;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul71784673 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul67525491;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul67525491 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul87785838;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul87785838 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul23310091;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul23310091 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul46270536;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul46270536 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul68757860;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul68757860 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86802963;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul86802963 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul53822703;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul53822703 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul90385034;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul90385034 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul7805454;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul7805454 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul911861;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul911861 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul56249698;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul56249698 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul87318315;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul87318315 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul52124506;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul52124506 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul75227896;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul75227896 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul76738774;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul76738774 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69326514;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul69326514 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul90281916;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul90281916 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul41945500;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul41945500 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul52561090;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul52561090 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul99610958;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul99610958 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul49396736;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul49396736 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul70095174;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul70095174 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6422523;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul6422523 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91685460;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul91685460 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul56690306;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul56690306 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul93694803;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul93694803 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60744939;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60744939 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul70567786;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul70567786 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul11562967;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul11562967 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul93335776;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul93335776 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul94010174;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul94010174 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul14224775;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul14224775 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul75766646;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul75766646 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul97740625;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul97740625 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul1921259;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul1921259 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul95485882;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul95485882 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul19686782;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul19686782 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul98960127;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul98960127 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul32131426;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul32131426 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul20026128;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul20026128 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul55860630;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul55860630 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul81703317;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul81703317 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul83353940;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul83353940 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul31989462;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul31989462 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul30339979;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul30339979 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60621396;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul60621396 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul54072178;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul54072178 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul4024540;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul4024540 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39545018;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul39545018 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul35848868;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul35848868 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul26992144;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul26992144 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul24359332;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul24359332 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul58403716;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul58403716 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul92211196;     plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul92211196 = plRUmkdQAqyhemsZXsRDEWrvptvOPdMQiPul61226867;}
// Junk Finished
