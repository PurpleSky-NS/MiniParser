﻿#pragma once

#include <functional>
#include "Statement.h"
#include "SuffixExpression.h"
#include "Calculator.h"
#include "CalculatorParser.h"

/*基础的赋值语句*/
class AssignStatement :public Statement
{
public:

	AssignStatement() = default;
	AssignStatement(const AssignStatement&) = delete;
	AssignStatement(AssignStatement&&) = delete;
	~AssignStatement() = default;

	inline bool SetStatement(const std::string& leftStr, const std::string& rightStr);

	inline virtual bool Check() override;

	inline virtual bool Execute() override;

	inline virtual void Clear();

	inline virtual StatementType GetType()const;

	inline virtual void Save(std::ostream& out)const;

	inline virtual void Load(std::istream& in);

private:

	VariousIDF m_leftVar;
	SuffixExpression m_rightExpression;
};

bool AssignStatement::SetStatement(const std::string& leftStr, const std::string& rightStr)
{
	/*a=12
	a[5]=4+568-sd*/
	Clear();
	InfixExpression infExp;
	if (!(infExp.ParseExpression(leftStr)//转化成功
		&& infExp.GetExpression().size() == 1//转化后只有一个元素
		&& infExp.GetExpression()[0]->GetType() == ItemBase::Identification//该元素是个标识符
		&& ((IdentificationItem*)infExp.GetExpression()[0])->GetIdentificationType() == IdentificationItem::VariousIDF))//该元素是个变量
	{
		CompileError("[" + leftStr + "]这好像不是一个变量吧...");
		return false;
	}
	VariousIDF& left_vidf = *(VariousIDF*)infExp.GetExpression()[0];
	m_leftVar.GetName() = left_vidf.GetName();
	m_leftVar.NegSigned() = left_vidf.NegSigned();
	m_leftVar.SetToArrayItem(left_vidf.IsArrayItem());
	m_leftVar.ArrayPosExpression() = std::move(left_vidf.ArrayPosExpression());

	if (!(infExp.ParseExpression(rightStr) && m_rightExpression.ParseExpression(infExp)))
	{
		CompileError("右边的表达式有点问题吧...");
		return false;
	}
	return true;
}

bool AssignStatement::Check()
{
	if (m_leftVar.IsArrayItem() && !Statement::Check(m_leftVar.ArrayPosExpression()) || Statement::Check(m_rightExpression))
		return false;
	m_program->var_table.UpdateVarious(m_leftVar.GetName(), nullptr);
	return true;
}

bool AssignStatement::Execute()
{
	VariousBase* varBase = m_program->var_table.GetVarious(m_leftVar.GetName());
	if (varBase == nullptr)
	{
		if (m_leftVar.IsArrayItem())//要使用数组成员但之前未定义
		{
			RuntimeError("[" + m_leftVar.GetName() + "]数组之前没定义过呢...");
			return false;
		}
		varBase = m_program->var_pool.GetObject();
	}
	if (m_leftVar.IsArrayItem())
	{
		if (varBase->GetType() != VariousBase::Array)//之前存在但是个变量，非数组
		{
			RuntimeError("[" + m_leftVar.GetName() + "]数组之前没定义过呢...");
			return false;
		}
		Array* arr = (Array*)varBase;
		unsigned pos;
		if (!CalculateToUnsigned(m_leftVar.ArrayPosExpression(), pos))
			return false;
		if (pos >= arr->Size())
		{
			RuntimeError("数组[" + m_leftVar.GetName() + "]下标越界了...[总大小:" + std::to_string(arr->Size()) + "][请求下标:" + std::to_string(pos) + "]");
			return false;
		}
		if (!CalculateExpression(m_rightExpression, (*arr)[pos]))
			return false;
	}
	else
	{
		/*之前是数组，现在用变量覆盖*/
		if (varBase->GetType() == VariousBase::Array)
		{
			FreeVarious(varBase);
			varBase = m_program->var_pool.GetObject();//成为变量
		}
		if (!CalculateExpression(m_rightExpression, ((Various*)varBase)->GetValue()))
			return false;
	}
	m_program->var_table.UpdateVarious(m_leftVar.GetName(), varBase);
	return true;
}

void AssignStatement::Clear()
{
	m_leftVar.Clear();
	m_rightExpression.Clear();
}

AssignStatement::StatementType AssignStatement::GetType() const
{
	return StatementType::AssignStatement;
}

void AssignStatement::Save(std::ostream& out) const
{
	CalculatorParser::Save(out, m_leftVar);
	CalculatorParser::Save(out, m_rightExpression.GetExpression());
}

void AssignStatement::Load(std::istream& in)
{
	CalculatorParser::Load(in, m_leftVar);
	CalculatorParser::Load(in, m_rightExpression.GetExpression());
}
