#include "NewSkinchanger.h"
#include "Interfaces.h"
#include "CommonIncludes.h"
#include "Interfaces.h"
#include "Offsets.h"
#include "SDK.h"
#include <unordered_map>
#include "shonaxsettings.h"
#include "sticker.h"
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
std::unordered_map<char*, char*> killIcons = {};
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
HANDLE worldmodel_handle;
CBaseAttributableItem* worldmodel;
#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))
#include "Game.h"
void newskinchanger()
{
	
	auto m_local = m_pEntityList->GetClientEntity(m_pEngine->GetLocalPlayer());
	auto weapons = m_local->m_hMyWeapons();
	for (size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		auto pEntity = m_pEntityList->GetClientEntityFromHandle(weapons[i]);
		if (pEntity)
		{
			auto pWeapon = (CBaseAttributableItem*)pEntity;
			//auto wep = (CBaseCombatWeapon*)pEntity;
			if (Shonax::settingsxd.skinenabled)
			{
				int Model = Shonax::settingsxd.Knife;
				int weapon = *pWeapon->fixskins();
				//ApplyStickerHooks(wep);
				switch (weapon)
				{
				case 7: // AK47 
				{
					switch (Shonax::settingsxd.AK47Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 180;//fire serpent
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 524;//Fuel Injector
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 639;//Bloodsport
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 302;//vulcan
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 44;//case hardened
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 456;//Hydroponic
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 474;//Aquamarine Revenge
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 490;//Frontside Misty
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 506;//Point Disarray
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 600;//Neon Revolution
						break;
					case 11:
						*pWeapon->GetFallbackPaintKit() = 14;//red laminate
						break;
					case 12:
						*pWeapon->GetFallbackPaintKit() = 282;//redline
						break;
					case 13:
						*pWeapon->GetFallbackPaintKit() = 316;//jaguar
						break;
					case 14:
						*pWeapon->GetFallbackPaintKit() = 340;//jetset
						break;
					case 15:
						*pWeapon->GetFallbackPaintKit() = 380;//wasteland rebel
						break;
					case 16:
						*pWeapon->GetFallbackPaintKit() = 675;//The Empress
						break;
					case 17:
						*pWeapon->GetFallbackPaintKit() = 422;//Elite Build
						break;
					default:
						break;
					}
				}
				break;
				case 16: // M4A4
				{
					switch (Shonax::settingsxd.M4A4Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 255;//Asiimov
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 309;//Howl
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 400;//Dragon King
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 449;//Poseidon
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 471;//Daybreak
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 512;//Royal Paladin
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 533;//BattleStar
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 588;//Desolate Space
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 632;//Buzz Kill
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 155;//Bullet Rain
						break;
					case 11:
						*pWeapon->GetFallbackPaintKit() = 664;//Hell Fire
						break;
					case 12:
						*pWeapon->GetFallbackPaintKit() = 480;//Evil Daimyo
						break;
					case 13:
						*pWeapon->GetFallbackPaintKit() = 384;//Griffin
						break;
					case 14:
						*pWeapon->GetFallbackPaintKit() = 187;//Zirka
						break;
					case 15:
						*pWeapon->GetFallbackPaintKit() = 167;//Radiation Harzard
						break;
					default:
						break;
					}
				}
				break;
				case 2: // dual
				{
					switch (Shonax::settingsxd.DualSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 276;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 491;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 658;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 625;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 447;
						break;
					default:
						break;
					}
				}
				break;
				case 60: // M4A1
				{
					switch (Shonax::settingsxd.M4A1SSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 644;//Decimator
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 326;//Knight
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 548;//Chantico's Fire
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 497;//Golden Coi
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 430;//Hyper Beast
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 321;//Master Piece
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 445;//Hot Rod
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 587;//Mecha Industries
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 360;//Cyrex
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 440;//Icarus Fell
						break;
					case 11:
						*pWeapon->GetFallbackPaintKit() = 631;//Flashback
						break;
					case 12:
						*pWeapon->GetFallbackPaintKit() = 681;//Flashback
						break;
					case 13:
						*pWeapon->GetFallbackPaintKit() = 430;//Hyper Beast
						break;
					case 14:
						*pWeapon->GetFallbackPaintKit() = 301;//Atomic Alloy
						break;
					case 15:
						*pWeapon->GetFallbackPaintKit() = 257;//Guardian
						break;
					case 16:
						*pWeapon->GetFallbackPaintKit() = 663;//Briefing
						break;
					default:
						break;
					}
				}
				break;
				case 9: // AWP
				{
					switch (Shonax::settingsxd.AWPSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 279;//asiimov
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 344;//dlore
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 640;//Fever Dream
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 446;//medusa
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 475;//hyperbeast
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 174;//boom
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 51;//lightning strike
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 84;//pink ddpat
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 181;//corticera
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 259;//redline
						break;
					case 11:
						*pWeapon->GetFallbackPaintKit() = 395;//manowar
						break;
					case 12:
						*pWeapon->GetFallbackPaintKit() = 212;//graphite
						break;
					case 13:
						*pWeapon->GetFallbackPaintKit() = 227;//electric hive
						break;
					case 14:
						*pWeapon->GetFallbackPaintKit() = 451;//Sun in Leo
						break;
					case 15:
						*pWeapon->GetFallbackPaintKit() = 475;//Hyper Beast
						break;
					case 16:
						*pWeapon->GetFallbackPaintKit() = 251;//Pit viper
						break;
					case 17:
						*pWeapon->GetFallbackPaintKit() = 584;//Phobos
						break;
					case 18:
						*pWeapon->GetFallbackPaintKit() = 525;//Elite Build
						break;
					case 19:
						*pWeapon->GetFallbackPaintKit() = 424;//Worm God
						break;
					case 20:
						*pWeapon->GetFallbackPaintKit() = 662;//Oni Taiji
						break;
					case 21:
						*pWeapon->GetFallbackPaintKit() = 640;//Fever Dream
						break;
					default:
						break;
					}
				}
				break;
				case 61: // USP
				{
					switch (Shonax::settingsxd.USPSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 653;//Neo-Noir
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 637;//Cyrex
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 313;//Orion
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 504;//Kill Confirmed
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 183;//Overgrowth
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 339;//Caiman
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 221;//Serum
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 290;//Guardian
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 318;//Road Rash
						break;
					default:
						break;
					}
				}
				break;
				case 4: // Glock
				{
					switch (Shonax::settingsxd.GlockSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 38;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 48;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 437;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 586;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 353;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 680;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 607;
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 532;
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 381;
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 230;
						break;
					case 11:
						*pWeapon->GetFallbackPaintKit() = 159;
						break;
					case 12:
						*pWeapon->GetFallbackPaintKit() = 623;
						break;
					case 13:
						*pWeapon->GetFallbackPaintKit() = 479;
						break;
					case 14:
						*pWeapon->GetFallbackPaintKit() = 680;
						break;
					case 15:
						*pWeapon->GetFallbackPaintKit() = 367;
						break;
					default:
						break;
					}
				}
				break;
				case 1: // Deagle
				{
					switch (Shonax::settingsxd.DeagleSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 37;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 527;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 645;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 185;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 37;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 645;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 231;
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 603;
						break;
					default:
						break;
					}
				}
				break;
				case 3: // Five Seven
				{
					switch (Shonax::settingsxd.FiveSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 427;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 660;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 352;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 530;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 510;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 646;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 585;
						break;
					default:
						break;
					}
				}
				break;
				case 8: // AUG
				{
					switch (Shonax::settingsxd.AUGSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 9;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 33;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 280;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 455;
						break;
					default:
						break;
					}
				}
				break;
				case 10: // Famas
				{
					switch (Shonax::settingsxd.FAMASSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 429;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 371;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 477;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 492;
						break;
					default:
						break;
					}
				}
				break;
				case 11: // G3SG1
				{
					switch (Shonax::settingsxd.G3sg1Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 677;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 511;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 463;
						break;
					default:
						break;
					}
				}
				break;
				case 13: // Galil
				{
					switch (Shonax::settingsxd.GalilSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 398;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 647;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 661;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 428;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 379;
						break;
					default:
						break;
					}
				}
				break;
				case 14: // M249
				{
					switch (Shonax::settingsxd.M249Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 496;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 401;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 266;
						break;
					default:
						break;
					}
				}
				break;
				case 17: // Mac 10
				{
					switch (Shonax::settingsxd.Mac10Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 433;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 651;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 310;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 498;
						break;
					default:
						break;
					}
				}
				break;
				case 19: // P90
				{
					*pWeapon->GetFallbackPaintKit() = 156;
				}
				break;
				case 24: // UMP-45
				{
					switch (Shonax::settingsxd.UMP45Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 37;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 441;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 448;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 556;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 688;
						break;
					default:
						break;
					}
				}
				break;
				case 25: // XM1014
				{
					switch (Shonax::settingsxd.XmSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 654;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 363;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 689;
						break;
					default:
						break;
					}
				}
				break;
				case 63: // CZ75-Auto
				{
					switch (Shonax::settingsxd.Cz75Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 543;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 435;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 270;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 643;
						break;
					default:
						break;
					}
				}
				break;
				case 26: // Bizon
				{
					switch (Shonax::settingsxd.BizonSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 676;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 542;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 508;
						break;
					default:
						break;
					}
				}
				break;
				case 27: // Mag 7
				{
					switch (Shonax::settingsxd.MagSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 39;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 431;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 608;
						break;
					default:
						break;
					}
				}
				break;
				case 28: // Negev
				{
					switch (Shonax::settingsxd.NegevSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 514;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 483;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 432;
						break;
					default:
						break;
					}
				}
				break;
				case 29: // Sawed Off
				{
					switch (Shonax::settingsxd.SawedSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 638;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 256;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 517;
						break;
					default:
						break;
					}
				}
				break;
				case 30: // Tec 9
				{
					switch (Shonax::settingsxd.tec9Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 179;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 248;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 216;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 272;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 289;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 303;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 374;
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 555;
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 614;
						break;
					default:
						break;
					}
				}
				break;
				case 32: // P2000
				{
					switch (Shonax::settingsxd.P2000Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 485;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 38;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 184;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 211;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 389;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 442;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 443;
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 515;
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 550;
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 591;
						break;
					default:
						break;
					}
				}
				break;
				case 33: // MP7
				{
					switch (Shonax::settingsxd.Mp9Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 481;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 536;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 500;
						break;
					default:
						break;
					}
				}
				break;
				case 34: // MP9
				{
					switch (Shonax::settingsxd.Mp9Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 262;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 482;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 609;
						break;
					default:
						break;
					}
				}
				break;
				case 35: // Nova
				{
					switch (Shonax::settingsxd.NovaSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 537;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 356;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 286;
						break;
					default:
						break;
					}
				}
				break;
				case 36: // P250
				{
					switch (Shonax::settingsxd.P250Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 102;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 466;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 467;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 501;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 551;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 678;
						break;
					default:
						break;
					}
				}
				break;
				case 38: // Scar 20
				{
					switch (Shonax::settingsxd.SCAR20Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 642;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 298;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 406;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 453;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 502;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 518;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 597;
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 685;
						break;
					default:
						break;
					}
				}
				break;
				case 39: // SG553
				{
					switch (Shonax::settingsxd.Sg553Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 519;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 487;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 287;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 586;
						break;
					default:
						break;
					}
				}
				break;
				case 40: // SSG08
				{
					switch (Shonax::settingsxd.SSG08Skin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 26;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 60;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 96;
						break;
					case 4:
						*pWeapon->GetFallbackPaintKit() = 99;
						break;
					case 5:
						*pWeapon->GetFallbackPaintKit() = 157;
						break;
					case 6:
						*pWeapon->GetFallbackPaintKit() = 200;
						break;
					case 7:
						*pWeapon->GetFallbackPaintKit() = 222;
						break;
					case 8:
						*pWeapon->GetFallbackPaintKit() = 233;
						break;
					case 9:
						*pWeapon->GetFallbackPaintKit() = 253;
						break;
					case 10:
						*pWeapon->GetFallbackPaintKit() = 304;
						break;
					case 11:
						*pWeapon->GetFallbackPaintKit() = 319;
						break;
					case 12:
						*pWeapon->GetFallbackPaintKit() = 361;
						break;
					case 13:
						*pWeapon->GetFallbackPaintKit() = 503;
						break;
					case 14:
						*pWeapon->GetFallbackPaintKit() = 538;
						break;
					case 15:
						*pWeapon->GetFallbackPaintKit() = 554;
						break;
					case 16:
						*pWeapon->GetFallbackPaintKit() = 624;
						break;
					default:
						break;
					}
				}
				break;
				case 64: // Revolver
				{
					switch (Shonax::settingsxd.RevolverSkin)
					{
					case 0:
						*pWeapon->GetFallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->GetFallbackPaintKit() = 683;
						break;
					case 2:
						*pWeapon->GetFallbackPaintKit() = 522;
						break;
					case 3:
						*pWeapon->GetFallbackPaintKit() = 12;
						break;
					default:
						break;
					}
				}
				break;
				default:
					break;
				}
				if (pEntity->GetClientClass()->m_ClassID == (int)105)
				{
					auto pCustomName1 = MakePtr(char*, pWeapon, 0x301C);
					worldmodel_handle = pWeapon->m_hWeaponWorldModel();
					if (worldmodel_handle) worldmodel = (CBaseAttributableItem*)m_pEntityList->GetClientEntityFromHandleknife(worldmodel_handle);
					if (Model == 0) // Bayonet
					{
						int iBayonet = m_pModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
						*pWeapon->ModelIndex() = iBayonet; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iBayonet;
						if (worldmodel) *pWeapon->ModelIndex() = iBayonet + 1;
						*pWeapon->fixskins() = 500;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "bayonet";
						killIcons["knife_t"] = "bayonet";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 563; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 573; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 9) // Bowie Knife
					{
						int iBowie = m_pModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
						*pWeapon->ModelIndex() = iBowie; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iBowie;
						if (worldmodel) *worldmodel->ModelIndex() = iBowie + 1;
						*pWeapon->fixskins() = 514;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_survival_bowie";
						killIcons["knife_t"] = "knife_survival_bowie";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}

					}
					else if (Model == 6) // Butterfly Knife
					{
						int iButterfly = m_pModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
						*pWeapon->ModelIndex() = iButterfly; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iButterfly;
						if (worldmodel) *worldmodel->ModelIndex() = iButterfly + 1;
						*pWeapon->fixskins() = 515;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_butterfly";
						killIcons["knife_t"] = "knife_butterfly";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 7) // Falchion Knife
					{
						int iFalchion = m_pModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
						*pWeapon->ModelIndex() = iFalchion; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iFalchion;
						if (worldmodel) *worldmodel->ModelIndex() = iFalchion + 1;
						*pWeapon->fixskins() = 512;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_falchion";
						killIcons["knife_t"] = "knife_falchion";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 1) // Flip Knife
					{
						int iFlip = m_pModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
						*pWeapon->ModelIndex() = iFlip; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iFlip;
						if (worldmodel) *worldmodel->ModelIndex() = iFlip + 1;
						*pWeapon->fixskins() = 505;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_flip";
						killIcons["knife_t"] = "knife_flip";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 559; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 564; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}

					}
					else if (Model == 2) // Gut Knife
					{
						int iGut = m_pModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
						*pWeapon->ModelIndex() = iGut; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iGut;
						if (worldmodel) *worldmodel->ModelIndex() = iGut + 1;
						*pWeapon->fixskins() = 506;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_gut";
						killIcons["knife_t"] = "knife_gut";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 560; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 565; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 575; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 5) // Huntsman Knife
					{
						int iHuntsman = m_pModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
						*pWeapon->ModelIndex() = iHuntsman; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iHuntsman;
						if (worldmodel) *worldmodel->ModelIndex() = iHuntsman + 1;
						*pWeapon->fixskins() = 509;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_tactical";
						killIcons["knife_t"] = "knife_tactical";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 559; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 3) // Karambit
					{
						int iKarambit = m_pModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
						*pWeapon->ModelIndex() = iKarambit; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iKarambit;
						if (worldmodel) *worldmodel->ModelIndex() = iKarambit + 1;
						*pWeapon->fixskins() = 507;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_karambit";
						killIcons["knife_t"] = "knife_karambit";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Doppler Phase 4
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 566; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 576; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 582; // Freehand
						}

					}
					else if (Model == 4) // M9 Bayonet
					{
						int iM9Bayonet = m_pModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
						*pWeapon->ModelIndex() = iM9Bayonet; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iM9Bayonet;
						if (worldmodel) *worldmodel->ModelIndex() = iM9Bayonet + 1;
						*pWeapon->fixskins() = 508;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_m9_bayonet";
						killIcons["knife_t"] = "knife_m9_bayonet";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Doppler Phase 4
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 562; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 577; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 581; // Freehand
						}

					}
					else if (Model == 8)
					{
						int iDagger = m_pModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
						*pWeapon->ModelIndex() = iDagger; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iDagger;
						if (worldmodel) *worldmodel->ModelIndex() = iDagger + 1;
						*pWeapon->fixskins() = 516;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 10)
					{
						int Navaja = m_pModelInfo->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
						*pWeapon->ModelIndex() = Navaja; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Navaja;
						if (worldmodel) *worldmodel->ModelIndex() = Navaja + 1;
						*pWeapon->fixskins() = 520;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 11) // Shadow Daggers
					{
						int Stiletto = m_pModelInfo->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
						*pWeapon->ModelIndex() = Stiletto; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Stiletto;
						if (worldmodel) *worldmodel->ModelIndex() = Stiletto + 1;
						*pWeapon->fixskins() = 522;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 12)
					{
						int Ursus = m_pModelInfo->GetModelIndex("models/weapons/v_knife_ursus.mdl");
						*pWeapon->ModelIndex() = Ursus; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Ursus;
						if (worldmodel) *worldmodel->ModelIndex() = Ursus + 1;
						*pWeapon->fixskins() = 519;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 13)
					{
						int Talon = m_pModelInfo->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
						*pWeapon->ModelIndex() = Talon; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Talon;
						if (worldmodel) *worldmodel->ModelIndex() = Talon + 1;
						*pWeapon->fixskins() = 523;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = Shonax::settingsxd.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->GetFallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->GetFallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->GetFallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->GetFallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->GetFallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->GetFallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->GetFallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->GetFallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->GetFallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->GetFallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->GetFallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->GetFallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->GetFallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->GetFallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->GetFallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->GetFallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->GetFallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->GetFallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->GetFallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->GetFallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->GetFallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->GetFallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->GetFallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->GetFallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->GetFallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->GetFallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->GetFallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->GetFallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->GetFallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->GetFallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->GetFallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->GetFallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->GetFallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->GetFallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->GetFallbackPaintKit() = 580; // Freehand
						}
					}
				}
				*pWeapon->GetOriginalOwnerXuidLow() = 0;
				*pWeapon->GetOriginalOwnerXuidHigh() = 0;
				*pWeapon->GetFallbackWear() = 0.001f;
				*pWeapon->GetItemIDHigh() = 1;
				*pWeapon->GetCustomName() = Shonax::settingsxd.customname;
			}
		}
	}
}
// Junk Code By Troll Face & Thaisen's Gen
void SvdosTHhSUHIwGqAHDXdHWMEwaZdQZP15973136() {     double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau78045280 = -477768925;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau78612115 = -394355111;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau21077340 = 48021992;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau24983003 = -805984149;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau16238789 = -926265805;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau91531889 = -134779128;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90701423 = -440837451;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau88127299 = -353137314;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau66339694 = -300701636;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau75650242 = -290019407;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau81733256 = -246992316;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau40499090 = -341294318;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau21501361 = -309092449;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau15704517 = -936770852;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau11767247 = -795284789;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau76176958 = -687422554;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau15957633 = -538868732;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau25790171 = -675172388;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51701134 = -773896412;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64324845 = -328044696;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau53347346 = 54676411;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64231711 = -227319016;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau28536286 = -251293130;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau60646718 = -63456267;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4728271 = -445454995;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau33078429 = -423933664;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau953065 = -853797566;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau86235251 = -977669083;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau49679313 = -742206207;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau14469141 = -301528158;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68863151 = -106086781;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68469167 = -138089573;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau97752999 = -354984236;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau8031445 = -473550365;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72188873 = -262658168;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau5714948 = -895596141;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau42118956 = -264232357;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau11460618 = -506019230;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau53460224 = -975633965;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau59422080 = -420748368;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau2646789 = 43540148;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau55265247 = -355634124;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau18873386 = -901772357;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38342790 = -110518388;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau63025874 = -10879342;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90221642 = 53324606;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68528542 = -792335868;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38790370 = -59964414;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau23690105 = -431416709;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau40761111 = -555301646;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau83153458 = -381144488;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau23162907 = -39837740;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51084753 = -489072439;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau25250636 = -224842619;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62625789 = -407296767;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13813570 = -150449910;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau50075830 = -43061981;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau60430621 = -888521741;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau20254733 = -260529155;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau83160360 = -402332141;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90578825 = -280981563;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4466172 = -463168369;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38447986 = -610931107;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51870554 = -999173479;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau6787092 = -83932627;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13264089 = -8902743;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau42746090 = -986310082;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13469916 = -835542085;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau43515643 = -574112685;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau6052300 = -899688649;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau34058002 = -323190197;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4497016 = 67150497;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72329947 = -699538424;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau92279054 = -253148044;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau61678057 = -271584845;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau98082099 = -589689466;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45358325 = -325546659;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90193495 = -40774743;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau97620843 = 47423075;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau14506628 = -398779601;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64549886 = -631597797;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62162694 = -693833152;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62545147 = -446252374;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau8918203 = -86904562;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau31315682 = -920383670;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45700245 = 33750959;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau17384415 = -649017135;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72502364 = -30141618;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45405655 = 33746402;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau58375304 = -12208259;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau55639117 = -752534161;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau81688335 = -375710617;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau91205885 = -145490076;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau70299864 = -473301824;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68843254 = -39766806;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau98180616 = -493291483;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau16817261 = -744703017;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau67002832 = -902598879;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau31555699 = 73414238;    double gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau49761786 = -477768925;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau78045280 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau78612115;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau78612115 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau21077340;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau21077340 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau24983003;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau24983003 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau16238789;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau16238789 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau91531889;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau91531889 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90701423;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90701423 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau88127299;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau88127299 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau66339694;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau66339694 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau75650242;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau75650242 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau81733256;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau81733256 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau40499090;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau40499090 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau21501361;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau21501361 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau15704517;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau15704517 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau11767247;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau11767247 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau76176958;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau76176958 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau15957633;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau15957633 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau25790171;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau25790171 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51701134;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51701134 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64324845;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64324845 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau53347346;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau53347346 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64231711;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64231711 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau28536286;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau28536286 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau60646718;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau60646718 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4728271;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4728271 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau33078429;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau33078429 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau953065;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau953065 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau86235251;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau86235251 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau49679313;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau49679313 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau14469141;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau14469141 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68863151;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68863151 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68469167;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68469167 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau97752999;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau97752999 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau8031445;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau8031445 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72188873;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72188873 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau5714948;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau5714948 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau42118956;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau42118956 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau11460618;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau11460618 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau53460224;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau53460224 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau59422080;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau59422080 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau2646789;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau2646789 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau55265247;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau55265247 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau18873386;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau18873386 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38342790;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38342790 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau63025874;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau63025874 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90221642;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90221642 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68528542;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68528542 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38790370;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38790370 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau23690105;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau23690105 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau40761111;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau40761111 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau83153458;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau83153458 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau23162907;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau23162907 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51084753;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51084753 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau25250636;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau25250636 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62625789;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62625789 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13813570;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13813570 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau50075830;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau50075830 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau60430621;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau60430621 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau20254733;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau20254733 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau83160360;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau83160360 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90578825;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90578825 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4466172;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4466172 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38447986;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau38447986 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51870554;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau51870554 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau6787092;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau6787092 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13264089;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13264089 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau42746090;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau42746090 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13469916;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau13469916 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau43515643;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau43515643 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau6052300;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau6052300 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau34058002;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau34058002 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4497016;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau4497016 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72329947;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72329947 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau92279054;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau92279054 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau61678057;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau61678057 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau98082099;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau98082099 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45358325;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45358325 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90193495;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau90193495 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau97620843;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau97620843 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau14506628;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau14506628 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64549886;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau64549886 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62162694;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62162694 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62545147;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau62545147 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau8918203;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau8918203 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau31315682;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau31315682 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45700245;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45700245 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau17384415;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau17384415 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72502364;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau72502364 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45405655;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau45405655 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau58375304;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau58375304 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau55639117;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau55639117 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau81688335;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau81688335 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau91205885;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau91205885 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau70299864;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau70299864 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68843254;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau68843254 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau98180616;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau98180616 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau16817261;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau16817261 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau67002832;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau67002832 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau31555699;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau31555699 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau49761786;     gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau49761786 = gNsgQZFRzdbooSyAUWkLxiLowiZrSueRsFau78045280;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oQwbvceZihipkASDbKbZbLAteKNyMNB83376640() {     double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3344874 = -305969783;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX32855569 = -888158355;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX63092117 = -736785694;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX21429092 = -822765776;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX20980883 = -850462722;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX30511482 = -764943928;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX98728358 = -446042301;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX43358834 = -904699969;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82719323 = -154548406;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX77888202 = -506722898;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX22914317 = -866722891;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX63102052 = -578960572;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX96998976 = -43828286;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX46362201 = -695055693;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX69883449 = -814869295;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX73037681 = -220361356;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX65494982 = -741268260;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36680785 = -24974678;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX55057787 = -184878167;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX12819873 = -727801598;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX22326913 = -132552259;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX55176331 = -135040313;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3828323 = -764837538;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX39574732 = -335818512;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX93047964 = -920967000;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX21491034 = -434132818;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX81425355 = 95380667;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82670002 = -999811705;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX34397925 = -962879599;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX24166516 = -826112570;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX61053355 = -790486227;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28696096 = -169705023;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX33806661 = 39592328;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX68335149 = -693882591;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX67075304 = -586782128;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX95862938 = -202584014;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX72556297 = -402905564;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX47148024 = -159324040;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18076759 = -925992607;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX50384666 = -306097016;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6535141 = -540972786;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6120558 = -926347342;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX19123889 = -188161979;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX13077388 = -313137469;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX2982049 = -925302735;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92389047 = -685846816;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX38831249 = -316743673;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3215470 = 1491563;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90132712 = -980674704;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX14075682 = -665354710;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX39715692 = -263487118;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX68963818 = -906269189;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX13336761 = -48839950;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX83143968 = -213530452;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX61049728 = -671045635;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX48168543 = -70929471;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX29027246 = -23320818;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX23517385 = -300967183;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28381127 = -901798777;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX99489848 = -316329904;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX49086126 = -760324595;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16058357 = -446230596;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX8960909 = -941820371;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX58552808 = -328435836;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16834847 = -716236672;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX94218221 = -597017869;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX29295391 = -518552900;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28663827 = -349945695;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX79286896 = -8273565;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX74020510 = -512285282;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX481385 = -817455793;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18346958 = -481944220;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18604027 = -98982071;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX4673122 = -878781151;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6284733 = -86828812;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16206356 = -206204918;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36052442 = -946878334;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90750934 = -351700070;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36592683 = -410515777;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX658918 = -135120184;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82659784 = -17389145;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX78209885 = -906110897;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92537289 = 80862998;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX20322243 = -197524890;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX84450823 = -462625452;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92089537 = -884217038;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX15359336 = -20865074;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX50662692 = -746877221;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX7285422 = 77163044;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18906762 = -415852658;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX66835692 = -79263196;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX49038913 = -1938381;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18766898 = -257525264;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18586910 = -509662704;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX1298540 = -545772421;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90476783 = 5257809;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX97159648 = -984526972;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX60571080 = -859726144;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX96242541 = -596900797;    double WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX8763828 = -305969783;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3344874 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX32855569;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX32855569 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX63092117;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX63092117 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX21429092;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX21429092 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX20980883;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX20980883 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX30511482;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX30511482 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX98728358;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX98728358 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX43358834;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX43358834 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82719323;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82719323 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX77888202;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX77888202 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX22914317;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX22914317 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX63102052;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX63102052 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX96998976;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX96998976 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX46362201;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX46362201 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX69883449;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX69883449 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX73037681;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX73037681 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX65494982;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX65494982 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36680785;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36680785 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX55057787;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX55057787 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX12819873;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX12819873 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX22326913;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX22326913 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX55176331;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX55176331 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3828323;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3828323 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX39574732;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX39574732 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX93047964;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX93047964 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX21491034;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX21491034 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX81425355;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX81425355 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82670002;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82670002 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX34397925;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX34397925 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX24166516;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX24166516 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX61053355;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX61053355 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28696096;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28696096 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX33806661;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX33806661 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX68335149;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX68335149 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX67075304;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX67075304 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX95862938;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX95862938 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX72556297;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX72556297 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX47148024;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX47148024 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18076759;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18076759 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX50384666;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX50384666 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6535141;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6535141 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6120558;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6120558 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX19123889;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX19123889 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX13077388;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX13077388 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX2982049;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX2982049 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92389047;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92389047 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX38831249;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX38831249 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3215470;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3215470 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90132712;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90132712 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX14075682;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX14075682 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX39715692;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX39715692 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX68963818;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX68963818 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX13336761;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX13336761 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX83143968;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX83143968 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX61049728;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX61049728 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX48168543;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX48168543 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX29027246;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX29027246 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX23517385;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX23517385 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28381127;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28381127 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX99489848;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX99489848 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX49086126;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX49086126 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16058357;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16058357 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX8960909;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX8960909 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX58552808;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX58552808 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16834847;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16834847 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX94218221;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX94218221 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX29295391;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX29295391 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28663827;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX28663827 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX79286896;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX79286896 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX74020510;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX74020510 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX481385;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX481385 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18346958;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18346958 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18604027;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18604027 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX4673122;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX4673122 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6284733;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX6284733 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16206356;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX16206356 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36052442;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36052442 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90750934;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90750934 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36592683;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX36592683 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX658918;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX658918 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82659784;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX82659784 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX78209885;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX78209885 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92537289;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92537289 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX20322243;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX20322243 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX84450823;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX84450823 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92089537;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX92089537 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX15359336;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX15359336 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX50662692;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX50662692 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX7285422;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX7285422 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18906762;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18906762 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX66835692;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX66835692 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX49038913;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX49038913 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18766898;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18766898 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18586910;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX18586910 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX1298540;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX1298540 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90476783;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX90476783 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX97159648;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX97159648 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX60571080;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX60571080 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX96242541;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX96242541 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX8763828;     WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX8763828 = WSiBwWJYlWMVHoiJhWlkNbqWdPCwlPfMUfLX3344874;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VskwLmskOHDsazLYjQSvvbWgFmaqFPJ50780146() {     double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28644467 = -134170639;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87099022 = -281961594;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn5106895 = -421593381;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn17875181 = -839547406;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn25722977 = -774659630;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69491074 = -295108727;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn6755295 = -451247150;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn98590368 = -356262609;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn99098952 = -8395175;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn80126162 = -723426390;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn64095378 = -386453464;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn85705013 = -816626816;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn72496592 = -878564123;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn77019885 = -453340532;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn27999651 = -834453781;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69898405 = -853300157;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15032333 = -943667788;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn47571399 = -474776963;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn58414440 = -695859924;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61314900 = -27558500;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn91306478 = -319780924;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn46120950 = -42761609;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79120359 = -178381946;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn18502746 = -608180757;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn81367658 = -296478993;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn9903639 = -444331976;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61897647 = -55441100;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79104752 = 78045678;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn19116537 = -83553007;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn33863890 = -250696983;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn53243560 = -374885673;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn88923024 = -201320476;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69860321 = -665831111;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28638854 = -914214817;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61961735 = -910906092;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86010929 = -609571904;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn2993639 = -541578770;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82835431 = -912628848;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82693292 = -876351262;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41347252 = -191445664;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn10423493 = -25485721;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn56975867 = -397060580;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn19374392 = -574551603;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87811986 = -515756549;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn42938223 = -739726129;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn94556452 = -325018256;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn9133957 = -941151478;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn67640570 = 62947540;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn56575321 = -429932723;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87390254 = -775407769;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn96277925 = -145829749;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn14764730 = -672700642;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75588769 = -708607457;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41037301 = -202218284;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn59473667 = -934794509;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82523516 = 8590970;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn7978663 = -3579648;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86604148 = -813412625;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn36507522 = -443068414;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15819339 = -230327654;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn7593427 = -139667628;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn27650542 = -429292829;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79473831 = -172709603;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn65235062 = -757698193;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn26882603 = -248540717;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75172353 = -85132989;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15844693 = -50795705;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn43857738 = -964349306;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15058150 = -542434441;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41988722 = -124881877;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn66904766 = -211721388;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn32196901 = 68961059;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn64878107 = -598425701;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn17067188 = -404414260;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn50891407 = 97927221;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn34330611 = -922720344;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn26746559 = -468210007;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn91308373 = -662625397;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75564523 = -868454628;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86811206 = -971460737;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn769683 = -503180498;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn94257077 = -18388640;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn22529432 = -492021599;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn31726283 = -308145238;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn37585965 = -4867235;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn38478830 = -702185031;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn13334256 = -492713019;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28823020 = -363612828;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69165187 = -979420309;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79438219 = -819497063;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn78032266 = -505992256;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn16389491 = -728166146;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn46327910 = -369560434;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn66873954 = -546023608;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn33753825 = 48221963;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82772951 = -596192893;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn77502036 = -124350978;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn54139329 = -816853411;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn60929383 = -167215833;    double JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn67765870 = -134170639;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28644467 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87099022;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87099022 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn5106895;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn5106895 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn17875181;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn17875181 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn25722977;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn25722977 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69491074;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69491074 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn6755295;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn6755295 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn98590368;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn98590368 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn99098952;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn99098952 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn80126162;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn80126162 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn64095378;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn64095378 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn85705013;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn85705013 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn72496592;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn72496592 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn77019885;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn77019885 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn27999651;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn27999651 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69898405;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69898405 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15032333;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15032333 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn47571399;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn47571399 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn58414440;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn58414440 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61314900;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61314900 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn91306478;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn91306478 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn46120950;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn46120950 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79120359;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79120359 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn18502746;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn18502746 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn81367658;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn81367658 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn9903639;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn9903639 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61897647;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61897647 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79104752;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79104752 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn19116537;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn19116537 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn33863890;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn33863890 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn53243560;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn53243560 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn88923024;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn88923024 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69860321;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69860321 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28638854;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28638854 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61961735;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn61961735 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86010929;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86010929 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn2993639;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn2993639 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82835431;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82835431 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82693292;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82693292 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41347252;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41347252 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn10423493;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn10423493 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn56975867;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn56975867 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn19374392;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn19374392 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87811986;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87811986 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn42938223;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn42938223 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn94556452;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn94556452 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn9133957;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn9133957 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn67640570;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn67640570 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn56575321;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn56575321 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87390254;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn87390254 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn96277925;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn96277925 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn14764730;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn14764730 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75588769;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75588769 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41037301;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41037301 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn59473667;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn59473667 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82523516;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82523516 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn7978663;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn7978663 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86604148;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86604148 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn36507522;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn36507522 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15819339;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15819339 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn7593427;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn7593427 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn27650542;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn27650542 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79473831;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79473831 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn65235062;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn65235062 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn26882603;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn26882603 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75172353;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75172353 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15844693;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15844693 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn43857738;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn43857738 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15058150;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn15058150 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41988722;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn41988722 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn66904766;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn66904766 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn32196901;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn32196901 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn64878107;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn64878107 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn17067188;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn17067188 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn50891407;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn50891407 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn34330611;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn34330611 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn26746559;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn26746559 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn91308373;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn91308373 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75564523;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn75564523 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86811206;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn86811206 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn769683;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn769683 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn94257077;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn94257077 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn22529432;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn22529432 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn31726283;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn31726283 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn37585965;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn37585965 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn38478830;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn38478830 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn13334256;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn13334256 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28823020;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28823020 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69165187;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn69165187 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79438219;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn79438219 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn78032266;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn78032266 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn16389491;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn16389491 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn46327910;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn46327910 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn66873954;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn66873954 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn33753825;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn33753825 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82772951;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn82772951 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn77502036;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn77502036 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn54139329;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn54139329 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn60929383;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn60929383 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn67765870;     JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn67765870 = JFZKrCVlttLzgxfRfvgBsZgZuWDqZLBfyytn28644467;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HNJtjRlFGDilsTtLLsVmcVfjMtBkzNT42515076() {     int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML64145814 = 24994558;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML12762916 = -430931569;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML66839402 = -693028751;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML18009345 = -841962345;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML42402384 = 20597940;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML49301538 = -144429986;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML59647247 = -92042817;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML45896206 = -460984586;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML79311119 = -388961911;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73225043 = -577988845;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92612887 = -487715553;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73893422 = -789168436;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML37891089 = 32708321;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73522557 = -387200347;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML54483952 = -623020910;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML82670281 = -24693803;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML43752726 = -559468083;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21911040 = -696448301;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML28498330 = -649942556;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14643330 = -177213409;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML99036872 = 95978630;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML9148142 = -248335494;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML60309898 = 75647594;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML76366918 = -8685207;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML93862056 = -752458708;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML52982283 = -701340992;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46433011 = -656586685;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML98116647 = -796773;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML10378268 = -989095529;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25339612 = 3997911;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21315811 = -486210104;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML27645214 = -321563069;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML41650918 = -360104085;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML91791573 = -100413695;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML69200351 = -172174044;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92216607 = -962426412;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML72995973 = -50723645;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML18340311 = -921732981;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML60247242 = -833452326;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML15187771 = -511940871;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92450695 = 40257776;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML93923318 = -820349632;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML721783 = -908035051;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML27023387 = -890564087;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML79777009 = -256054978;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25423828 = -773432066;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML97833008 = -24949939;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML32228683 = -576676403;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML76634404 = -597880540;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML2455940 = -739794521;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML3191508 = -410856;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML44205360 = -661092508;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML62311960 = -835804426;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML77741551 = -996530886;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML43478204 = -833075938;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML54997672 = -726669948;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML52453017 = -406579163;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML90472483 = -584343545;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML24147288 = 10496362;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML89420101 = -278061069;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML2868528 = -487843302;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML61530599 = 8753955;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML35517938 = -471889058;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML53971508 = -292959823;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML51909232 = 8221258;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML64967674 = -66152484;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML32242504 = -329064352;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46099515 = -866877984;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML4322206 = -115026303;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML62267345 = -660594499;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML9674309 = -973970158;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25412415 = -637735103;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML61663798 = -862995976;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML13310559 = -38001686;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML22192634 = -117471185;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML5113554 = -83671739;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML8426360 = -340300443;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML33286511 = -33788320;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML96589909 = -752630229;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML68438228 = -979026643;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML55149274 = -576391054;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14204328 = 20089718;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21482243 = -402916233;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML7922329 = -149301008;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML22148104 = -995591233;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML77110451 = -825117596;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML65333253 = -485758644;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML63909367 = -363573200;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML48313370 = -267337758;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14202679 = -445504096;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML39763591 = -455847249;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML82523489 = -466380101;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML94193022 = -832229343;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML70827141 = -455391258;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML12319244 = 75902431;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML30920097 = -968496180;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML58405381 = -248460574;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46750274 = -515075228;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML75114155 = -798785345;    int VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14809336 = 24994558;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML64145814 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML12762916;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML12762916 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML66839402;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML66839402 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML18009345;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML18009345 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML42402384;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML42402384 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML49301538;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML49301538 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML59647247;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML59647247 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML45896206;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML45896206 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML79311119;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML79311119 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73225043;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73225043 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92612887;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92612887 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73893422;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73893422 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML37891089;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML37891089 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73522557;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML73522557 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML54483952;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML54483952 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML82670281;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML82670281 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML43752726;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML43752726 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21911040;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21911040 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML28498330;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML28498330 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14643330;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14643330 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML99036872;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML99036872 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML9148142;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML9148142 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML60309898;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML60309898 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML76366918;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML76366918 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML93862056;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML93862056 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML52982283;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML52982283 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46433011;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46433011 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML98116647;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML98116647 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML10378268;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML10378268 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25339612;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25339612 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21315811;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21315811 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML27645214;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML27645214 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML41650918;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML41650918 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML91791573;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML91791573 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML69200351;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML69200351 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92216607;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92216607 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML72995973;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML72995973 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML18340311;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML18340311 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML60247242;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML60247242 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML15187771;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML15187771 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92450695;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML92450695 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML93923318;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML93923318 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML721783;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML721783 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML27023387;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML27023387 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML79777009;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML79777009 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25423828;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25423828 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML97833008;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML97833008 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML32228683;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML32228683 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML76634404;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML76634404 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML2455940;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML2455940 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML3191508;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML3191508 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML44205360;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML44205360 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML62311960;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML62311960 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML77741551;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML77741551 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML43478204;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML43478204 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML54997672;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML54997672 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML52453017;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML52453017 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML90472483;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML90472483 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML24147288;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML24147288 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML89420101;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML89420101 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML2868528;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML2868528 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML61530599;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML61530599 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML35517938;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML35517938 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML53971508;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML53971508 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML51909232;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML51909232 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML64967674;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML64967674 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML32242504;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML32242504 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46099515;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46099515 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML4322206;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML4322206 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML62267345;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML62267345 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML9674309;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML9674309 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25412415;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML25412415 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML61663798;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML61663798 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML13310559;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML13310559 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML22192634;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML22192634 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML5113554;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML5113554 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML8426360;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML8426360 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML33286511;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML33286511 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML96589909;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML96589909 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML68438228;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML68438228 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML55149274;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML55149274 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14204328;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14204328 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21482243;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML21482243 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML7922329;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML7922329 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML22148104;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML22148104 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML77110451;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML77110451 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML65333253;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML65333253 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML63909367;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML63909367 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML48313370;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML48313370 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14202679;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14202679 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML39763591;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML39763591 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML82523489;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML82523489 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML94193022;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML94193022 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML70827141;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML70827141 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML12319244;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML12319244 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML30920097;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML30920097 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML58405381;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML58405381 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46750274;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML46750274 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML75114155;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML75114155 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14809336;     VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML14809336 = VaAxWjNKfFiQaxpiNEZrPcKqliahpPAFtjML64145814;}
// Junk Finished
