#pragma once
#pragma once

#include "Interfaces.h"
#include "IGameEvents.h"

#include "Entities.h"
#include "Menu.h"

#include "Hooks.h"
#include "Utilities.h"
#include "RenderManager.h"




class IGameEvent {
public:
	virtual ~IGameEvent() {};
	virtual const char *GetName() const = 0;	// get event name

	virtual bool  IsReliable() const = 0; // if event handled reliable
	virtual bool  IsLocal() const = 0; // if event is never networked
	virtual bool  IsEmpty(const char *keyName = NULL) = 0; // check if data field exists

														   // Data access
	virtual bool  GetBool(const char *keyName = NULL, bool defaultValue = false) = 0;
	virtual int   GetInt(const char *keyName = NULL, int defaultValue = 0) = 0;
	virtual unsigned __int64 GetUint64(const char *keyName = NULL, unsigned __int64 defaultValue = 0) = 0;
	virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f) = 0;
	virtual const char *GetString(const char *keyName = NULL, const char *defaultValue = "") = 0;
	virtual const wchar_t * GetWString(char const *keyName = NULL, const wchar_t *defaultValue = L"") = 0;

	virtual void SetBool(const char *keyName, bool value) = 0;
	virtual void SetInt(const char *keyName, int value) = 0;
	virtual void SetUint64(const char *keyName, unsigned __int64 value) = 0;
	virtual void SetFloat(const char *keyName, float value) = 0;
	virtual void SetString(const char *keyName, const char *value) = 0;
	virtual void SetWString(const char *keyName, const wchar_t *value) = 0;
};
class trace_info
{
public:
	trace_info(Vector starts, Vector positions, float times, int userids)
	{
		this->start = starts;
		this->position = positions;
		this->time = times;
		this->userid = userids;
	}

	Vector position;
	Vector start;
	float time;
	int userid;
};

extern std::vector<trace_info> trace_logs;
#define g_pGameEventManager Interfaces::EventManager

class CBulletListener
{
	class BulletListener : IGameEventListener2
	{
	public:
		void start()
		{
			//if (!g_pGameEventManager->AddListener(this, "bullet_impact", false))
			//{
			//	throw std::exception("Failed to register the event");
			//}
		}
		void stop()
		{
			//g_pGameEventManager->RemoveListener(this);
		}
		void FireGameEvent(IGameEvent* event) override
		{
			CBulletListener::singleton()->OnFireEvent(event);
		}
		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		}
	};
public:
	static CBulletListener* singleton()
	{
		static CBulletListener* instance = new CBulletListener();
		return instance;
	}

	void init()
	{
		_listener.start();
	}
	
	void OnFireEvent(IGameEvent* event)
	{
		if (!strcmp(event->GetName(), "bullet_impact") && true)
		{
			auto index = m_pEntityList->GetClientEntity(m_pEngine->GetPlayerForUserID(event->GetInt("userid")));
			Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
			if (index)
				trace_logs.push_back(trace_info(index->GetEyePosition(), position, m_pGlobals->curtime, event->GetInt("userid")));
		}
	}

	void OnStudioRender()
	{
		if (true) {
			for (unsigned int i = 0; i < trace_logs.size(); i++) {

				auto shooter = m_pEntityList->GetClientEntity(m_pEngine->GetPlayerForUserID(trace_logs[i].userid));

				if (!shooter) return;

				Color color;
				if (shooter->GetTeamNum() == 3)
					color = Color(7,13, 218, 255);
				else
					color = Color(175, 10, 10, 255);

				Hooks::DrawBeamd(trace_logs[i].start, trace_logs[i].position, color);

				trace_logs.erase(trace_logs.begin() + i);
			}
		}
	}

private:
	BulletListener _listener;

	class cbullet_tracer_info
	{
	public:
		cbullet_tracer_info(Vector src, Vector dst, float time, Color color)
		{
			this->src = src;
			this->dst = dst;
			this->time = time;
			this->color = color;
		}

		Vector src, dst;
		float time;
		Color color;
	};

	std::vector<cbullet_tracer_info> logs;
};
