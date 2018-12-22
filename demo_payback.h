#include "MiscDefinitions.h"
#include "ClientRecvProps.h"
#include "offsets.h"
#include "Vector.h"
#include "MiscClasses.h"
#include "Vector2D.h"


class demo_playback_parameter_t {
private:
	std::uint8_t	_pad0[0x10];		// 0x00

public:
	std::uint32_t	m_is_overwatch;		// 0x10

private:
	std::uint8_t	_pad1[0x88];		// 0x14
}; // size = 0x9C

class c_demo_file {
public:
	char* get_header_id(void) {
		return reinterpret_cast< char* >(reinterpret_cast< std::uintptr_t >(this) + 0x108);
	}
};

class i_demo_player {
public:
	c_demo_file * get_demo_file(void) {
		return *reinterpret_cast< c_demo_file** >(reinterpret_cast< std::uintptr_t >(this) + 0x4);
	}

	demo_playback_parameter_t* get_playback_parameter(void) {
		return *reinterpret_cast< demo_playback_parameter_t** >(reinterpret_cast< std::uintptr_t >(this) + 0x5C8);
	}

	bool is_in_overwatch(void) {
		auto playback_parameter = get_playback_parameter();
		return (playback_parameter && playback_parameter->m_is_overwatch);
	}

	void set_overwatch_state(bool state) {
		auto playback_parameter = get_playback_parameter();
		if (playback_parameter) {
			playback_parameter->m_is_overwatch = state;
		}
	}

	bool is_playing_demo(void) {
		using is_playing_demo_t = bool(__thiscall*)(i_demo_player*);
		return call_vfunc< is_playing_demo_t >(this, 4)(this);
	}

	bool is_playing_time_demo(void) {
		using is_player_time_demo_t = bool(__thiscall*)(i_demo_player*);
		return call_vfunc< is_player_time_demo_t >(this, 6)(this);
	}
};
