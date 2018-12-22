#pragma once

#include "Hooks.h"

class Spammers
{
public:
	void run(CUserCmd* m_pcmd);
	void clan_changer();
	void name_spammer();
	void chat_spammer();
};

extern Spammers spammers;