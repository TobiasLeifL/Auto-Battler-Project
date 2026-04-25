#pragma once

inline int NextID()
{
	static int counter = 0;
	counter = counter + 1;
	return counter;
}

template <typename T>
static int GetTypeID()
{
	static int id = NextID();
	return id;
}