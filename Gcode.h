#pragma once
#ifndef _GCODE_
#define _GCODE_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <iomanip>
#include <regex>

/**************************************************
������Ҫʵ�ֵĹ�����Gcode���ݽṹ�����룬���
**************************************************/
#define PI 3.14159265

using namespace std;

struct eachValue {
	bool isValidValue=false;
	double value;
};

//the basic structure of command line
//Gxx Fxx Xxx Yxx Exx
struct GCommand {
	string Gcommand="";
	int feedRate=-1;
	eachValue X;
	eachValue Y;
	eachValue Z;
	eachValue E;
};

enum codeType
{
	SKIRT,
	FILL,
	WALL_INNER,
	WALL_OUTER,
	SKIN,
	SUPPORT,
	UNDEFINED,
};

struct eachLine {
	bool isGcommand;
	codeType codetype;
	string otherCommand;//�����G����ָ��
	GCommand GcommandLine;//����G����ָ��
};

//���ʱ����contour index��˳��
//����contour�ṹ����Ϊ�˱�����һ���ڰ���������
struct contour {
	int contourIndex;
	vector<eachLine> wallOuterContainer;
	vector<eachLine> wallInnerContainer;
	vector<eachLine> skinContainer;
	vector<eachLine> fillContainer;
	vector<eachLine> supportContainer;
	vector<codeType> typeSequence;//һ�������ڲ�ͬ���͵����˳��
};

struct LayerInfo {
	int layer;
	vector<eachLine> lineContainer;//�����������������Ϣ
	vector<eachLine> skirtContainer;
	vector<contour> contourContainer;
	
};

//��point2D�����ԭGcode���ó��ĵ�
struct point2D
{
	float x;
	float y;
	double e;
	int index;//��ԭgcode�е�λ��
};

//��samplePoint2D��������sample point
struct samplePoint2D {
	float x=0;
	float y=0;
	double e=0;
	int indexStart=0;
	int indexEnd=0;
	int contourIndex;//���ڼ�¼������λ���ĸ�contour��
	int parentContourIndex;//���ڼ�¼�ò������������ĸ�������
	int parentPointIndex;//���ڼ�¼�ò������Ӧ�ڸ������ϵ��ĸ���
	float ContourWaveLength;//��¼�ò������Ӧ��wavelength

	//sort based on the value of e
	bool operator< (const samplePoint2D& other)
	{
		return e < other.e;
	}
};

class Gcode
{
public:
	Gcode();
	~Gcode();

	bool readInCura15Gcode(string fileName);
	bool readInCuraGcode(string fileName);

	bool isNewContour(const vector<string> &inputGcode, int index, string inputbuffer);
	void parseCommandLine(string inputbuffer,eachLine &tmpLine);
	void clearContourContainer(contour &tmpContour);

	bool outputGcode(string fileName);
	void outputLine(eachLine tmpline,ofstream &out);

	void updateGcode();

	point2D findActuralStartPoint(LayerInfo tmpLayer, int contourIndex);
	point2D findActuralEndPoint(LayerInfo tmpLayer, int contourIndex);

	//the structure of Gcode file 
	// start-Gcode
	// layer by layer command
	// end-Gcode
	vector<string> startGcode;
	vector<LayerInfo> mainBody;
	vector<string> endGcode;

	//Assistant information
	vector<vector<vector<samplePoint2D>>> samplePoints;
	
};
#endif // !_GCODE_
