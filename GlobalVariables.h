#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include "GlobalVariables.h"

using namespace std;

enum TextureType
{
	Stripe,
	Wave,
	ZWave,
	Hybrid
};

class GlobalVariables
{
public:
	GlobalVariables();
	~GlobalVariables();

	void readInConfigure();

	//定义控制变量
	string inputFilePath;
	string outputFilePath;
	string inputGcodeName;

	float gapLength;
	float amplitude;
	float wavelength;

	TextureType texture;

	void setInputFilePath(string inString);
	string getInputFilePath();

	void setOutputFilePath(string inString);
	string getOutputFilePath();

	void setInputGcodeName(string inString);
	string getInputGcodeName();

	void setGapLength(float inValue);
	float getGapLength();

	void setAmplitude(float inValue);
	float getAmplitude();

	void setWavelength(float inValue);
	float getWavelength();

	void setTextureType(TextureType type);
	TextureType getTextureType();
};

