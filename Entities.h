typedef unsigned long CBaseHandle;
#pragma once

#include "MiscDefinitions.h"
#include "ClientRecvProps.h"
#include "offsets.h"
#include "Vector.h"
#include "ShonaxOffsets.h"

#define TEAM_CS_T 2
#define TEAM_CS_CT 3

#define BONE_USED_BY_HITBOX			0x00000100

#ifdef NDEBUG
#define strenc( s ) std::string( cx_make_encrypted_string( s ) )
#define charenc( s ) strenc( s ).c_str()
#define wstrenc( s ) std::wstring( strenc( s ).begin(), strenc( s ).end() )
#define wcharenc( s ) wstrenc( s ).c_str()
#else
#define strenc( s ) ( s )
#define charenc( s ) ( s )
#define wstrenc( s ) ( s )
#define wcharenc( s ) ( s )
#endif

class IClientRenderable;
class IClientNetworkable;
class IClientUnknown;
class IClientThinkable;
class IClientEntity;
class CSWeaponInfo;

struct CHudTexture
{
	char	szShortName[64];	//0x0000
	char	szTextureFile[64];	//0x0040
	bool	bRenderUsingFont;	//0x0080
	bool	bPrecached;			//0x0081
	char	cCharacterInFont;	//0x0082
	BYTE	pad_0x0083;			//0x0083
	int		hFont;				//0x0084
	int		iTextureId;			//0x0088
	float	afTexCoords[4];		//0x008C
	int		iPosX[4];			//0x009C
}; //Size=0x00AC



enum class CSGOClassID
{
	CAI_BaseNPC = 0,
	CAK47,
	CBaseAnimating,
	CBaseAnimatingOverlay,
	CBaseAttributableItem,
	CBaseButton,
	CBaseCombatCharacter,
	CBaseCombatWeapon,
	CBaseCSGrenade,
	CBaseCSGrenadeProjectile,
	CBaseDoor,
	CBaseEntity,
	CBaseFlex,
	CBaseGrenade,
	CBaseParticleEntity,
	CBasePlayer,
	CBasePropDoor,
	CBaseTeamObjectiveResource,
	CBaseTempEntity,
	CBaseToggle,
	CBaseTrigger,
	CBaseViewModel,
	CBaseVPhysicsTrigger,
	CBaseWeaponWorldModel,
	CBeam,
	CBeamSpotlight,
	CBoneFollower,
	CBRC4Target,
	CBreachCharge,
	CBreachChargeProjectile,
	CBreakableProp,
	CBreakableSurface,
	CC4,
	CCascadeLight,
	CChicken,
	CColorCorrection,
	CColorCorrectionVolume,
	CCSGameRulesProxy,
	CCSPlayer,
	CCSPlayerResource,
	CCSRagdoll,
	CCSTeam,
	CDangerZone,
	CDangerZoneController,
	CDEagle,
	CDecoyGrenade,
	CDecoyProjectile,
	CDrone,
	CDronegun,
	CDynamicLight,
	CDynamicProp,
	CEconEntity,
	CEconWearable,
	CEmbers,
	CEntityDissolve,
	CEntityFlame,
	CEntityFreezing,
	CEntityParticleTrail,
	CEnvAmbientLight,
	CEnvDetailController,
	CEnvDOFController,
	CEnvGasCanister,
	CEnvParticleScript,
	CEnvProjectedTexture,
	CEnvQuadraticBeam,
	CEnvScreenEffect,
	CEnvScreenOverlay,
	CEnvTonemapController,
	CEnvWind,
	CFEPlayerDecal,
	CFireCrackerBlast,
	CFireSmoke,
	CFireTrail,
	CFish,
	CFists,
	CFlashbang,
	CFogController,
	CFootstepControl,
	CFunc_Dust,
	CFunc_LOD,
	CFuncAreaPortalWindow,
	CFuncBrush,
	CFuncConveyor,
	CFuncLadder,
	CFuncMonitor,
	CFuncMoveLinear,
	CFuncOccluder,
	CFuncReflectiveGlass,
	CFuncRotating,
	CFuncSmokeVolume,
	CFuncTrackTrain,
	CGameRulesProxy,
	CGrassBurn,
	CHandleTest,
	CHEGrenade,
	CHostage,
	CHostageCarriableProp,
	CIncendiaryGrenade,
	CInferno,
	CInfoLadderDismount,
	CInfoMapRegion,
	CInfoOverlayAccessor,
	CItem_Healthshot,
	CItemCash,
	CItemDogtags,
	CKnife,
	CKnifeGG,
	CLightGlow,
	CMaterialModifyControl,
	CMelee,
	CMolotovGrenade,
	CMolotovProjectile,
	CMovieDisplay,
	CParadropChopper,
	CParticleFire,
	CParticlePerformanceMonitor,
	CParticleSystem,
	CPhysBox,
	CPhysBoxMultiplayer,
	CPhysicsProp,
	CPhysicsPropMultiplayer,
	CPhysMagnet,
	CPhysPropAmmoBox,
	CPhysPropLootCrate,
	CPhysPropRadarJammer,
	CPhysPropWeaponUpgrade,
	CPlantedC4,
	CPlasma,
	CPlayerResource,
	CPointCamera,
	CPointCommentaryNode,
	CPointWorldText,
	CPoseController,
	CPostProcessController,
	CPrecipitation,
	CPrecipitationBlocker,
	CPredictedViewModel,
	CProp_Hallucination,
	CPropCounter,
	CPropDoorRotating,
	CPropJeep,
	CPropVehicleDriveable,
	CRagdollManager,
	CRagdollProp,
	CRagdollPropAttached,
	CRopeKeyframe,
	CSCAR17,
	CSceneEntity,
	CSensorGrenade,
	CSensorGrenadeProjectile,
	CShadowControl,
	CSlideshowDisplay,
	CSmokeGrenade,
	CSmokeGrenadeProjectile,
	CSmokeStack,
	CSpatialEntity,
	CSpotlightEnd,
	CSprite,
	CSpriteOriented,
	CSpriteTrail,
	CStatueProp,
	CSteamJet,
	CSun,
	CSunlightShadowControl,
	CSurvivalSpawnChopper,
	CTablet,
	CTeam,
	CTeamplayRoundBasedRulesProxy,
	CTEArmorRicochet,
	CTEBaseBeam,
	CTEBeamEntPoint,
	CTEBeamEnts,
	CTEBeamFollow,
	CTEBeamLaser,
	CTEBeamPoints,
	CTEBeamRing,
	CTEBeamRingPoint,
	CTEBeamSpline,
	CTEBloodSprite,
	CTEBloodStream,
	CTEBreakModel,
	CTEBSPDecal,
	CTEBubbles,
	CTEBubbleTrail,
	CTEClientProjectile,
	CTEDecal,
	CTEDust,
	CTEDynamicLight,
	CTEEffectDispatch,
	CTEEnergySplash,
	CTEExplosion,
	CTEFireBullets,
	CTEFizz,
	CTEFootprintDecal,
	CTEFoundryHelpers,
	CTEGaussExplosion,
	CTEGlowSprite,
	CTEImpact,
	CTEKillPlayerAttachments,
	CTELargeFunnel,
	CTEMetalSparks,
	CTEMuzzleFlash,
	CTEParticleSystem,
	CTEPhysicsProp,
	CTEPlantBomb,
	CTEPlayerAnimEvent,
	CTEPlayerDecal,
	CTEProjectedDecal,
	CTERadioIcon,
	CTEShatterSurface,
	CTEShowLine,
	CTesla,
	CTESmoke,
	CTESparks,
	CTESprite,
	CTESpriteSpray,
	CTest_ProxyToggle_Networkable,
	CTestTraceline,
	CTEWorldDecal,
	CTriggerPlayerMovement,
	CTriggerSoundOperator,
	CVGuiScreen,
	CVoteController,
	CWaterBullet,
	CWaterLODControl,
	CWeaponAug,
	CWeaponAWP,
	CWeaponBaseItem,
	CWeaponBizon,
	CWeaponCSBase,
	CWeaponCSBaseGun,
	CWeaponCycler,
	CWeaponElite,
	CWeaponFamas,
	CWeaponFiveSeven,
	CWeaponG3SG1,
	CWeaponGalil,
	CWeaponGalilAR,
	CWeaponGlock,
	CWeaponHKP2000,
	CWeaponM249,
	CWeaponM3,
	CWeaponM4A1,
	CWeaponMAC10,
	CWeaponMag7,
	CWeaponMP5Navy,
	CWeaponMP7,
	CWeaponMP9,
	CWeaponNegev,
	CWeaponNOVA,
	CWeaponP228,
	CWeaponP250,
	CWeaponP90,
	CWeaponSawedoff,
	CWeaponSCAR20,
	CWeaponScout,
	CWeaponSG550,
	CWeaponSG552,
	CWeaponSG556,
	CWeaponSSG08,
	CWeaponTaser,
	CWeaponTec9,
	CWeaponTMP,
	CWeaponUMP45,
	CWeaponUSP,
	CWeaponXM1014,
	CWorld,
	CWorldVguiText,
	DustTrail,
	MovieExplosion,
	ParticleSmokeGrenade,
	RocketTrail,
	SmokeTrail,
	SporeExplosion,
	SporeTrail,
};

enum ItemDefinitionIndex
{
	WEAPON_DEAGLE = 1,
	WEAPON_MP5 = 23,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE_CT = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034

};

class CSWeaponInfo
{
public:

public:
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* hud_name;				//0x0088 
	char* weapon_name;			//0x008C 
	char _0x0090[60];
	__int32 type;				//0x00CC 
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	BYTE full_auto;				//0x00EC 
	char _0x00ED[3];
	__int32 damage;				//0x00F0 
	float armor_ratio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float penetration;			//0x00FC 
	char _0x0100[8];
	float range;				//0x0108 
	float range_modifier;		//0x010C 
	char _0x0110[16];
	BYTE silencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};

enum class CSGOHitboxID
{
	Head = 0,
	Neck,
	Pelvis,
	Stomach,
	LowerChest,
	Chest,
	UpperChest,
	RightThigh,
	LeftThigh,
	RightShin,
	LeftShin,
	RightFoot,
	LeftFoot,
	RightHand,
	LeftHand,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
	Max,
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

class ScriptCreatedItem
{
public:
	CPNETVAR_FUNC(short*, ItemDefinitionIndex, 0xE67AB3B8); //m_iItemDefinitionIndex
	CPNETVAR_FUNC(int*, AccountID, 0x24abbea8); //m_iAccountID
	CPNETVAR_FUNC(int*, ItemIDHigh, 0x714778A); //m_iItemIDHigh
	CPNETVAR_FUNC(int*, ItemIDLow, 0x3A3DFC74); //m_iItemIDLow
	CPNETVAR_FUNC(char**, szCustomName, 0x13285ad9); //m_szCustomName
	CPNETVAR_FUNC(int*, EntityQuality, 0x110be6fe); //m_iEntityQuality
};

class AttributeContainer
{
public:
	CPNETVAR_FUNC(ScriptCreatedItem*, m_Item, 0x7E029CE5);
};
#define EVENT_DEBUG_ID_INIT 42 

class CBaseCombatWeapon
{
public:
	CNETVAR_FUNC(float, GetNextPrimaryAttack, 0xDB7B106E); //m_flNextPrimaryAttack
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin
	CNETVAR_FUNC(int, GetZoomLevel, 0x26553F1A);
	CPNETVAR_FUNC(AttributeContainer*, m_AttributeManager, 0xCFFCE089);

	int GetAmmoInClip()
	{
		static int m_Clip = GET_NETVAR("DT_BaseCombatWeapon", "m_iClip1");
		return *(int*)((DWORD)this + m_Clip);
	}
	float GetSpread() { return call_vfunc<float(__thiscall*)(void*)>(this, 439)(this); }
	CSWeaponInfo* GetCSWpnData() { return call_vfunc<CSWeaponInfo*(__thiscall*)(void*)>(this, 448)(this); }
	float GetCone() { return call_vfunc<float(__thiscall*)(void*)>(this, 471)(this); }
	void UpdateAccuracyPenalty() { call_vfunc<void(__thiscall*)(void*)>(this, 472)(this); }
	float GetAccuracyPenalty()
	{
		static int m_fAccuracyPenalty = GET_NETVAR("DT_WeaponCSBase", "m_fAccuracyPenalty");
		return *(float*)((DWORD)this + m_fAccuracyPenalty);
	}
	short fix()
	{
		return *(short*)((uintptr_t)this + 0x2FAA);
	}
	bool IsScoped(int x = 0)
	{
		return GetZoomLevel() > 0;
	}
	bool HitChance(float flChance)
	{
		if (flChance >= 99.f)
			flChance = 99.f;

		if (flChance < 1)
			flChance = 1;

		float flSpread = GetCone();
		return((((100.f - flChance) * 0.65f) * 0.01125) >= flSpread);
	}

	ScriptCreatedItem* GetItem()
	{
		return m_AttributeManager()->m_Item();
	}
	short* GetItemDefinitionIndex()
	{
		return (short*)m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	}
	int* GetAccountID()
	{
		return (int*)m_AttributeManager()->m_Item()->AccountID();
	}
	int* GetItemIDHigh()
	{
		return (int*)m_AttributeManager()->m_Item()->ItemIDHigh();
	}
	int* GetItemIDLow()
	{
		return (int*)m_AttributeManager()->m_Item()->ItemIDLow();
	}
	char** GetCustomName()
	{
		return (char**)m_AttributeManager()->m_Item()->szCustomName();
	}
	int* GetEntityQuality()
	{
		return (int*)m_AttributeManager()->m_Item()->EntityQuality();
	}
	bool IsKnife()
	{
		short* Index = GetItemDefinitionIndex();
		if (*Index == 42 || *Index == 59 || *Index >= 500)
			return true;
		return false;
	}
	bool IsGrenade()
	{ 
		short* Index = GetItemDefinitionIndex();
		if (*Index == 43 || *Index == 44 || *Index == 45 || *Index == 46 || *Index == 47 || *Index == 48)
			return true;
		return false;
	}
	bool IsC4()
	{
		short* Index = GetItemDefinitionIndex();
		return *Index == 49;
	}
	bool m_bIsSniper()
	{
		short WeaponId = *this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_AWP || WeaponId == WEAPON_SCAR20 || WeaponId == WEAPON_G3SG1 || WeaponId == WEAPON_SSG08;
	}
	bool m_bIsPistol()
	{
		short weapon_id = *this->GetItemDefinitionIndex();

		return weapon_id == WEAPON_DEAGLE || weapon_id == WEAPON_ELITE || weapon_id == WEAPON_FIVESEVEN || weapon_id == WEAPON_P250 ||
			weapon_id == WEAPON_GLOCK || weapon_id == WEAPON_HKP2000 || weapon_id == WEAPON_CZ75A || weapon_id == WEAPON_USP_SILENCER || weapon_id == WEAPON_TEC9 || weapon_id == WEAPON_REVOLVER;
	}
	bool m_bIsSmg()
	{
		short weapon_id = *this->GetItemDefinitionIndex();

		return weapon_id == WEAPON_MAC10 || weapon_id == WEAPON_MP7 || weapon_id == WEAPON_MP9 || weapon_id == WEAPON_P90 ||
			weapon_id == WEAPON_BIZON || weapon_id == WEAPON_UMP45;
	}
	bool m_bIsShotgun()
	{
		short weapon_id = *this->GetItemDefinitionIndex();

		return weapon_id == WEAPON_XM1014 || weapon_id == WEAPON_NOVA || weapon_id == WEAPON_SAWEDOFF || weapon_id == WEAPON_MAG7;
	}
	bool m_bIsReloading();
	float GetFireReadyTime()
	{
		return *(float*)((DWORD)this + GET_NETVAR("DT_WeaponCSBase", "m_flPostponeFireReadyTime"));
	}
	char* GetGunIcon()
	{
		short WeaponId = *this->GetItemDefinitionIndex();
		switch (WeaponId)
		{
		case WEAPON_KNIFE_CT:
		case WEAPON_KNIFE_T:
		case 500:
		case 505:
		case 506:
		case 507:
		case 508:
		case 509:
		case 512:
		case 514:
		case 515:
		case 516:
			return "]";
		case WEAPON_DEAGLE:
			return "A";
		case WEAPON_ELITE:
			return "B";
		case WEAPON_FIVESEVEN:
			return "C";
		case WEAPON_GLOCK:
			return "D";
		case WEAPON_HKP2000:
			return "E";
		case WEAPON_P250:
			return "F";
		case WEAPON_USP_SILENCER:
			return "G";
		case WEAPON_TEC9:
			return "H";
		case WEAPON_CZ75A:
			return "I";
		case WEAPON_REVOLVER:
			return "J";
		case WEAPON_MAC10:
			return "K";
		case WEAPON_UMP45:
			return "L";
		case WEAPON_BIZON:
			return "M";
		case WEAPON_MP7:
			return "N";
		case WEAPON_MP9:
			return "O";
		case WEAPON_P90:
			return "P";
		case WEAPON_GALILAR:
			return "Q";
		case WEAPON_FAMAS:
			return "R";
		case WEAPON_M4A1_SILENCER:
			return "S";
		case WEAPON_M4A1:
			return "T";
		case WEAPON_AUG:
			return "U";
		case WEAPON_SG556:
			return "V";
		case WEAPON_AK47:
			return "W";
		case WEAPON_G3SG1:
			return "X";
		case WEAPON_SCAR20:
			return "Y";
		case WEAPON_AWP:
			return "Z";
		case WEAPON_SSG08:
			return "a";
		case WEAPON_XM1014:
			return "b";
		case WEAPON_SAWEDOFF:
			return "c";
		case WEAPON_MAG7:
			return "d";
		case WEAPON_NOVA:
			return "e";
		case WEAPON_NEGEV:
			return "f";
		case WEAPON_M249:
			return "g";
		case WEAPON_TASER:
			return "h";
		case WEAPON_FLASHBANG:
			return "i";
		case WEAPON_HEGRENADE:
			return "j";
		case WEAPON_SMOKEGRENADE:
			return "k";
		case WEAPON_MOLOTOV:
			return "l";
		case WEAPON_DECOY:
			return "m";
		case WEAPON_INCGRENADE:
			return "n";
		case WEAPON_C4:
			return "o";
		default:
			return " ";
		}
	}
	char* GetGunName()
	{
		short WeaponId = *this->GetItemDefinitionIndex();
		switch (WeaponId)
		{
		case WEAPON_AK47:
			return "AK47";
			break;
		case WEAPON_AUG:
			return "AUG";
			break;
		case WEAPON_REVOLVER:
			return "R8-REVOLVER";
			break;
		case WEAPON_AWP:
			return "AWP";
			break;
		case WEAPON_BIZON:
			return "BIZON";
			break;
		case WEAPON_C4:
			return "C4";
			break;
		case WEAPON_CZ75A:
			return "CZ75";
			break;
		case WEAPON_DEAGLE:
			return "DESERT EAGLE";
			break;
		case WEAPON_DECOY:
			return "DECOY";
			break;
		case WEAPON_ELITE:
			return "DUALBERETTAS";
			break;
		case WEAPON_FAMAS:
			return "FAMAS";
			break;
		case WEAPON_FIVESEVEN:
			return "FIVESEVEN";
			break;
		case WEAPON_FLASHBANG:
			return "FLASH";
			break;
		case WEAPON_G3SG1:
			return "G3SG1";
			break;
		case WEAPON_GALILAR:
			return "GALIL";
			break;
		case WEAPON_GLOCK:
			return "GLOCK";
			break;
		case WEAPON_INCGRENADE:
			return "INCENDENARY";
			break;
		case WEAPON_MOLOTOV:
			return "MOLOTOV";
			break;
		case WEAPON_SSG08:
			return "SSG08";
			break;
		case WEAPON_HEGRENADE:
			return "HEGRENADE";
			break;
		case WEAPON_M249:
			return "M249";
			break;
		case WEAPON_M4A1:
			return "M4A1";
			break;
		case WEAPON_MAC10:
			return "MAC10";
			break;
		case WEAPON_MAG7:
			return "MAG7";
			break;
		case WEAPON_MP7:
			return "MP7";
			break;
		case WEAPON_MP9:
			return "MP9";
			break;
		case WEAPON_NOVA:
			return "NOVA";
			break;
		case WEAPON_NEGEV:
			return "NEGEV";
			break;
		case WEAPON_P250:
			return "P250";
			break;
		case WEAPON_P90:
			return "P90";
			break;
		case WEAPON_SAWEDOFF:
			return "SAWEDOFF";
			break;
		case WEAPON_SCAR20:
			return "SCAR-20";
			break;
		case WEAPON_SMOKEGRENADE:
			return "SMOKE";
			break;
		case WEAPON_SG556:
			return "SG553";
			break;
		case WEAPON_TEC9:
			return "TEC9";
			break;
		case WEAPON_HKP2000:
			return "P2000";
			break;
		case WEAPON_USP_SILENCER:
			return "USP-S";
			break;
		case WEAPON_UMP45:
			return "UMP45";
			break;
		case WEAPON_XM1014:
			return "XM1014";
			break;
		case WEAPON_TASER:
			return "TASER";
			break;
		case WEAPON_M4A1_SILENCER:
			return "M4A1-S";
		}
		if (IsKnife())
			return "KNIFE";
		return "";
	}
};

class CCSBomb
{
public:
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(float, GetC4BlowTime, 0xB5E0CA1C); //m_flC4Blow
	CNETVAR_FUNC(float, GetC4DefuseCountDown, 0xB959B4A6); //m_flDefuseCountDown
};

class CLocalPlayerExclusive
{
public:
	CNETVAR_FUNC(Vector, GetViewPunchAngle, 0x68F014C0);//m_viewPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngle, 0xBF25C290);//m_aimPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngleVel, 0x8425E045);//m_aimPunchAngleVel
};

class CollisionProperty
{
public:
	CNETVAR_FUNC(Vector, GetMins, 0xF6F78BAB);//m_vecMins
	CNETVAR_FUNC(Vector, GetMaxs, 0xE47C6FC4);//m_vecMaxs
	CNETVAR_FUNC(unsigned char, GetSolidType, 0xB86722A1);//m_nSolidType
	CNETVAR_FUNC(unsigned short, GetSolidFlags, 0x63BB24C1);//m_usSolidFlags
	CNETVAR_FUNC(int, GetSurroundsType, 0xB677A0BB); //m_nSurroundType

	bool IsSolid()
	{
		return (GetSolidType() != SOLID_NONE) && ((GetSolidFlags() & FSOLID_NOT_SOLID) == 0);
	}
};

class IClientRenderable
{
public:
	virtual IClientUnknown*            GetIClientUnknown( ) = 0;
	virtual Vector const&              GetRenderOrigin( void ) = 0;
	virtual Vector const&              GetRenderAngles( void ) = 0;
	virtual bool                       ShouldDraw( void ) = 0;
	virtual int                        GetRenderFlags( void ) = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused( void ) const {}
	virtual void*				       GetShadowHandle( ) const = 0;
	virtual void   				 	   RenderHandle( ) = 0;
	virtual const model_t*             GetModel( ) const = 0;
	virtual int                        DrawModel( int flags, const int /*RenderableInstance_t*/ &instance ) = 0;
	virtual int                        GetBody( ) = 0;
	virtual void                       GetColorModulation( float* color ) = 0;
	virtual bool                       LODTest( ) = 0;
	virtual bool                       SetupBones( matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime ) = 0;
	virtual void                       SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights ) = 0;
	virtual void                       DoAnimationEvents( void ) = 0;
	virtual void* /*IPVSNotify*/       GetPVSNotifyInterface( ) = 0;
	virtual void                       GetRenderBounds( Vector& mins, Vector& maxs ) = 0;
	virtual void                       GetRenderBoundsWorldspace( Vector& mins, Vector& maxs ) = 0;
	virtual void                       GetShadowRenderBounds( Vector &mins, Vector &maxs, int /*ShadowType_t*/ shadowType ) = 0;
	virtual bool                       ShouldReceiveProjectedTextures( int flags ) = 0;
	virtual bool                       GetShadowCastDistance( float *pDist, int /*ShadowType_t*/ shadowType ) const = 0;
	virtual bool                       GetShadowCastDirection( Vector *pDirection, int /*ShadowType_t*/ shadowType ) const = 0;
	virtual bool                       IsShadowDirty( ) = 0;
	virtual void                       MarkShadowDirty( bool bDirty ) = 0;
	virtual IClientRenderable*         GetShadowParent( ) = 0;
	virtual IClientRenderable*         FirstShadowChild( ) = 0;
	virtual IClientRenderable*         NextShadowPeer( ) = 0;
	virtual int /*ShadowType_t*/       ShadowCastType( ) = 0;
	virtual void                       CreateModelInstance( ) = 0;
	virtual ModelInstanceHandle_t      GetModelInstance( ) = 0;
	virtual const matrix3x4&           RenderableToWorldTransform( ) = 0;
	virtual int                        LookupAttachment( const char *pAttachmentName ) = 0;
	virtual   bool                     GetAttachment( int number, Vector &origin, Vector &angles ) = 0;
	virtual bool                       GetAttachment( int number, matrix3x4_t &matrix ) = 0;
	virtual float*                     GetRenderClipPlane( void ) = 0;
	virtual int                        GetSkin( ) = 0;
	virtual void                       OnThreadedDrawSetup( ) = 0;
	virtual bool                       UsesFlexDelayedWeights( ) = 0;
	virtual void                       RecordToolMessage( ) = 0;
	virtual bool                       ShouldDrawForSplitScreenUser( int nSlot ) = 0;
	virtual uint8_t                      OverrideAlphaModulation( uint8_t nAlpha ) = 0;
	virtual uint8_t                      OverrideShadowAlphaModulation( uint8_t nAlpha ) = 0;
};
#define VirtualFn( cast ) typedef cast( __thiscall* OriginalFn )


enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};
class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;// FOR NETVARS FIND YOURSELF ClientClass* stuffs
	virtual void			NotifyShouldTransmit( /* ShouldTransmitState_t state*/) = 0;
	virtual void			OnPreDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			OnDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PreDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PostDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			unknown();
	virtual bool			IsDormant(void) = 0;
	virtual int				GetIndex(void) const = 0;
	virtual void			ReceiveMessage(int classID /*, bf_read &msg*/) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;


	void SetModelIndex(int nModelIndex)
	{

		VirtualFn(void)(PVOID, int);
		call_vfunc< OriginalFn >(this, 75)(this, nModelIndex);
	}
	void PreDataUpdate(DataUpdateType_t updateType)
	{
		PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
		typedef void(__thiscall* OriginalFn)(PVOID, int);
		return call_vfunc<OriginalFn>(pNetworkable, 6)(pNetworkable, updateType);
	}
};

class IClientUnknown
{
public:
	virtual void*		GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IClientEntity*		GetIClientEntity() = 0;
	virtual IClientEntity*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual void*				GetThinkHandle() = 0;
	virtual void				SetThinkHandle(void* hThink) = 0;
	virtual void				Release() = 0;
};

class __declspec (novtable)IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
	template<class T>
	inline T GetFieldValue(int offset) {
		return *(T*)((DWORD)this + offset);
	}
	template<class T>
	T* GetFieldPointer(int offset) {
		return (T*)((DWORD)this + offset);
	}
public:
public:
	virtual void			Release(void) = 0;
	virtual Vector&			GetAbsOrigin(void) const = 0;
	
	//---                 NetVars                  ---//

	CPNETVAR_FUNC(CLocalPlayerExclusive*, localPlayerExclusive, 0x7177BC3E);// m_Local
	CPNETVAR_FUNC(CollisionProperty*, collisionProperty, 0xE477CBD0);//m_Collision

	CNETVAR_FUNC(int, GetFlags, 0xE456D580); //m_fFlags
	CPNETVAR_FUNC(int*, GetPointerFlags, 0xE456D580);
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin
	CNETVAR_FUNC(Vector, GetRotation, 0x6BEA197A); //m_angRotation
	CNETVAR_FUNC(int, GetTeamNum, 0xC08B6C6E); //m_iTeamNum
	CNETVAR_FUNC(int, GetMaxHealth, 0xC52E1C28); //m_iMaxHealth
	CNETVAR_FUNC(int, GetHealth, 0xA93054E3); //m_iHealth
	CNETVAR_FUNC(unsigned char, GetLifeState, 0xD795CCFC); //m_lifeState
	CNETVAR_FUNC(HANDLE, GetActiveWeaponHandle, 0xB4FECDA3); //m_hActiveWeapon
	CNETVAR_FUNC(int, GetTickBase, 0xD472B079); //m_nTickBase
	CNETVAR_FUNC(Vector, GetViewOffset, 0xA9F74931); //m_vecViewOffset[0]
	CNETVAR_FUNC(Vector, GetVelocity, 0x40C1CA24); //m_vecVelocity[0]
	CNETVAR_FUNC(bool, IsDefusing, 0xA2C14106); //m_bIsDefusing
	CNETVAR_FUNC(int, ArmorValue, 0x3898634); //m_ArmorValue
	CNETVAR_FUNC(bool, HasHelmet, 0x7B97F18A); //m_bHasHelmet
	CNETVAR_FUNC(bool, IsScoped, 0x61B9C22C); //m_bIsScoped
	CNETVAR_FUNC(int, GetMoney, 0xF4B3E183); //m_iAccount

	bool m_bGunGameImmunity();
	float m_flSimulationTime();
	float m_flOldSimulationTime();
	int m_iFOV();
	int m_iDefaultFOV();
	CBaseHandle * m_hMyWeapons();
	Vector* GetEyeAngles();
	float GetLowerBodyYaw();
	Vector* GetHeadRotation();
	CBaseCombatWeapon* GetWeapon();
	int GetObserverMode();
	char* GetLastPlaceName();
	int GetMoveType();
	float GetMaxSpeed();
	bool IsInAir();
	bool is_grounded();
	bool SetupBones2(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	Vector GetVecOrigin();
	float GetCycle();
	int GetSequence();
	float GetPoseParams(int idx);
	Vector GetAbsOrigin2() {
		__asm {
			MOV ECX, this
			MOV EAX, DWORD PTR DS : [ECX]
			CALL DWORD PTR DS : [EAX + 0x28]
		}
	}
	Vector GetEyePosition() {
		Vector Origin = *(Vector*)((DWORD)this + 0x138);
		Vector View = *(Vector*)((DWORD)this + 0x108);
		return(Origin + View);
	}
	int GetChokedPackets();
	inline UINT* GetWeapons() {
		return (UINT*)((DWORD)this + 0x2DF8);
	}


	int GetActiveWeaponIndex()
	{
		return *reinterpret_cast<int*>(uintptr_t(this) + 0x2EF8) & 0xFFF;
	}

	bool IsPlayer()
	{
		if (!this)
			return false;

		if (this->IsDormant())
			return false;

		if (this->GetTeamNum() == 0)
			return false;

		ClientClass* cClass = (ClientClass*)this->GetClientClass();

		if (cClass->m_ClassID != 38)
		{
			return false;
		}
		return true;
	}
	Vector* ViewPunch();
	Vector* AimPunch();
	Vector GetNetworkedOrigin();
	Vector GetAbsOriginal()
	{
		typedef Vector&(__thiscall *o_getAbsOriginal)(void*);
		return call_vfunc<o_getAbsOriginal>(this, 10)(this);
	}
	Vector GetAbsAngles()
	{
		typedef Vector&(__thiscall *o_getAbsAechse)(void*);
		return call_vfunc<o_getAbsAechse>(this, 11)(this);
	}
	/*Vector GetAbsAngles2() {
		__asm {
			MOV ECX, this;
			MOV EAX, DWORD PTR DS : [ECX];
			CALL DWORD PTR DS : [EAX + 0x2C]
		}
	}*/

	void SetAbsOriginal(Vector origin);
	void SetAbsAngles(Vector angle);
	void UpdateClientSideAnimation()
	{
		typedef void(__thiscall *o_updateClientSideAnimation)(void*);
		call_vfunc<o_updateClientSideAnimation>(this, 219)(this);
	}
	void SetAngle2( Vector wantedang );
	int draw_model(int flags, uint8_t alpha) {
		using fn = int(__thiscall*)(void*, int, uint8_t);
		return call_vfunc< fn >(GetClientRenderable(), 9)(GetClientRenderable(), flags, alpha);
	}

	CNETVAR_FUNC(HANDLE, GetObserverTargetHandle, 0x8660FD83); //m_hObserverTarget
	inline float getnextattack() { return nigvar(float, this, 0x2D60); }
	// ----------------------------------------------//
	bool IsAlive()
	{
		return (GetLifeState() == LIFE_ALIVE && GetHealth() > 0);
	}

	Vector GetBonePos(int i)
	{
		matrix3x4 boneMatrix[128];
		if (this->SetupBones(boneMatrix, 128, BONE_USED_BY_HITBOX, GetTickCount64()))
		{
			return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
		}
		return Vector(0, 0, 0);
	}

	Vector GetVecVelocity()
	{
		return *(Vector*)((DWORD)this + 0x114);
	}

	Vector GetHeadPos()
	{
		return this->GetBonePos(6);
	}
};

class CBaseAttributableItem : public IClientUnknown, public IClientRenderable, public IClientNetworkable {
public:
	inline short* GetItemDefinitionIndex() {
		return (short*)((DWORD)this + ShonaxOffsets::m_iItemDefinitionIndex);
	}
	inline int* GetItemIDHigh() {
		return (int*)((DWORD)this + ShonaxOffsets::m_iItemIDHigh);
	}
	inline int* GetAccountID() {
		return (int*)((DWORD)this + ShonaxOffsets::m_iAccountID);
	}
	inline int* GetEntityQuality() {
		return (int*)((DWORD)this + ShonaxOffsets::m_iEntityQuality);
	}
	inline char* GetCustomName() {
		return (char*)((DWORD)this + 0x303C);
	}
	inline int* GetOriginalOwnerXuidLow() {
		return (int*)((DWORD)this + 0x31B0);
	}
	inline int* GetOriginalOwnerXuidHigh() {
		return (int*)((DWORD)this + 0x31B4);
	}
	inline int* GetFallbackPaintKit() {
		return (int*)((DWORD)this + ShonaxOffsets::m_nFallbackPaintKit);
	}
	int* ModelIndex()
	{
		return (int*)((uintptr_t)this + 0x258);
	}
	int* ViewModelIndex()
	{
		return (int*)((uintptr_t)this + 0x3220);
	}
	HANDLE m_hWeaponWorldModel()
	{
		return *(HANDLE*)((uintptr_t)this + 0x3234);
	}
	short* fixskins()
	{
		return (short*)((uintptr_t)this + ShonaxOffsets::m_iItemDefinitionIndex);
	}
	short getdefen()
	{
		return (short)((uintptr_t)this + ShonaxOffsets::m_iItemDefinitionIndex);
	}
	
	inline int* GetFallbackSeed() {
		return (int*)((DWORD)this + ShonaxOffsets::m_nFallbackSeed);
	}
	inline float* GetFallbackWear() {
		return (float*)((DWORD)this + ShonaxOffsets::m_flFallbackWear);
	}
	inline int* GetFallbackStatTrak() {
		return (int*)((DWORD)this + ShonaxOffsets::m_nFallbackStatTrak);
	}

	inline int GetModelIndex() {
		return *(int*)((DWORD)this + ShonaxOffsets::m_nModelIndex);
	}

	inline void SetModelIndex(int nModelIndex) {
		*(int*)((DWORD)this + 0x258) = nModelIndex;
	}
};
enum
{
	TE_BEAMPOINTS = 0x00,		// beam effect between two points
	TE_SPRITE = 0x01,	// additive sprite, plays 1 cycle
	TE_BEAMDISK = 0x02,	// disk that expands to max radius over lifetime
	TE_BEAMCYLINDER = 0x03,		// cylinder that expands to max radius over lifetime
	TE_BEAMFOLLOW = 0x04,		// create a line of decaying beam segments until entity stops moving
	TE_BEAMRING = 0x05,		// connect a beam ring to two entities
	TE_BEAMSPLINE = 0x06,
	TE_BEAMRINGPOINT = 0x07,
	TE_BEAMLASER = 0x08,		// Fades according to viewpoint
	TE_BEAMTESLA = 0x09,
};
enum
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,		// Only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,		// Has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,		// Has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,		// When drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
	NUM_BEAM_FLAGS = 17	// KEEP THIS UPDATED!
};
//#define TE_BEAMPOINTS 0		// beam effect between two points
class C_Beam;
class Beam_t;

struct BeamInfo_t
{
	int			m_nType;

	// Entities
	IClientEntity* m_pStartEnt;
	int			m_nStartAttachment;
	IClientEntity* m_pEndEnt;
	int			m_nEndAttachment;

	// Points
	Vector		m_vecStart;
	Vector		m_vecEnd;

	int			m_nModelIndex;
	const char	*m_pszModelName;

	int			m_nHaloIndex;
	const char	*m_pszHaloName;
	float		m_flHaloScale;

	float		m_flLife;
	float		m_flWidth;
	float		m_flEndWidth;
	float		m_flFadeLength;
	float		m_flAmplitude;

	float		m_flBrightness;
	float		m_flSpeed;

	int			m_nStartFrame;
	float		m_flFrameRate;

	float		m_flRed;
	float		m_flGreen;
	float		m_flBlue;

	bool		m_bRenderable;

	int			m_nSegments;

	int			m_nFlags;

	// Rings
	Vector		m_vecCenter;
	float		m_flStartRadius;
	float		m_flEndRadius;

	BeamInfo_t()
	{
		m_nType = TE_BEAMPOINTS;
		m_nSegments = -1;
		m_pszModelName = NULL;
		m_pszHaloName = NULL;
		m_nModelIndex = -1;
		m_nHaloIndex = -1;
		m_bRenderable = true;
		m_nFlags = 0;
	}
};

class ITraceFilter;
class ITexture;

class IViewRenderBeams
{
public:
	// Construction
public:
	IViewRenderBeams(void);
	virtual				~IViewRenderBeams(void);

	// Implement IViewRenderBeams
public:
	virtual	void		InitBeams(void);
	virtual	void		ShutdownBeams(void);
	virtual	void		ClearBeams(void);

	// Updates the state of the temp ent beams
	virtual void		UpdateTempEntBeams();

	virtual void		DrawBeam(Beam_t *pbeam);
	virtual void		DrawBeam(C_Beam* pbeam, ITraceFilter *pEntityBeamTraceFilter = NULL);

	virtual	void		KillDeadBeams(IClientEntity *pDeadEntity);

	virtual	void		CreateBeamEnts(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1);
	virtual Beam_t		*CreateBeamEnts(BeamInfo_t &beamInfo);

	virtual	void		CreateBeamEntPoint(int	nStartEntity, const Vector *pStart, int nEndEntity, const Vector* pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b);
	virtual Beam_t		*CreateBeamEntPoint(BeamInfo_t &beamInfo);

	virtual	void		CreateBeamPoints(Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b);
	virtual	Beam_t		*CreateBeamPoints(BeamInfo_t &beamInfo);

	virtual	void		CreateBeamRing(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags);
	virtual Beam_t		*CreateBeamRing(BeamInfo_t &beamInfo);

	virtual void		CreateBeamRingPoint(const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags);
	virtual Beam_t		*CreateBeamRingPoint(BeamInfo_t &beamInfo);

	virtual	void		CreateBeamCirclePoints(int type, Vector& start, Vector& end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float endWidth, float fadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b);
	virtual Beam_t		*CreateBeamCirclePoints(BeamInfo_t &beamInfo);

	virtual	void		CreateBeamFollow(int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float r, float g, float b,
		float brightness);
	virtual Beam_t		*CreateBeamFollow(BeamInfo_t &beamInfo);
	/*virtual void	InitBeams(void) = 0;
	virtual void	ShutdownBeams(void) = 0;
	virtual void	ClearBeams(void) = 0;

	// Updates the state of the temp ent beams
	virtual void	UpdateTempEntBeams() = 0;

	virtual void	DrawBeam(C_Beam* pbeam, ITraceFilter *pEntityBeamTraceFilter = NULL) = 0;
	virtual void	DrawBeam(Beam_t *pbeam) = 0;

	virtual void	KillDeadBeams(IClientEntity* pEnt) = 0;

	// New interfaces!
	virtual Beam_t	*CreateBeamEnts(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamEntPoint(BeamInfo_t &beamInfo) = 0;
	virtual	Beam_t	*CreateBeamPoints(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamRing(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamRingPoint(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamCirclePoints(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamFollow(BeamInfo_t &beamInfo) = 0;

	virtual void	FreeBeam(Beam_t *pBeam) = 0;
	virtual void	UpdateBeamInfo(Beam_t *pBeam, BeamInfo_t &beamInfo) = 0;*/
};
class C_PlantedC4 : public IClientEntity
{
public:
	bool IsBombTicking()
	{
		return (bool)((uintptr_t)this + GET_NETVAR("DT_PlantedC4", "m_bBombTicking"));
	}

	float GetBombTime()
	{
		return *(float*)((uintptr_t)this + GET_NETVAR("DT_PlantedC4", "m_flC4Blow"));
	}

	bool IsBombDefused()
	{
		return *(bool*)((uintptr_t)this + GET_NETVAR("DT_PlantedC4", "m_bBombDefused"));
	}

	int GetBombDefuser()
	{
		return *(int*)((uintptr_t)this + GET_NETVAR("DT_PlantedC4", "m_hBombDefuser"));
	}
};






class CBaseCSGrenade : CBaseCombatWeapon
{
public:

	float GetThrowTime()
	{
		uintptr_t thr = 0x3344;
		return *(float*)((uintptr_t)this + thr);
	}
	float throw_time()
	{
		return *reinterpret_cast<float*>(uintptr_t(this) + ShonaxOffsets::m_fThrowTime);
	}
};