#pragma once


#include "CommonIncludes.h"
#define STICKERS_COUNT 5
class Weapon_tTT
{
public:
	int SkinsWeapon;
	int SkinsKnife;
	int VremennyiWeapon;
	int VremennyiKnife;
	int Stikers1;
	int Stikers2;
	int Stikers3;
	int Stikers4;
	float ChangerWear = 0;
	int ChangerStatTrak = 0;
	char ChangerName[32] = "";
	bool ChangerEnabled;
};

namespace Shonax
{
	class Sticker_t
	{
	public:
		int   iID = 0;
		float flWear = 0.f;
		float flScale = 1.f;
		int   iRotation = 0;
	};

	class Csettings
	{
	public:

		bool Save(std::string file_name);
		bool Load(std::string file_name);
		void CreateConfig(std::string name); 
		std::vector<std::string> GetConfigs();
		int config_sel;





		int tabsfonts;
		float tabssize = 20.0f;
		int textfonts;
		float textsize = 16.0f;
		bool owerwatchisenabled;
		float namecheatcol[4] = { 1.0f,0.0f,0.0f,1.0f };
		float playername[4] = { 1.0f,0.0f,1.0f,1.0f };



		char customname;
		bool grenadeprediction;
		float grenadepredline_col[4];
		bool esprank;
		bool aimbot_enabled;
		int aimbot_mode;
		int selection_mode;
		bool autofire;
		int autofirehotkey;
		int autofire_mode;
		int revolver_mode;
		int autofire_bone;
		float aimbotfov;
		bool autowall;
		float autowalldamage;
		bool preferbody;
		float bodyaimonxp;
		bool bodyaimifdead;
		bool bodyaimiawp;
		int bodyaim_mode;
		bool bodyaimiscout;
		int bodyaim_mode1;
		bool multipoint;
		float pointscale;
		bool hitchance;
		float hitchancescale;
		bool autostop;
		bool autoscope;
		bool norecoil;
		bool friendlyfire;
		int autostop_mode;
		bool accuracystanding;
		bool multiHitboxes[9] =
		{
			true, true, true, true, true,
			true, true, true, true
		};
		bool antiaimcorrection;
		int antiaimcorrectiontype;
		bool antiuntrusted;
		bool overrideenable;
		int overridehotkey;
		bool shitdogresolver;
		bool disabieInterpolation;
		bool backtrack;
		bool legitbacktrack;
		float backtrackticks;
		bool linearextrapolations;
		bool resolverdebug;
		bool lbyindicator;
		int antiaimmode;

		int standaapitch;
		int standaayaw;
		float yawoffset;
		int fakestandaayaw;
		float fakeyawoffset;
		bool standaacleanup;

		int moveaapitch;
		int moveaayaw;
		float moveyawoffset;
		int movefakeaayaw;
		float movefakeyawoffset;
		bool moveaacleanup;

		int edge_mode;
		int edgeaapitch;
		int edgeaayaw;
		float edgeyawoffset;
		int edgefakeyaw;
		float edgefakeyawoffset;

		float speedseed;
		float jitterrange;
		bool dormantcheck;
		int autodirection_mode;
		bool lbybreaker;
		float lbydelta;




		int pistolhotkey;
		int smghotkey;
		int riflehotkey;
		int shotgunhotkey;
		int sniperhotkey;


		int legitweapon;

		bool pistolenabled;
		int pistolmode;
		bool pistolautofire;
		int pistolautofire_mode;
		int pistolbone;
		float pistolfov;
		int reactiontime;
		float pistolsmooth;
		float pistolrecoil;
		bool autopistol;
		

		int backflip;


		bool smgenabled;
		int smgmode;
		bool smgautofire;
		int smgautofire_mode;
		int smgbone;
		int smgreactiontime;
		float smgfov;
		float smgsmooth;
		float smgrecoil;

		
		bool rifleenabled;
		int riflemode;
		bool rifleautofire;
		int rifleautofire_mode;
		int riflebone;
		int riflereactiontime;
		float riflefov;
		float riflesmooth;
		float riflerecoil;



		
		bool shotgunenabled;
		int shotgunmode;
		bool shotgunautofire;
		int shotgunautofire_mode;
		int shotgunbone;
		int shotgunreactiontime;
		float shotgunfov;
		float shotgunsmooth;
		float shotgunrecoil;

		
		bool sniperenabled;
		int snipermode;
		bool sniperautofire;
		int sniperautofire_mode;
		int sniperbone;
		int sniperreactiontime;
		float sniperfov;
		float snipersmooth;
		float sniperrecoil;






		bool legitantiaim;
		int legitchoicelist;










		int esptype;
		bool espteam;
		bool espbox;
		float espboxcolor[4] = { 1.0f,0.0f,0.0f,1.0f };
		int espboxtype;
		int espmode;
		bool espoutline;
		bool espboxfill;
		float espboxfillopacity;
		bool espglow;
		float espglowcolor[4] = { 1.0f,0.0f,0.0f,1.0f };
		bool esphealth;
		int esphealthtype;
		bool esphealthtext;
		bool esparmor;
		float esparmorcolor[4] = { 0.0f,0.0f,1.0f,1.0f };
		bool espsceleton;
		float espsceletoncolor[4] = { 0.0f,1.0f,0.0f,1.0f };
		bool esphitbones;
		float esphitbonescolor[4] = { 0.0f,1.0f,0.0f,1.0f };


		bool espsnaplines;
		float espsnaplinescolor[4] = { 0.0f,1.0f,0.0f,1.0f };

		int chamstype;
		bool chamsenemy;
		float chamsenemycolor[4] = { 1.0f,0.0f,0.0f,1.0f };
		bool chamsenemyinvisible;
		float chamsenemyinvisiblecolor[4] = { 1.0f,1.0f,0.0f,1.0f };

		bool chamsteam;
		float chamsteamcolor[4] = { 0.0f,1.0f,0.0f,1.0f };
		bool chamsteaminvisible;
		float chamsteaminvisiblecolor[4] = { 0.0f,1.0f,1.0f,1.0f };
		bool legitbacktrackchams;
		float legitbacktrackchamscolor[4] = { 1.0f,1.0f,1.0f,1.0f };
		bool espname;
		bool espweapon;
		bool espammo;
		bool espscoped;
		bool esphitmarker;
		bool espdamageindicator;
		bool espreticle;
		bool espdamagereticle;
		bool espfakechams;
		float espfakechamscolor[4] = { 1.0f,1.0f,1.0f,1.0f };
		int fakechamstype;
		bool spreadcrosshair;
		int spreadcrosshairtype;
		bool antiaimlines;
		float fovchanger;
		bool noflash;
		int buttontype;
		bool nosmoke;
		int nosmoketype;
		bool novisrecoil;
		bool legitresolver;
		float bruteAfterX;
		bool noscope;
		bool forcethirdperson;
		int thirdpersonhotkey;
		int thirdpersonangle;
		bool nightmode;
		bool disablepostprocess;
		bool watermark;

		bool legitticks;
		float legittickscolor[4] = { 1.0f,1.0f,1.0f,1.0f };
		bool enginepred;


		bool bunnyhop;
		bool autostrafe;
		int autostrafetype;
		bool fakewalk;
		float fakewalkvalue;

		bool fakewalk1;
		int slowmokey;
		int fakewalkhotkey;
		bool moonwalk;

		

		bool eventlogs;
		int eventlogsfonts;



		bool fakeping;
		float fakepingvalue;

		bool fakelag;
		int fakelagtype;
		int fakelagtypelag;
		bool disableflags;

		bool manualangeflip;
		bool manualangefliparrow;
		int leftflip;
		int rightflip;




		int playerforcepitch;
		int playerforceyaw;









		bool fovchangerenabled;


		
		bool blockbot;
		int airstackkey;
		bool directionarrow;
		float directionarrowcolor[4] = { 1.0f,0.0f,0.0f,1.0f };
		bool hitangle;
		bool bullettracer;
		bool removeparticles;
		bool autobuy;
		int autobuytype;
		bool trashtalk;
		bool autoaccept;
		bool crasher;
		int fakelagpack;

		bool playerlist;

		Weapon_tTT weapons[520];
		int hitmarker_val;

		struct wpnz
		{
			bool  EnabledStickers;
			bool  EnabledSkins;
			int   ChangerSkin;
			char  ChangerName[32] = "";
			int   ChangerStatTrak;
			int   ChangerSeed;
			float ChangerWear;
			Sticker_t Stickers[STICKERS_COUNT];
		}Weapons[519];

		bool skinenabled;
		int Knife;

		bool PING;
		bool FPS;
		int gloves;
		int skingloves;
		float glovewear;
		bool glovesenabled;
		bool rankchanger;
		int KnifeSkin;
		int rank_id;
		int wins;
		int level;
		int friendly;
		int teaching;
		int leader;
		int AK47Skin;
		int GalilSkin;
		int M4A1SSkin;
		int M4A4Skin;
		int AUGSkin;
		int FAMASSkin;
		int AWPSkin;
		int SSG08Skin;
		int SCAR20Skin;
		int P90Skin;
		int Mp7Skin;
		int NovaSkin;
		int UMP45Skin;
		int GlockSkin;
		int SawedSkin;
		int USPSkin;
		int MagSkin;
		int XmSkin;
		int DeagleSkin;
		int DualSkin;
		int FiveSkin;
		int RevolverSkin;
		int Mac10Skin;
		int tec9Skin;
		int Cz75Skin;
		int NegevSkin;
		int M249Skin;
		int Mp9Skin;
		int P2000Skin;
		int BizonSkin;
		int Sg553Skin;
		int P250Skin;
		int G3sg1Skin;
	};
	
	extern Csettings settingsxd;
}





namespace Font
{
	extern void InitFonts();
	extern unsigned int in_game_logging_font;
}
