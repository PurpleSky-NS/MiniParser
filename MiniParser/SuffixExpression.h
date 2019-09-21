﻿#pragma once

#include "InfixExpression.h"

class SuffixExpression :public Expression
{
public:
	inline SuffixExpression() = default;
	/*中缀表达式类构造*/
	inline SuffixExpression(const InfixExpression *expression);
	/*中缀表达式字符串构造*/
	inline SuffixExpression(const std::string &expression);
	SuffixExpression(const SuffixExpression&) = delete;
	SuffixExpression(SuffixExpression&&) = delete;
	~SuffixExpression() = default;
};
