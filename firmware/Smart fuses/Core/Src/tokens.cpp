/*
 * tokens.cpp
 *
 *  Created on: 27 wrz 2022
 *      Author: pile
 */

#include "tokens.hpp"

bool HandlerToken::enabled()
{
	return _enable;
}

void HandlerTokenSource::start()
{
	token._enable = true;
}

void HandlerTokenSource::stop()
{
	token._enable = false;
}

bool HandlerTokenSource::enabled()
{
	return token._enable;
}

HandlerToken* HandlerTokenSource::getToken()
{
	return &token;
}
