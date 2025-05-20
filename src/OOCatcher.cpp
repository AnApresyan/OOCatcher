#include "OOCatcher.h"
OOCatcher::~OOCatcher()
{
	for (auto s : segments)
		delete s;
}