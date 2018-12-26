#include "GlobalVariables.h"

GlobalVariables::GlobalVariables()
{
}


GlobalVariables::~GlobalVariables()
{
}

void GlobalVariables::readInConfigure()
{
	//�����������
	char buffer[256];//��ȡ�ļ�����
	string inputbuffer;//��ʽת����ʱ����

	ifstream in("Configure");
	if (!in.is_open())
	{
		//�����飬ȷ���ļ�����ȷ
		cout << "Error opening file!";
		return ;
	}
	while (!in.eof())
	{
		in.getline(buffer, 200);
		inputbuffer = buffer;

		//��б�ܿ�ͷ�����ļ��е�ע�ͣ����Բ�������һ��
		if (inputbuffer[0] == '/')
			continue;
		//INPUT_FILE_PATH �������ļ�·���ı�ʶ��
		else if (inputbuffer == "INPUT_FILE_PATH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setInputFilePath(inputbuffer);
			cout << "Input file path is: " << inputFilePath << endl;
		}
		//OUTPUT_FILE_PATH ������ļ�·����ʶ��
		else if (inputbuffer == "OUTPUT_FILE_PATH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setOutputFilePath(inputbuffer);
			cout << "Output file path is: " << outputFilePath << endl;
		}
		//INPUT_GCODE_NAME �Ƕ�ȡ���ļ���
		else if (inputbuffer == "INPUT_GCODE_NAME")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setInputGcodeName(inputbuffer);
			cout << "Input Gcode name is: " << inputGcodeName << endl;
		}
		//TEXTURE_TYPE ���������������
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
		//GAP_LENGTH ������sineʱ�Ĳ���
		else if (inputbuffer == "GAP_LENGTH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setGapLength(stof(inputbuffer));

		}
		//WAVE_LENGTH �ǳ�ʼ����
		else if (inputbuffer == "WAVE_LENGTH")
		{
			in.getline(buffer, 100);
			inputbuffer = buffer;
			setWavelength(stof(inputbuffer));
		}
		//AMPLITUDE �����Ҳ�����
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


