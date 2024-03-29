﻿#pragma once

#include "Program.h"
#include "Array.h"
#include "Various.h"
#include "SuffixExpression.h"

class Statement :public StatementBase
{
protected:

	inline void CompileError(const std::string& msg)const;
	inline void RuntimeError(const std::string& msg)const;

	/*静态检查*/
	inline bool Check(const ExpressionType& expression)const;
	/*动态检查，在程序中查找标识符*/
	inline bool IsContextHasIDF(IdentificationItem* idf)const;
	/*动态检查，在变量表中查找变量*/
	inline bool IsContextHasIDF(VariousIDF* var)const;
	/*动态检查，在程序中查找函数*/
	inline bool IsContextHasIDF(FunctionIDF* func)const;

	/*获取标识符所代表的数值*/
	inline bool GetIdentificationValue(IdentificationItem* idf, double& val)const;
	/*获取变量标识符所代表的数值*/
	inline bool GetIDFValue(VariousIDF* idf, double& val)const;
	/*获取函数标识符所代表的数值*/
	inline bool GetIDFValue(FunctionIDF* idf, double& val)const;

	/*会打印错误信息并且可能会触发错误退出*/
	inline bool CalculateExpression(const ExpressionType& exp, double& ret)const;
	/*表达式计算出整数，如果计算错误或者不是无符号整数会打印错误信息并触发错误退出*/
	inline bool CalculateToUnsigned(const ExpressionType& exp, unsigned& ret)const;

	/*释放变量*/
	inline void FreeVarious(VariousBase* var)const;

};
inline void Statement::CompileError(const std::string& msg)const
{
	m_program->err_log.AddCompileError(msg, GetLine());
}

inline void Statement::RuntimeError(const std::string& msg)const
{
	m_program->err_log.AddRuntimeError(msg, GetLine());
	m_program->OnErrorFinish();
}

bool Statement::Check(const ExpressionType& expression)const
{
	if (!Calculator::CheckExpression(expression))
	{
		CompileError("你这个表达式写的有问题呀~");
		return false;
	}
	for (auto& i : expression)
		if (i->GetType() == ItemBase::Identification && !IsContextHasIDF((IdentificationItem*)i))
			return false;
	return true;
}

bool Statement::IsContextHasIDF(IdentificationItem* idf) const
{
	if (idf->GetIdentificationType() == IdentificationItem::VariousIDF)
		return IsContextHasIDF((VariousIDF*)idf);
	return IsContextHasIDF((FunctionIDF*)idf);
}

bool Statement::IsContextHasIDF(VariousIDF* var)const
{
	if (!m_program->var_table.ExistVarious(var->GetName()))
	{
		CompileError("你这个变量[" + var->GetName() + "]之前好像没定义过吧...");
		return false;
	}
	return true;
}

bool Statement::IsContextHasIDF(FunctionIDF* func)const
{
	if (func->GetName() != m_program->GetName() && !Program::ExistProgram(func->GetName()))//如果不存在就报错
	{
		CompileError("[" + func->GetName() + "]这调用的啥，原谅我智商低识别不出来...");
		return false;
	}
	for (auto& i : func->Params())//查参数
		for (auto& j : i)
			if (j->GetType() == ItemBase::Identification && !IsContextHasIDF((IdentificationItem*)j))
				return false;
	return true;
}

inline bool Statement::GetIdentificationValue(IdentificationItem* idf, double& val)const
{
	if (idf->GetIdentificationType() == IdentificationItem::VariousIDF)
		return GetIDFValue((VariousIDF*)idf, val);
	else
		return GetIDFValue((FunctionIDF*)idf, val);
}

inline bool Statement::GetIDFValue(VariousIDF* idf, double& val) const
{
	VariousBase* varBase = m_program->var_table.GetVarious(idf->GetName());
	if (idf->IsArrayItem())
	{
		if (varBase->GetType() != VariousBase::Array)
		{
			RuntimeError("[" + idf->GetName() + "]标识符并不是数组");
			return false;
		}
		Array* arr = (Array*)varBase;
		unsigned pos;
		if (!CalculateToUnsigned(idf->ArrayPosExpression(), pos))
		{
			RuntimeError("数组[" + idf->GetName() + "]下标不是正整数或0呀");
			return false;
		}
		if (pos >= arr->Size())
		{
			RuntimeError("数组[" + idf->GetName() + "]下标越界了...[总大小:" + std::to_string(arr->Size()) + "][请求下标:" + std::to_string(pos) + "]");
			return false;
		}
		val = (idf->NegSigned() ? -(*(Array*)varBase)[pos] : (*(Array*)varBase)[pos]);
		return true;
	}
	if (varBase->GetType() == VariousBase::Array)
	{
		RuntimeError("[" + idf->GetName() + "]是个数组，并不是变量呀...给个下标如何...");
		return false;
	}
	val = (idf->NegSigned() ? -((Various*)varBase)->GetValue() : ((Various*)varBase)->GetValue());
	return true;
}

inline bool Statement::GetIDFValue(FunctionIDF* idf, double& val) const
{
	val = 0;
	return true;
}

inline bool Statement::CalculateExpression(const ExpressionType& exp, double& ret) const
{
	Calculator::CalculateResult res;
	ret = Calculator::Calculate(exp, std::bind(&Statement::GetIdentificationValue, this, std::placeholders::_1, std::placeholders::_2), m_program->val_pool, res);
	if (res == Calculator::Succeed)
		return true;
	else if (res == Calculator::ExpressionError)
		RuntimeError("表达式计算错误...");
	else if (res == Calculator::MathError)
		RuntimeError("计算发生数学错误...");
	else if (res == Calculator::IDFError)
		RuntimeError("计算时遇到未知变量...");
	else
		RuntimeError("计算发生未知错误...");
	return false;
}

inline bool Statement::CalculateToUnsigned(const ExpressionType& exp, unsigned& ret) const
{
	double val;
	if (!CalculateExpression(exp, val))
		return false;
	if (!Calculator::IsDigit(val))
	{
		RuntimeError("计算结果非整数...");
		return false;
	}
	if (val < 0)
	{
		RuntimeError("计算结果是负数...");
		return false;
	}
	ret = (unsigned)val;
	return true;
}

inline void Statement::FreeVarious(VariousBase* var) const
{
	switch (var->GetType())
	{
	case VariousBase::Various:
		m_program->var_pool.FreeObject((Various*)var);//回收
		break;
	case VariousBase::Array:
		((Array*)var)->Clear();//先释放一些内存
		m_program->arr_pool.FreeObject((Array*)var);//回收
		break;
	}
}
