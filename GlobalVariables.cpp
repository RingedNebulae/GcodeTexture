#include "GlobalVariables.h"

GlobalVariables::GlobalVariables()
{
}


GlobalVariables::~GlobalVariables()
{
}

void GlobalVariables::readInConfigure()
{
	//定义输入变量
	char buffer[256];//读取文件缓存
	string inputbuffer;//格式转化临时变量

	ifstream in("Configure");
	if (!in.is_open())
	{
		//错误检查，确保文件打开正确
		cout << "Error opening file!";
		return ;
	}
	while (!in.eof())
	{
		in.getline(buffer, 200);
		inputbuffer = buffer;

		//以斜杠开头的是文件中的注释，忽略并读入下一行
		if (inputbuffer[0] == '/')
			continue;
		//INPUT_FILE_PATH 是输入文件路径的标识符
		else if (inputbuffer == "INPUT_FILE_PATH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setInputFilePath(inputbuffer);
			cout << "Input file path is: " << inputFilePath << endl;
		}
		//OUTPUT_FILE_PATH 是输出文件路径标识符
		else if (inputbuffer == "OUTPUT_FILE_PATH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setOutputFilePath(inputbuffer);
			cout << "Output file path is: " << outputFilePath << endl;
		}
		//INPUT_GCODE_NAME 是读取的文件名
		else if (inputbuffer == "INPUT_GCODE_NAME")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setInputGcodeName(inputbuffer);
			cout << "Input Gcode name is: " << inputGcodeName << endl;
		}
		//TEXTURE_TYPE 是生成纹理的类型
		else if (inputbuffer == "TEXTURE_TYPE")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			if (inputbuffer == "Stripe")
			{
				texture = Stripe;
			}
			else if (inputbuffer == "Wave")
			{
				texture = Wave;
			}
			else if (inputbuffer == "ZWave")
			{
				texture = ZWave;
			}
			else if (inputbuffer == "Hybrid")
			{
				texture = Hybrid;
			}
		}
		//GAP_LENGTH 是生成sine时的步长
		else if (inputbuffer == "GAP_LENGTH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setGapLength(stof(inputbuffer));

		}
		//WAVE_LENGTH 是初始波长
		else if (inputbuffer == "WAVE_LENGTH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setWavelength(stof(inputbuffer));
		}
		//AMPLITUDE 是正弦波幅度
		else if (inputbuffer == "AMPLITUDE")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setAmplitude(stof(inputbuffer));
		}
	}
}

void GlobalVariables::setInputFilePath(string inString)
{
	inputFilePath = inString;
}

string GlobalVariables::getInputFilePath()
{
	return inputFilePath;
}

void GlobalVariables::setOutputFilePath(string inString)
{
	outputFilePath = inString;
}

string GlobalVariables::getOutputFilePath()
{
	return outputFilePath;
}

void GlobalVariables::setInputGcodeName(string inString)
{
	inputGcodeName = inString;
}

string GlobalVariables::getInputGcodeName()
{
	return inputGcodeName;
}

void GlobalVariables::setGapLength(float inValue)
{
	gapLength = inValue;
}

float GlobalVariables::getGapLength()
{
	return gapLength;
}

void GlobalVariables::setAmplitude(float inValue)
{
	amplitude = inValue;
}

float GlobalVariables::getAmplitude()
{
	return amplitude;
}

void GlobalVariables::setWavelength(float inValue)
{
	wavelength = inValue;
}

float GlobalVariables::getWavelength()
{
	return wavelength;
}

void GlobalVariables::setTextureType(TextureType type)
{
	texture = type;
}

TextureType GlobalVariables::getTextureType()
{
	return texture;
}


