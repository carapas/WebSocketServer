// CmdForwarder.h
// Credit: R0mai
#pragma once
#include <stdarg.h>
#include <list>

template <typename RetType, typename... Ts>
RetType callCmd(void* fp, const Ts&& ... args)
{
	return reinterpret_cast<RetType(*)(Ts...)>(fp)(args...);
}

class CmdForwarder
{
private:
	map<string, void*>	m_cmds;
	map<void*, string> m_cmdsDesc;

	void* getCmd(string sKey)
	{
		return m_cmds.count(sKey) == 1 ? m_cmds[sKey]: NULL;
	}

public:
	CmdForwarder() {}
	~CmdForwarder() {}
	
	bool isCmd(string sKey)
	{
		return m_cmds.count(sKey) == 1;
	}

	map<string, list<string>> getCmdDesc()
	{
		// Description, list of keys
		map<string, list<string>> ret;
		
		string sDesc;
		for(pair<string, void*> entry: m_cmds) {
			sDesc = m_cmdsDesc[entry.second];
			
			if(ret.count(sDesc) == 1) {
				ret[sDesc].push_back(entry.first);
			}
			else {
				list<string> sub;
				sub.push_back(entry.first);
				ret.insert(map<string, list<string>>::
					value_type(sDesc, sub));
			}
		}

		return ret;
	}

	/**
	 * Takes function pointer and match it with a set of keys
	 */
	void registerCmd(void* fp, string sDesc, size_t nKeys, ...)
	{
		va_list vl;
		va_start(vl, nKeys);
		string sKey;
		for(size_t i = 0; i < nKeys; ++i) {
			sKey = va_arg(vl, const char *);
			m_cmds.insert(map<string, void*>::
				value_type(sKey, fp));
			m_cmdsDesc.insert(map<void*, string>::
				value_type(fp, sDesc));
		}
		va_end(vl);
	}
	
	template <typename RetType, typename ... Ts>
	RetType retrieveAndCallCmd(string sKey, const Ts&& ... args)
	{
		void* fp = getCmd(sKey);
		return fp? callCmd<RetType, Ts...>(fp, forward<Ts>(args)...) : NULL;
	}
};