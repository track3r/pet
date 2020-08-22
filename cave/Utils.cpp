#include "pch.h"
#include "Utils.h"

float randf()
{
	return (float)rand() / (float) RAND_MAX;
}

float randfun()
{
	return (float)rand() / (float)RAND_MAX - 0.5f;
}