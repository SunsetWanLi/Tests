#pragma once
#include <string>
#include <vector>
class TurnType
{
public:
	static const std::string C ;// continue (go straight) //$NON-NLS-1$
	static const std::string TL ; // turn left //$NON-NLS-1$
	static const std::string TSLL ; // turn slightly left //$NON-NLS-1$
	static const std::string TSHL ; // turn sharply left //$NON-NLS-1$
	static const std::string TR ; // turn right //$NON-NLS-1$
	static const std::string TSLR ; // turn slightly right //$NON-NLS-1$
	static const std::string TSHR ; // turn sharply right //$NON-NLS-1$
	static const std::string KL ; // keep left //$NON-NLS-1$
	static const std::string KR ; // keep right//$NON-NLS-1$
	static const std::string TU ; // U-turn //$NON-NLS-1$
	static const std::string TRU ; // Right U-turn //$NON-NLS-1$
	static std::vector<std::string> predefinedTypes; 

	static TurnType valueOf(const std::string& s, bool leftSide);

private:
	std::string value;
	int exitOut;
	bool leftSide;
	// calculated clockwise head rotation if previous direction to NORTH
	float turnAngle;
	bool skipToSpeak;
	std::vector<int> lanes;
	void initDataMember();
public:

	static TurnType getExitTurn(int out, float angle, bool leftSide);

	TurnType(const std::string& value, int exitOut, bool leftSide);

	// calculated Clockwise head rotation if previous direction to NORTH
	float getTurnAngle();

	bool isLeftSide();

	void setTurnAngle(float turnAngle);

	TurnType(std::string value);

	std::string getValue();

	int getExitOut();

	bool isRoundAbout();

	// lanes encoded as array of int 
	// last bit is 1, 0 (should we take this lane)
	// first bits 0 - left, 1 - straight, 2 - right
	void setLanes(const std::vector<int>& lanes);

	std::vector<int>& getLanes();

	bool keepLeft();

	bool keepRight();

	bool isSkipToSpeak();
	void setSkipToSpeak(bool skipToSpeak);


	std::string toString();
	bool isNull();
public:
	TurnType(void);
	~TurnType(void);
};

