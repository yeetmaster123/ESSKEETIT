#include "Hooks.h"
#include "Utilities.h"



#define CREATE_EVENT_LISTENER(class_name)\
class class_name : public IGameEventListener2\
{\
public:\
	~class_name() { m_pGameEventManager->RemoveListener(this); }\
\
	virtual void FireGameEvent(IGameEvent* game_event);\
};\

namespace CGameEvents //fuck namespaces, fuck ur style bolbi
{
	void InitializeEventListeners();
	CREATE_EVENT_LISTENER(ItemPurchaseListener);
	CREATE_EVENT_LISTENER(PlayerHurtListener);
	CREATE_EVENT_LISTENER(RoundStartListener);
	CREATE_EVENT_LISTENER(BulletImpactListener);
};



namespace FEATURES
{
	namespace MISC
	{
		class ColorLine
		{
		public:
			ColorLine() {}

			ColorLine(std::string text, Color color = Color(255,255,255,255))
			{
				texts.push_back(text);
				colors.push_back(color);
			}

			void PushBack(std::string text, Color color = Color(255, 255, 255, 255))
			{
				texts.push_back(text);
				colors.push_back(color);
			}

			void PushFront(std::string text, Color color = Color(255, 255, 255, 255))
			{
				texts.insert(texts.begin(), text);
				colors.insert(colors.begin(), color);
			}

			void ChangeAlpha(int alpha)
			{
				for (auto& color : colors)
					color[3] = alpha;
			}

			void Draw(int x, int y, unsigned int font);

			std::string Text()
			{
				std::string text;
				for (const auto string : texts)
					text += string;

				return text;
			}

		private:
			std::vector<std::string> texts;
			std::vector<Color> colors;
		};

		class InGameLogger
		{
		public:
			struct Log
			{
				Log()
				{
					time = UTILS::GetCurtime();
				}

				Log(std::string text, Color color = Color(255, 255, 255, 255))
				{
					color_line = ColorLine(text, color);
					time = UTILS::GetCurtime();
				}

				ColorLine color_line;
				float time;
			};

		public:
			void Do();

			void AddLog(Log log)
			{
				log_queue.insert(log_queue.begin(), log);
			}

		private:
			std::vector<Log> log_queue;

		private:
			const float text_time = 5.f;
			const float text_fade_in_time = 0.3f;
			const float text_fade_out_time = 0.2f;

			const int max_lines_at_once = 10;

			const int ideal_height = 20;
			const int ideal_width = 20;

			const int slide_in_distance = 20;
			const int slide_out_distance = 20;
			const float slide_out_speed = 0.2f;
		};

		extern InGameLogger in_game_logger;
	}
}


namespace FEATURES
{
	namespace MISC
	{
		void InitializeEventListeners();
		void RemoveEventListeners();
		void Do();
	}
}
