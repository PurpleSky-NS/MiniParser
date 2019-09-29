﻿#pragma once

#include <unordered_map>
#include <string>
#include "VariousBase.h"

class VariousTable
{
public:

	static inline void ClearTable();
	static inline VariousTable& GetTable();

	inline void UpdateVarious(const std::string& name, VariousBase* various);

	inline VariousBase* GetVarious(const std::string& name)const;

	inline void RemoveVarious(const std::string& name);

private:

	static VariousTable m_varTable;

	VariousTable() = default;
	VariousTable(const VariousTable&) = delete;
	VariousTable(VariousTable&&) = delete;
	~VariousTable() = default;

	std::unordered_map<std::string, VariousBase*> m_variousMap;

};
void VariousTable::ClearTable()
{
	m_varTable.m_variousMap.clear();
}
VariousTable& VariousTable::GetTable()
{
	return m_varTable;
}
void VariousTable::UpdateVarious(const std::string& name, VariousBase* various)
{
	m_variousMap.insert({ name, various });
}

VariousBase* VariousTable::GetVarious(const std::string& name) const
{
	auto fd = m_variousMap.find(name);
	if (fd == m_variousMap.end())
		return nullptr;
	return fd->second;
}

void VariousTable::RemoveVarious(const std::string& name)
{
	m_variousMap.erase(name);
}
