inline void**& GetVTable(void* inst, size_t offset = 0)
{
	return *reinterpret_cast<void***>((size_t)inst + offset);
}
inline const void** GetVTable(const void* inst, size_t offset = 0)
{
	return *reinterpret_cast<const void***>((size_t)inst + offset);
}
template< typename Fn >
inline Fn GetVFunc(const void* inst, size_t index, size_t offset = 0)
{
	return reinterpret_cast<Fn>(GetVTable(inst, offset)[index]);
}

class IBaseClientDLL
{
public:
};

extern IBaseClientDLL* g_pClient;

class Vector
{
public:
	float x, y, z;
};

typedef Vector QAngle;

class CClockDriftMgr
{
public:
	float m_ClockOffsets[16];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

class INetChannel;
class CClientState
{
public:
	void ForceFullUpdate()
	{
		m_nDeltaTick = -1;
	}

	char pad_0000[156];             //0x0000
	uint32_t m_NetChannel;			//0x009C
	uint32_t m_nChallengeNr;        //0x00A0
	char pad_00A4[100];             //0x00A4
	uint32_t m_nSignonState;        //0x0108
	char pad_010C[8];               //0x010C
	float m_flNextCmdTime;          //0x0114
	uint32_t m_nServerCount;        //0x0118
	uint32_t m_nCurrentSequence;    //0x011C
	char pad_0120[8];               //0x0120
	CClockDriftMgr m_ClockDriftMgr; //0x0128
	uint32_t m_nDeltaTick;          //0x0178
	bool m_bPaused;                 //0x017C
	char pad_017D[3];               //0x017D
	uint32_t m_nViewEntity;         //0x0180
	uint32_t m_nPlayerSlot;         //0x0184
	char m_szLevelName[260];        //0x0188
	char m_szLevelNameShort[40];    //0x028C
	char m_szGroupName[40];         //0x02B4
	char pad_02DC[56];              //0x02DC
	uint32_t m_nMaxClients;         //0x0310
	char pad_0314[18940];           //0x0314
	Vector viewangles;              //0x4D10
}; //Size: 0x4D1C

extern CClientState* g_pClientState;

struct SndInfo_t
{
	// Sound Guid
	int			m_nGuid;
	void* m_filenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
	int			m_nSoundSource;
	int			m_nChannel;
	// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
	//  entity upon which to show the lips moving, if the sound has sentence data
	int			m_nSpeakerEntity;
	float		m_flVolume;
	float		m_flLastSpatializedVolume;
	// Radius of this sound effect (spatialization is different within the radius)
	float		m_flRadius;
	int			m_nPitch;
	Vector		*m_pOrigin;
	Vector		*m_pDirection;

	// if true, assume sound source can move and update according to entity
	bool		m_bUpdatePositions;
	// true if playing linked sentence
	bool		m_bIsSentence;
	// if true, bypass all dsp processing for this sound (ie: music)	
	bool		m_bDryMix;
	// true if sound is playing through in-game speaker entity.
	bool		m_bSpeaker;
	// for snd_show, networked sounds get colored differently than local sounds
	bool		m_bFromServer;
};

enum soundlevel_t
{
	SNDLVL_NONE = 0,

	SNDLVL_20dB = 20,			// rustling leaves
	SNDLVL_25dB = 25,			// whispering
	SNDLVL_30dB = 30,			// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,			// refrigerator

	SNDLVL_50dB = 50,	// 3.9	// average home
	SNDLVL_55dB = 55,	// 3.0

	SNDLVL_IDLE = 60,	// 2.0	
	SNDLVL_60dB = 60,	// 2.0	// normal conversation, clothes dryer

	SNDLVL_65dB = 65,	// 1.5	// washing machine, dishwasher
	SNDLVL_STATIC = 66,	// 1.25

	SNDLVL_70dB = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,	// 0.8	// busy traffic

	SNDLVL_80dB = 80,	// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
	SNDLVL_TALKING = 80,	// 0.7
	SNDLVL_85dB = 85,	// 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,	// 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,			// helicopter, power mower
	SNDLVL_110dB = 110,			// snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,			// auto horn, propeller aircraft
	SNDLVL_130dB = 130,			// air raid siren

	SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,	// 0.2

	SNDLVL_150dB = 150,	// 0.2

	SNDLVL_180dB = 180,			// rocket launching

								// NOTE: Valid soundlevel_t values are 0-255.
								//       256-511 are reserved for sounds using goldsrc compatibility attenuation.
};

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter()
	{
	}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};

#define	PITCH_NORM		100			// non-pitch shifted
#define PITCH_LOW		95			// other values are possible - 0-255, where 255 is very high
#define PITCH_HIGH		120

class IEngineSound
{
public:
	// Precache a particular sample
	virtual bool PrecacheSound(const char *pSample, bool bPreload = false, bool bIsUISound = false) = 0;
	virtual bool IsSoundPrecached(const char *pSample) = 0;
	virtual void PrefetchSound(const char *pSample) = 0;

	// Just loads the file header and checks for duration (not hooked up for .mp3's yet)
	// Is accessible to server and client though
	virtual float GetSoundDuration(const char *pSample) = 0;

	// Pitch of 100 is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
	// down to 1 is a lower pitch.   150 to 70 is the realistic range.
	// EmitSound with pitch != 100 should be used sparingly, as it's not quite as
	// fast (the pitchshift mixer is not native coded).

	// NOTE: setting iEntIndex to -1 will cause the sound to be emitted from the local
	// player (client-side only)
	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
		float flVolume, float flAttenuation, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
		const Vector *pOrigin = NULL, const Vector *pDirection = NULL, std::vector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1, int xd = 0) = 0;

	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
		float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
		const Vector *pOrigin = NULL, const Vector *pDirection = NULL, std::vector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1, int xd = 0) = 0;

	virtual void EmitSentenceByIndex(IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex,
		float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
		const Vector *pOrigin = NULL, const Vector *pDirection = NULL, std::vector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;

	virtual void StopSound(int iEntIndex, int iChannel, const char *pSample) = 0;

	// stop all active sounds (client only)
	virtual void StopAllSounds() = 0;

	// Set the room type for a player (client only)
	virtual void SetRoomType(IRecipientFilter& filter, int roomType) = 0;

	// Set the dsp preset for a player (client only)
	virtual void SetPlayerDSP(IRecipientFilter& filter, int dspType, bool fastReset) = 0;

	// emit an "ambient" sound that isn't spatialized
	// only available on the client, assert on server
	virtual void EmitAmbientSound(const char *pSample, float flVolume, int iPitch = PITCH_NORM, int flags = 0, float soundtime = 0.0f) = 0;


	//	virtual EntChannel_t	CreateEntChannel() = 0;

	virtual float GetDistGainFromSoundLevel(soundlevel_t soundlevel, float dist) = 0;

	// Client .dll only functions
	virtual int		GetGuidForLastSoundEmitted() = 0;
	virtual bool	IsSoundStillPlaying(int guid) = 0;
	virtual void	StopSoundByGuid(int guid) = 0;
	// Set's master volume (0.0->1.0)
	virtual void	SetVolumeByGuid(int guid, float fvol) = 0;

	// Retrieves list of all active sounds
	virtual void	GetActiveSounds(std::vector < SndInfo_t >& sndlist) = 0;

	virtual void	PrecacheSentenceGroup(const char *pGroupName) = 0;
	virtual void	NotifyBeginMoviePlayback() = 0;
	virtual void	NotifyEndMoviePlayback() = 0;
};
extern IEngineSound* g_pEngineSound;

class bf_read;
class bf_write;

class INetMessage
{
public:
	virtual	~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel * netchan) = 0; // netchannel this message is from/for
	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message

	virtual bool	Process(void) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(bf_read &buffer) = 0; // returns true if parsing was OK
	virtual	bool	WriteToBuffer(bf_write &buffer) = 0;	// returns true if writing was OK

	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling

	virtual int				GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
	virtual const char		*GetName(void) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel		*GetNetChannel(void) const = 0;
	virtual const char		*ToString(void) const = 0; // returns a human readable string about message content
};

class INetChannel
{
public:
	char pad_0000[20];           //0x0000
	bool m_bProcessingMessages;  //0x0014
	bool m_bShouldDelete;        //0x0015
	char pad_0016[2];            //0x0016
	int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
	int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
	int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
	int32_t m_nChokedPackets;    //0x002C number of choked packets
	bool SendNetMsg(INetMessage* msg, bool bForceReliable, bool bVoice)
	{
		typedef bool(__thiscall* func1)(void*, INetMessage*, bool, bool);
		return GetVFunc<func1>(this, 42)(this, msg, bForceReliable, bVoice);
	}
	char pad_0030[1044];         //0x0030
}; //Size: 0x0444

class CUserCmd
{
public:
	CUserCmd()
	{
		memset(this, 0, sizeof(*this));
	};
	virtual ~CUserCmd() {};

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

		CRC32_Final(&crc);
		return crc;
	}

	int     command_number;     // 0x04 For matching server and client commands for debugging
	int     tick_count;         // 0x08 the tick the client created this command
	QAngle  viewangles;         // 0x0C Player instantaneous view angles.
	Vector  aimdirection;       // 0x18
	float   forwardmove;        // 0x24
	float   sidemove;           // 0x28
	float   upmove;             // 0x2C
	int     buttons;            // 0x30 Attack button states
	char    impulse;            // 0x34
	int     weaponselect;       // 0x38 Current weapon id
	int     weaponsubtype;      // 0x3C
	int     random_seed;        // 0x40 For shared random functions
	short   mousedx;            // 0x44 mouse accum in x from create move
	short   mousedy;            // 0x46 mouse accum in y from create move
	bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
	char    pad_0x4C[0x18];     // 0x4C Current sizeof( usercmd ) =  100  = 0x64
};

class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	CRC32_t  m_crc;
};

class CInput
{
public:
	virtual void  Init_All(void);
	virtual void  Shutdown_All(void);
	virtual int   GetButtonBits(int);
	virtual void  CreateMove(int sequence_number, float input_sample_frametime, bool active);
	virtual void  ExtraMouseSample(float frametime, bool active);
	virtual bool  WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
	virtual void  EncodeUserCmdToBuffer(bf_write& buf, int slot);
	virtual void  DecodeUserCmdFromBuffer(bf_read& buf, int slot);


	inline CUserCmd* GetUserCmd(int sequence_number);
	inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);

	bool                m_fTrackIRAvailable;            //0x04
	bool                m_fMouseInitialized;            //0x05
	bool                m_fMouseActive;                 //0x06
	bool                m_fJoystickAdvancedInit;        //0x07
	char                pad_0x08[0x2C];                 //0x08
	void*               m_pKeys;                        //0x34
	char                pad_0x38[0x6C];                 //0x38
	bool                m_fCameraInterceptingMouse;     //0x9C
	bool                m_fCameraInThirdPerson;         //0x9D
	bool                m_fCameraMovingWithMouse;       //0x9E
	Vector              m_vecCameraOffset;              //0xA0
	bool                m_fCameraDistanceMove;          //0xAC
	int                 m_nCameraOldX;                  //0xB0
	int                 m_nCameraOldY;                  //0xB4
	int                 m_nCameraX;                     //0xB8
	int                 m_nCameraY;                     //0xBC
	bool                m_CameraIsOrthographic;         //0xC0
	QAngle              m_angPreviousViewAngles;        //0xC4
	QAngle              m_angPreviousViewAnglesTilt;    //0xD0
	float               m_flLastForwardMove;            //0xDC
	int                 m_nClearInputState;             //0xE0
	CUserCmd*           m_pCommands;                    //0xEC
	CVerifiedUserCmd*   m_pVerifiedCommands;            //0xF0
};

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
	return &m_pCommands[sequence_number % 150];
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
	return &m_pVerifiedCommands[sequence_number % 150];
}

extern CInput* g_pInput;