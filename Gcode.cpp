#include "Gcode.h"


Gcode::Gcode()
{
}


Gcode::~Gcode()
{
}

bool Gcode::readInCuraGcode(string fileName)
{
	string inputbuffer;//Ϊ�˶����ļ��ı��ڸ�ʽת��
	vector<string> inputGcode;

	ifstream openGcode(fileName);
	if (!openGcode.is_open())
	{
		cout << "Error opening file!";
		return false;
	}

	//���Ȱ�Gcode�ļ�ȫ����������������inputGcode������
	while (getline(openGcode, inputbuffer))
	{
		inputGcode.push_back(inputbuffer);
	}

	//��ʼ�Զ�ȡ��Gcode���н���
	bool isStartGcode = true;
	bool isMainBody = false;
	bool isEndGcode = false;

	eachLine tmpLine;
	LayerInfo tmpLayer;
	contour tmpContour;
	int contourIndex = 0;

	//���жԶ�ȡ��gcode�ļ����н���
	for (int i = 0; i < inputGcode.size(); i++)
	{
		inputbuffer = inputGcode.at(i);

		auto isFound = inputbuffer.find("M117 Printing...");
		if (isFound != string::npos)
		{
			//���ҵ�"M117 Printing...",˵������startGcode�����һ��
			isStartGcode = false;
			isMainBody = true;
		}

		isFound = inputbuffer.find("End GCode");
		if (isFound != string::npos)
		{
			//���ҵ�End GCode������ʼEndGcode����
			isMainBody = false;
			isEndGcode = true;
		}

		if (isStartGcode)
		{
			//startGcode�еĴ���ֱ�ӱ��棬���账��
			startGcode.push_back(inputbuffer);
			continue;
		}
		else if (isMainBody)
		{
			//����mainBody�еĴ��룬���н���

			//�ж��Ƿ�ʼ�µ�һ��
			isFound = inputbuffer.find(";LAYER:");
			if (isFound != string::npos)
			{
				//��ʼ�µ�һ��ǰ���Ƚ���һ���contour��Ϣ����
				if (tmpContour.wallOuterContainer.size() != 0)
				{
					tmpLayer.contourContainer.push_back(tmpContour);
					contourIndex = 0;
					//Ȼ�󽫵�ǰcontour��գ�׼�����µ�contour
					clearContourContainer(tmpContour);

				}
				//������һ��ʱ�ѵ�ǰ�����Ϣ����mainbody
				if (tmpLayer.contourContainer.size() != 0)
				{
					mainBody.push_back(tmpLayer);
					//��ʼ�µ�һ�㣬���tmpLayer
					tmpLayer.contourContainer.clear();
					tmpLayer.lineContainer.clear();
					tmpLayer.skirtContainer.clear();
					tmpLine.codetype = UNDEFINED;
				}

				//������ǰ��ı��
				int cnt_int = 0;
				isFound = inputbuffer.find(':');
				for (int i = isFound + 1; i < inputbuffer.size(); i++)
				{
					cnt_int *= 10;
					cnt_int += inputbuffer[i] - '0';
				}
				tmpLayer.layer = cnt_int;

				tmpLine.codetype = UNDEFINED;
			}

			//�ж�һ�����Ƿ�ʼһ���µ�contour
			if (inputbuffer.find(";TYPE:WALL-INNER") != string::npos)
			{
				//��һ����ÿ����һ��wall-inner,��ʾ��ʼһ���µ�contour
				//���Ƚ���ǰcontour���浽layerContainer��
				if (tmpContour.wallOuterContainer.size() != 0)
				{
					tmpLayer.contourContainer.push_back(tmpContour);
					//Ȼ�󽫵�ǰcontour��գ�׼�����µ�contour
					clearContourContainer(tmpContour);
					//contour����������1
					contourIndex++;

				}
				//�����µ�contour
				tmpLine.codetype = WALL_INNER;
				tmpContour.contourIndex = contourIndex;
				tmpContour.typeSequence.push_back(WALL_INNER);

			}
			//�жϵ�ǰ�еĴ�������
			else if (inputbuffer.find(";TYPE:WALL-OUTER") != string::npos)
			{
				//����������������������Ҳ��ʼ�µ�����
				//���ں̵ܶ����������ܲ�����wallinner��ֱ�ӿ�ʼwallouter
				if (tmpContour.typeSequence.size() == 0)
				{
					//�����µ�contour
					tmpLine.codetype = WALL_OUTER;
					tmpContour.contourIndex = contourIndex;
					tmpContour.typeSequence.push_back(WALL_OUTER);
				}
				else if (tmpContour.typeSequence.at(tmpContour.typeSequence.size() - 1) != WALL_INNER)
				{
					if (tmpContour.wallOuterContainer.size() != 0)
					{
						tmpLayer.contourContainer.push_back(tmpContour);
						//Ȼ�󽫵�ǰcontour��գ�׼�����µ�contour
						clearContourContainer(tmpContour);
						//contour����������1
						contourIndex++;
					}
					//�����µ�contour
					tmpLine.codetype = WALL_OUTER;
					tmpContour.contourIndex = contourIndex;
					tmpContour.typeSequence.push_back(WALL_OUTER);
				}
				else
				{
					tmpLine.codetype = WALL_OUTER;
					tmpContour.typeSequence.push_back(WALL_OUTER);
				}

			}
			else if (inputbuffer.find(";TYPE:SKIN") != string::npos)
			{
				tmpLine.codetype = SKIN;
				tmpContour.typeSequence.push_back(SKIN);
			}
			else if (inputbuffer.find(";TYPE:FILL") != string::npos)
			{
				tmpLine.codetype = FILL;
				tmpContour.typeSequence.push_back(FILL);
			}
			else if (inputbuffer.find(";TYPE:SUPPORT") != string::npos)
			{
				tmpLine.codetype = SUPPORT;
				tmpContour.typeSequence.push_back(SUPPORT);
			}
			else if (inputbuffer.find(";TYPE:SKIRT") != string::npos)
			{
				tmpLine.codetype = SKIRT;
			}

			//������
			parseCommandLine(inputbuffer, tmpLine);

			//������Ӧ������
			if (tmpLine.codetype == WALL_INNER)
				tmpContour.wallInnerContainer.push_back(tmpLine);
			else if (tmpLine.codetype == WALL_OUTER)
				tmpContour.wallOuterContainer.push_back(tmpLine);
			else if (tmpLine.codetype == SKIN)
				tmpContour.skinContainer.push_back(tmpLine);
			else if (tmpLine.codetype == FILL)
				tmpContour.fillContainer.push_back(tmpLine);
			else if (tmpLine.codetype == SUPPORT)
				tmpContour.supportContainer.push_back(tmpLine);
			else if (tmpLine.codetype == SKIRT)
				tmpLayer.skirtContainer.push_back(tmpLine);
			else if (tmpLine.codetype == UNDEFINED)
				tmpLayer.lineContainer.push_back(tmpLine);

			//�����ǩΪwall-outerʱ��������������������
			if (tmpLine.codetype == WALL_OUTER)
			{
				if (isNewContour(inputGcode, i, inputbuffer))
				{
					//����һ�п�ʼ�µ�����
					if (tmpContour.wallOuterContainer.size() != 0)
					{
						tmpLayer.contourContainer.push_back(tmpContour);
						//Ȼ�󽫵�ǰcontour��գ�׼�����µ�contour
						clearContourContainer(tmpContour);
						//contour����������1
						contourIndex++;

					}
					//�����µ�contour
					tmpLine.codetype = WALL_OUTER;
					tmpContour.contourIndex = contourIndex;
					tmpContour.typeSequence.push_back(WALL_OUTER);

				}

			}

			//����tmpline
			tmpLine.otherCommand.clear();

		}
		else if (isEndGcode)
		{
			//endGcode�еĴ���ֱ�ӱ��棬���账��
			endGcode.push_back(inputbuffer);
			continue;
		}
		else
		{
			//�������ĳ�����벻���������е���һ�࣬�򱨴�
			cout << "Wrong Gcode type!" << endl;
			return false;
		}
	}
}

bool Gcode::isNewContour(const vector<string> &inputGcode, int index, string inputbuffer)
{
	regex reg1("^G0+.*X+.*Y+.*");//ƥ��G0
	regex reg2("^G1 F\\d+ E\\d+\.?\\d*");//ƥ��G1 F E ָ��
	regex reg3("^G1+ Z+.*");//ƥ��G1 Zָ��
	regex reg4("^G1 F?\\d* ?X\\d+\.?\\d* Y\\d+\.?\\d* E\\d+\.?\\d*");//ƥ��������G1ָ��
	smatch r1,r2,r3,r4,r5,r6;
	//������һ�����ģ�����һ��wall-outer���ж������
	//G0 -> G1 E -> G1 Z -> G0 -> G1 Z -> G1 E 
	if (regex_match(inputGcode.at(index), r1, reg1)
		&& regex_match(inputGcode.at(index + 1), r2, reg2)
		&& regex_match(inputGcode.at(index + 2), r3, reg3)
		&& regex_match(inputGcode.at(index + 3), r4, reg1)
		&& regex_match(inputGcode.at(index + 4), r5, reg3)
		&& regex_match(inputGcode.at(index + 5), r6, reg2))
	{
		return true;
	}
	//������һ�����ģ�����һ��wall-outer���ж������
	//G1 X Y E -> G0 -> G1 X Y E
	else if (regex_match(inputGcode.at(index), r1, reg4)
		&& regex_match(inputGcode.at(index + 1), r2, reg1)
		&& regex_match(inputGcode.at(index + 2), r3, reg4))
	{
		return true;
	}
	//��ʱֻ�ҵ�������pattern��һ��wall-outer��������������
	//������������ټӡ���
	else
	{
		return false;
	}
	
}

void Gcode::parseCommandLine(string inputbuffer, eachLine &tmpLine)
{
	string tmpString;//�����ַ�����ʱ�����Ĵ洢
	if (inputbuffer[0] == 'G')
	{
		GCommand tmpGcodeLine;
		tmpLine.isGcommand = true;
		istringstream lineRecord(inputbuffer);
		while (lineRecord >> tmpString)
		{
			switch (tmpString[0])
			{
			case 'G':
				tmpGcodeLine.Gcommand = tmpString;
				continue;
			case 'F':
				tmpString.erase(tmpString.begin());
				tmpGcodeLine.feedRate = std::stof(tmpString);
				continue;
			case 'X':
				tmpString.erase(tmpString.begin());
				tmpGcodeLine.X.isValidValue = true;
				tmpGcodeLine.X.value = std::stof(tmpString);
				continue;
			case 'Y':
				tmpString.erase(tmpString.begin());
				tmpGcodeLine.Y.isValidValue = true;
				tmpGcodeLine.Y.value = std::stof(tmpString);
				continue;
			case 'Z':
				tmpString.erase(tmpString.begin());
				tmpGcodeLine.Z.isValidValue = true;
				tmpGcodeLine.Z.value = std::stof(tmpString);
				continue;
			case 'E':
				tmpString.erase(tmpString.begin());
				tmpGcodeLine.E.isValidValue = true;
				tmpGcodeLine.E.value = std::stof(tmpString);
				continue;
			default:
				cout << "Gcode line reading error!" << endl;
				break;
			}
		}
		tmpLine.GcommandLine = tmpGcodeLine;

	}
	else
	{
		tmpLine.isGcommand = false;
		tmpLine.otherCommand = inputbuffer;
	}
}

void Gcode::clearContourContainer(contour & tmpContour)
{
	//��tmpContour�еĸ����������
	tmpContour.contourIndex = -1;
	tmpContour.skinContainer.clear();
	tmpContour.fillContainer.clear();
	tmpContour.supportContainer.clear();
	tmpContour.typeSequence.clear();
	tmpContour.wallInnerContainer.clear();
	tmpContour.wallOuterContainer.clear();

}

bool Gcode::outputGcode(string fileName)
{
	ofstream out(fileName);
	if (!out.good()) {
		std::cout << "Cannot open output file\n";
		return false;
	}
	//���startgcode
	for (int i = 0; i < startGcode.size(); i++)
	{
		out << startGcode.at(i) << endl;
	}

	//���gcode����
	for (int i = 0; i < mainBody.size(); i++)
	{
		LayerInfo tmpLayer = mainBody.at(i);
		//�����ǰ�������ָ��
		for (int j = 0; j < tmpLayer.lineContainer.size(); j++)
		{
			eachLine tmpline = tmpLayer.lineContainer.at(j);
			outputLine(tmpline, out);
		}
		if (tmpLayer.skirtContainer.size()>0)
		{
			for (int j = 0; j < tmpLayer.skirtContainer.size(); j++)
			{
				eachLine tmpline = tmpLayer.skirtContainer.at(j);
				outputLine(tmpline, out);
			}
		}
		//�����ǰ���ڵĸ�������
		for (int j = 0; j <tmpLayer.contourContainer.size(); j++)
		{
			contour tmpcontour = tmpLayer.contourContainer.at(j);
			for (int p = 0; p < tmpcontour.typeSequence.size(); p++)
			{
				switch (tmpcontour.typeSequence.at(p))
				{
				case WALL_INNER:
				{
					for (int k = 0; k < tmpcontour.wallInnerContainer.size(); k++)
					{
						eachLine tmpline = tmpcontour.wallInnerContainer.at(k);
						outputLine(tmpline, out);
					}
					break;
				}
				case WALL_OUTER:
				{
					for (int k = 0; k < tmpcontour.wallOuterContainer.size(); k++)
					{
						eachLine tmpline = tmpcontour.wallOuterContainer.at(k);
						outputLine(tmpline, out);
					}
					break;
				}
				case FILL:
				{
					for (int k = 0; k < tmpcontour.fillContainer.size(); k++)
					{
						eachLine tmpline = tmpcontour.fillContainer.at(k);
						outputLine(tmpline, out);
					}
					break;
				}
				case SKIN:
				{
					for (int k = 0; k < tmpcontour.skinContainer.size(); k++)
					{
						eachLine tmpline = tmpcontour.skinContainer.at(k);
						outputLine(tmpline, out);
					}
					break;
				}
				case SUPPORT:
				{
					for (int k = 0; k < tmpcontour.supportContainer.size(); k++)
					{
						eachLine tmpline = tmpcontour.supportContainer.at(k);
						outputLine(tmpline, out);
					}
					break;
				}
				default:
					break;
				}
			}

		}
	}

	//���endgcode
	for (int i = 0; i < endGcode.size(); i++)
	{
		out << endGcode.at(i) << endl;
	}

	return true;
}

void Gcode::outputLine(eachLine tmpline, ofstream &out)
{
	if (!tmpline.isGcommand)
	{
		out << tmpline.otherCommand << endl;
	}
	else if (tmpline.isGcommand)
	{
		out << tmpline.GcommandLine.Gcommand << " ";
		if (tmpline.GcommandLine.feedRate != -1)
		{
			out << "F" << tmpline.GcommandLine.feedRate << " ";
		}
		if (tmpline.GcommandLine.X.isValidValue)
		{
			out << "X" << tmpline.GcommandLine.X.value << " ";
		}
		if (tmpline.GcommandLine.Y.isValidValue)
		{
			out << "Y" << tmpline.GcommandLine.Y.value << " ";
		}
		if (tmpline.GcommandLine.Z.isValidValue)
		{
			out << "Z" << tmpline.GcommandLine.Z.value << " ";
		}
		if (tmpline.GcommandLine.E.isValidValue)
		{
			//��E����С�����5λ
			out.precision(5);
			out.setf(ios::fixed);
			out << "E" << tmpline.GcommandLine.E.value;
			//���������ı���С�����3λ
			out.precision(3);
		}
		out << endl;
	}
}

void Gcode::updateGcode()
{
	vector<LayerInfo> newMainBody;

	for (int i = 0; i < mainBody.size(); i++)
	{
		LayerInfo newTmpLayer;
		newTmpLayer.layer = mainBody.at(i).layer;
		newTmpLayer.lineContainer = mainBody.at(i).lineContainer;
		newTmpLayer.skirtContainer = mainBody.at(i).skirtContainer;

		for (int j = 0; j < mainBody.at(i).contourContainer.size(); j++)
		{
			contour newTmpContour;
			//��һ��contour�г�wallouter������������ֱ���
			newTmpContour.contourIndex = mainBody.at(i).contourContainer.at(j).contourIndex;
			newTmpContour.typeSequence = mainBody.at(i).contourContainer.at(j).typeSequence;
			newTmpContour.fillContainer = mainBody.at(i).contourContainer.at(j).fillContainer;
			newTmpContour.skinContainer = mainBody.at(i).contourContainer.at(j).skinContainer;
			newTmpContour.supportContainer = mainBody.at(i).contourContainer.at(j).supportContainer;
			newTmpContour.wallInnerContainer = mainBody.at(i).contourContainer.at(j).wallInnerContainer;

			//ֻ����wallouter����
			for (int k = 0; k < mainBody.at(i).contourContainer.at(j).wallOuterContainer.size(); k++)
			{
				eachLine tmpline = mainBody.at(i).contourContainer.at(j).wallOuterContainer.at(k);
				bool isDelete = false;
				//ֻ��G1ָ������ǲ��ֽ��в���
				if (tmpline.isGcommand
					&& tmpline.GcommandLine.Gcommand == "G1"
					&& tmpline.GcommandLine.X.isValidValue
					&& tmpline.GcommandLine.Y.isValidValue
					&& tmpline.GcommandLine.E.isValidValue)
				{
					//������sample point�Ĳ�
					if (i < samplePoints.size())
					{
						//�����sample point����û������һ�е�
						for (int index = 0; index < samplePoints[i][j].size(); index++)
						{
							//�ڵ�ǰ������sample point
							if (samplePoints[i][j][index].contourIndex == j
								&& samplePoints[i][j][index].indexStart == k)
							{
								//����ǰ�е����Կ���������ӵ���
								eachLine addLine;
								addLine.codetype = tmpline.codetype;
								addLine.isGcommand = tmpline.isGcommand;
								addLine.GcommandLine.Gcommand = tmpline.GcommandLine.Gcommand;
								addLine.GcommandLine.feedRate = tmpline.GcommandLine.feedRate;
								addLine.GcommandLine.Z = tmpline.GcommandLine.Z;

								//��sample point�������ֵ���Ƶ��������
								addLine.GcommandLine.X.value = samplePoints[i][j][index].x;
								addLine.GcommandLine.Y.value = samplePoints[i][j][index].y;
								addLine.GcommandLine.E.value = samplePoints[i][j][index].e;
								addLine.GcommandLine.E.isValidValue = true;
								addLine.GcommandLine.X.isValidValue = true;
								addLine.GcommandLine.Y.isValidValue = true;

								newTmpContour.wallOuterContainer.push_back(addLine);
								isDelete = true;
							}

						}
					}
					else//����û��sample point�Ĳ㣬�����д���ֱ�ӱ�����µ�������
					{
						newTmpContour.wallOuterContainer.push_back(mainBody.at(i).contourContainer.at(j).wallOuterContainer.at(k));
					}

				}
				else//���ڲ�������������G1������ָ������д���ֱ�ӱ�����µ�������
				{
					newTmpContour.wallOuterContainer.push_back(mainBody.at(i).contourContainer.at(j).wallOuterContainer.at(k));
				}
			}
			//һ���ڵ�һ�������������
			newTmpLayer.contourContainer.push_back(newTmpContour);
			//����vector
			newTmpContour.wallOuterContainer.clear();
		}
		//һ�㴦�����
		newMainBody.push_back(newTmpLayer);

	}
	//����������ɵ���Ϣ���ԭ����
	mainBody = newMainBody;
}

point2D Gcode::findActuralStartPoint(LayerInfo tmpLayer, int contourIndex)
{
	point2D acturalStartPoint;
	contour tmpContour = tmpLayer.contourContainer.at(contourIndex);
	eachLine tmpLine;
	int validBaseG1;
	int validEG1;
	int validXYG0;
	bool isG1Find = false;
	bool isG0Found = false;

	//1. �ҵ���wallouter��ʶ���µ�һ��X,Y,Eֵ����Ч��G1ָ��
	for (int i = 0; i < tmpContour.wallOuterContainer.size(); i++)
	{
		tmpLine = tmpContour.wallOuterContainer.at(i);
		if (tmpLine.isGcommand
			&& tmpLine.GcommandLine.X.isValidValue == true
			&& tmpLine.GcommandLine.Y.isValidValue == true
			&& tmpLine.GcommandLine.E.isValidValue == true)
		{
			//�ҵ����¼�õ��λ��
			validBaseG1 = i;
			break;
		}
	}

	//2. �Ӹ�ָ����ǰ���ҵ����������E��Ч��G1ָ��,��һ����Ϊ��ȷ��E
	//2.1 ��һ���������wallouter�п����ҵ�
	for (int i = validBaseG1 - 1; i != -1; i--)//�Ӹõ��ǰһ���㿪ʼ��
	{
		tmpLine = tmpContour.wallOuterContainer.at(i);
		if (tmpLine.isGcommand
			&& tmpLine.GcommandLine.E.isValidValue == true)
		{
			isG1Find = true;
			acturalStartPoint.e = tmpLine.GcommandLine.E.value;
			break;
		}
	}
	//2.2 �ڶ����������wallouter���Ҳ���
	//��Ҫ�ڵ�ǰ��������
	if (isG1Find == false
		&& tmpContour.typeSequence.at(0) != WALL_OUTER)
	{
		//�����ҵ�WALL_OUTER��λ��
		for (int i = 0; i < tmpContour.typeSequence.size(); i++)
		{
			if (tmpContour.typeSequence.at(i) == WALL_OUTER)
				validEG1 = i - 1;
		}
		if (tmpContour.typeSequence.at(validEG1) == WALL_INNER)
		{
			for (int j = tmpContour.wallInnerContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.wallInnerContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.E.isValidValue == true)
				{
					isG1Find = true;
					acturalStartPoint.e = tmpLine.GcommandLine.E.value;
					break;
				}
			}
		}
		else if (tmpContour.typeSequence.at(validEG1) == FILL)
		{
			for (int j = tmpContour.fillContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.fillContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.E.isValidValue == true)
				{
					isG1Find = true;
					acturalStartPoint.e = tmpLine.GcommandLine.E.value;
					break;
				}
			}
		}
		else if (tmpContour.typeSequence.at(validEG1) == SKIN)
		{
			for (int j = tmpContour.skinContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.skinContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.E.isValidValue == true)
				{
					isG1Find = true;
					acturalStartPoint.e = tmpLine.GcommandLine.E.value;
					break;
				}
			}
		}
		else if (tmpContour.typeSequence.at(validEG1) == SUPPORT)
		{
			for (int j = tmpContour.supportContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.supportContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.E.isValidValue == true)
				{
					isG1Find = true;
					acturalStartPoint.e = tmpLine.GcommandLine.E.value;
					break;
				}
			}
		}
	}
	//2.3 ������������ڵ�ǰ�������Ҳ���
	//����Ҫ�ڵ�ǰ����ǰ��
	else if (isG1Find == false
		&& tmpContour.typeSequence.at(0) == WALL_OUTER)
	{
		//�ڵ�ǰ���lineContainer�в���
		int tmpIndx = tmpLayer.lineContainer.size() - 1;
		for (int i = tmpIndx; i > 0; i--)
		{
			tmpLine = tmpLayer.lineContainer.at(i);
			if (tmpLine.isGcommand
				&& tmpLine.GcommandLine.E.isValidValue == true)
			{
				isG1Find = true;
				acturalStartPoint.e = tmpLine.GcommandLine.E.value;
				break;
			}
		}
	}

	//3. �Ӹ�ָ����ǰ���ҵ����������X��Y��Ч��G0ָ���һ����Ϊ��ȷ��X��Y
	//3.1 ��һ���������wallouter�п����ҵ�
	for (int i = validBaseG1 - 1; i != -1; i--)//�Ӹõ��ǰһ���㿪ʼ��
	{
		tmpLine = tmpContour.wallOuterContainer.at(i);
		if (tmpLine.isGcommand
			&& tmpLine.GcommandLine.Gcommand == "G0"
			&& tmpLine.GcommandLine.X.isValidValue == true
			&& tmpLine.GcommandLine.Y.isValidValue == true)
		{
			isG0Found = true;
			acturalStartPoint.x = tmpLine.GcommandLine.X.value;
			acturalStartPoint.y = tmpLine.GcommandLine.Y.value;
			break;
		}
	}
	//3.2 �ڶ����������wallouter���Ҳ���
	//��Ҫ�ڵ�ǰ��������
	if (isG0Found == false
		&& tmpContour.typeSequence.at(0) != WALL_OUTER)
	{
		//�����ҵ�WALL_OUTER��λ��
		for (int i = 0; i < tmpContour.typeSequence.size(); i++)
		{
			if (tmpContour.typeSequence.at(i) == WALL_OUTER)
				validXYG0 = i - 1;
		}
		if (tmpContour.typeSequence.at(validXYG0) == WALL_INNER)
		{
			for (int j = tmpContour.wallInnerContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.wallInnerContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.Gcommand == "G0"
					&& tmpLine.GcommandLine.X.isValidValue == true
					&& tmpLine.GcommandLine.Y.isValidValue == true)
				{
					isG0Found = true;
					acturalStartPoint.x = tmpLine.GcommandLine.X.value;
					acturalStartPoint.y = tmpLine.GcommandLine.Y.value;
					break;
				}
			}
		}
		else if (tmpContour.typeSequence.at(validXYG0) == FILL)
		{
			for (int j = tmpContour.fillContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.fillContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.Gcommand == "G0"
					&& tmpLine.GcommandLine.X.isValidValue == true
					&& tmpLine.GcommandLine.Y.isValidValue == true)
				{
					isG0Found = true;
					acturalStartPoint.x = tmpLine.GcommandLine.X.value;
					acturalStartPoint.y = tmpLine.GcommandLine.Y.value;
					break;
				}
			}
		}
		else if (tmpContour.typeSequence.at(validXYG0) == SKIN)
		{
			for (int j = tmpContour.skinContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.skinContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.Gcommand == "G0"
					&& tmpLine.GcommandLine.X.isValidValue == true
					&& tmpLine.GcommandLine.Y.isValidValue == true)
				{
					isG0Found = true;
					acturalStartPoint.x = tmpLine.GcommandLine.X.value;
					acturalStartPoint.y = tmpLine.GcommandLine.Y.value;
					break;
				}
			}
		}
		else if (tmpContour.typeSequence.at(validXYG0) == SUPPORT)
		{
			for (int j = tmpContour.supportContainer.size() - 1; j >= 0; j--)
			{
				tmpLine = tmpContour.supportContainer.at(j);
				if (tmpLine.isGcommand
					&& tmpLine.GcommandLine.Gcommand == "G0"
					&& tmpLine.GcommandLine.X.isValidValue == true
					&& tmpLine.GcommandLine.Y.isValidValue == true)
				{
					isG0Found = true;
					acturalStartPoint.x = tmpLine.GcommandLine.X.value;
					acturalStartPoint.y = tmpLine.GcommandLine.Y.value;
					break;
				}
			}
		}
	}

	//3.3 ������������ڵ�ǰ�������Ҳ���
	//����Ҫ�ڵ�ǰ����ǰ��
	else if (isG1Find == false
		&& tmpContour.typeSequence.at(0) == WALL_OUTER)
	{
		//�ڵ�ǰ���lineContainer�в���
		int tmpIndx = tmpLayer.lineContainer.size() - 1;
		for (int i = tmpIndx; i > 0; i--)
		{
			tmpLine = tmpLayer.lineContainer.at(i);
			if (tmpLine.isGcommand
				&& tmpLine.GcommandLine.Gcommand == "G0"
				&& tmpLine.GcommandLine.X.isValidValue == true
				&& tmpLine.GcommandLine.Y.isValidValue == true)
			{
				isG1Find = true;
				acturalStartPoint.x = tmpLine.GcommandLine.X.value;
				acturalStartPoint.y = tmpLine.GcommandLine.Y.value;
				break;
			}
		}
	}

	return acturalStartPoint;
}

point2D Gcode::findActuralEndPoint(LayerInfo tmpLayer, int contourIndex)
{
	point2D acturalEndPoint;
	int tmpIndexEnd = tmpLayer.contourContainer.at(contourIndex).wallOuterContainer.size() - 1;
	for (tmpIndexEnd; tmpIndexEnd > 0; tmpIndexEnd--)
	{
		eachLine tmpline = tmpLayer.contourContainer.at(contourIndex).wallOuterContainer.at(tmpIndexEnd);
		if (tmpline.isGcommand
			&& tmpline.GcommandLine.Gcommand == "G1"
			&& tmpline.codetype == WALL_OUTER
			&& tmpline.GcommandLine.X.isValidValue
			&& tmpline.GcommandLine.Y.isValidValue
			&& tmpline.GcommandLine.E.isValidValue)
		{
			acturalEndPoint.e = tmpline.GcommandLine.E.value;
			break;
		}
	}

	return acturalEndPoint;
}

