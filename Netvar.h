#include <xstring>
#include <vector>



class CRecvProxyData;
struct RecvTable;
struct RecvProp;

namespace UTILS
{
	typedef void(*NetvarHookCallback)(const CRecvProxyData *pData, void *pStruct, void *pOut);

	class CNetvarHookManager
	{
	public:
		void Initialize();
		void Hook(std::string table, std::string prop, NetvarHookCallback callback);
		int GetOffset(const char* tableName, const char* propName);

	private:
		int Get_Prop(const char* tableName, const char* propName, RecvProp** prop = 0);
		int Get_Prop(RecvTable* recvTable, const char* propName, RecvProp** prop = 0);
		RecvTable* GetTable(const char* tableName);
		std::vector< RecvTable* > m_tables;
	}; extern CNetvarHookManager netvar_hook_manager;
}