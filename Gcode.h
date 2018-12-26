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
该类主要实现的功能是Gcode数据结构，输入，输出
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
	string otherCommand;//保存非G代码指令
	GCommand GcommandLine;//保存G代码指令
};

//输出时按照contour index的顺序
//增加contour结构体是为了便于在一层内按轮廓处理
struct contour {
	int contourIndex;
	vector<eachLine> wallOuterContainer;
	vector<eachLine> wallInnerContainer;
	vector<eachLine> skinContainer;
	vector<eachLine> fillContainer;
	vector<eachLine> supportContainer;
	vector<codeType> typeSequence;//一个轮廓内不同类型的输出顺序
};

struct LayerInfo {
	int layer;
	vector<eachLine> lineContainer;//除轮廓以外的其他信息
	vector<eachLine> skirtContainer;
	vector<contour> contourContainer;
	
};

//用point2D保存从原Gcode中拿出的点
struct point2D
{
	float x;
	float y;
	double e;
	int index;//在原gcode中的位置
};

//用samplePoint2D保存插入的sample point
struct samplePoint2D {
	float x=0;
	float y=0;
	double e=0;
	int indexStart=0;
	int indexEnd=0;
	int contourIndex;//用于记录采样点位于哪个contour上
	int parentContourIndex;//用于记录该采样点来自于哪个父轮廓
	int parentPointIndex;//用于记录该采样点对应于父轮廓上的哪个点
	float ContourWaveLength;//记录该采样点对应的wavelength

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
