#pragma once

#include <vector>
#include <memory>
#include "Array.h"

class FixedSizeArray : public Array
{
public:

	static FixedSizeArray *GetFixedSizeArray(unsigned size);
	static FixedSizeArray *GetFixedSizeArray(const Array &array);
	static FixedSizeArray *GetFixedSizeArray(const std::vector<double> &initialValues);
	/*size必须比初始化列表的长度大*/
	static FixedSizeArray *GetFixedSizeArray(unsigned size, const std::vector<double> &initialValues);
	static FixedSizeArray *GetFixedSizeArray(unsigned size, const Array &initialValues);

	static void FreeFixedSizeArray(FixedSizeArray *array);

	inline virtual ArrayItem &operator[](unsigned idx)override;
	inline virtual const ArrayItem &operator[](unsigned idx)const override;

	inline virtual bool Contains(double value)const override;
	inline virtual bool ContainsIf(std::function<bool (const ArrayItem &item)> judge)const override;

	inline virtual int IndexOf(double value)const override;
	inline virtual int IndexOf(std::function<bool (const ArrayItem &item)> judge)const override;
	
	inline virtual unsigned Size()const override;

	inline virtual void Foreach(std::function<void(ArrayItem&)> Func)override;
	inline virtual void Foreach(std::function<void(ArrayItem&, unsigned)> Func)override;

	inline virtual void Free()override;

	inline virtual ArrayType GetArrayType()const override;

private:

	ArrayItem *m_values;
	unsigned m_size;

	inline FixedSizeArray(unsigned size);
	inline FixedSizeArray(const std::vector<double> &initialValues);
	inline FixedSizeArray(const Array &array);
	inline FixedSizeArray(unsigned size, const std::vector<double> &initialValues);
	inline FixedSizeArray(unsigned size, const Array &array);
	FixedSizeArray() = delete;
	FixedSizeArray(const FixedSizeArray &) = delete;
	FixedSizeArray(FixedSizeArray&&) = delete;
	inline ~FixedSizeArray();
};

FixedSizeArray::FixedSizeArray(unsigned size)
{
	m_values=new ArrayItem[size];
	m_size=size;
	memset(m_values,0,sizeof(ArrayItem)*size);
}
FixedSizeArray::FixedSizeArray(const std::vector<double> &initialValues)
{
	m_values=new ArrayItem[initialValues.size()];
	m_size=initialValues.size();
	for(unsigned i=0;i<initialValues.size();++i)
		m_values[i].value=initialValues[i];
}
FixedSizeArray::FixedSizeArray(const Array &array)
{
	m_values=new ArrayItem[array.Size()];
	m_size=array.Size();
	for(unsigned i=0;i<array.Size();++i)
		m_values[i].value=array[i].value;
}
FixedSizeArray::FixedSizeArray(unsigned capacity,const std::vector<double> &initialValues)
	:FixedSizeArray(capacity)
{
	for(unsigned i=0;i<initialValues.size();++i)
		m_values[i].value=initialValues[i];
}
FixedSizeArray::FixedSizeArray(unsigned capacity,const Array &array)
	:FixedSizeArray(capacity)
{
	for(unsigned i=0;i<array.Size();++i)
		m_values[i].value=array[i].value;
}
FixedSizeArray::~FixedSizeArray()
{
	delete[] m_values;
}
FixedSizeArray::ArrayItem &FixedSizeArray::operator[](unsigned idx)
{
	return m_values[idx];
}
const FixedSizeArray::ArrayItem &FixedSizeArray::operator[](unsigned idx)const
{
	return m_values[idx];
}
bool FixedSizeArray::Contains(double value)const
{
	for(unsigned i=0;i<m_size;++i)
		if(m_values[i].value==value)
			return true;
	return false;
}
bool FixedSizeArray::ContainsIf(std::function<bool (const ArrayItem &item)> judge)const
{
	for(unsigned i=0;i<m_size;++i)
		if(judge(m_values[i]))
			return true;
	return false;
}
int FixedSizeArray::IndexOf(double value)const
{
	for(unsigned i=0;i<m_size;++i)
		if(m_values[i].value==value)
			return (int)i;
	return -1;
}
int FixedSizeArray::IndexOf(std::function<bool (const ArrayItem &item)> judge)const
{
	for(unsigned i=0;i<m_size;++i)
		if(judge(m_values[i]))
			return (int)i;
	return -1;
}
unsigned FixedSizeArray::Size()const
{
	return m_size;
}
void FixedSizeArray::Foreach(std::function<void(ArrayItem&)> Func)
{
	for(unsigned i=0;i<m_size;++i)
		Func(m_values[i]);
}
void FixedSizeArray::Foreach(std::function<void(ArrayItem&,unsigned)> Func)
{
	for(unsigned i=0;i<m_size;++i)
		Func(m_values[i],i);
}
void FixedSizeArray::Free()
{
	FreeFixedSizeArray(this);
}

FixedSizeArray::ArrayType FixedSizeArray::GetArrayType() const
{
	return FixedSizeArray::ArrayType::FixedSizeArray;
}