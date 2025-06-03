#pragma once

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	float ReadSec() const;
	float ReadMSec() const;

private:
	int startTime;
};