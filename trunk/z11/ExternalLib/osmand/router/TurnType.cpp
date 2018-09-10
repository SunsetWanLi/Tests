#include "TurnType.h"
#include <sstream>
 const std::string TurnType::C = "C"; // continue (go straight) //$NON-NLS-1$
 const std::string TurnType::TL = "TL"; // turn left //$NON-NLS-1$
 const std::string TurnType::TSLL = "TSLL"; // turn slightly left //$NON-NLS-1$
 const std::string TurnType::TSHL = "TSHL"; // turn sharply left //$NON-NLS-1$
 const std::string TurnType::TR = "TR"; // turn right //$NON-NLS-1$
 const std::string TurnType::TSLR = "TSLR"; // turn slightly right //$NON-NLS-1$
 const std::string TurnType::TSHR = "TSHR"; // turn sharply right //$NON-NLS-1$
 const std::string TurnType::KL = "KL"; // keep left //$NON-NLS-1$
 const std::string TurnType::KR = "KR"; // keep right//$NON-NLS-1$
 const std::string TurnType::TU = "TU"; // U-turn //$NON-NLS-1$
 const std::string TurnType::TRU = "TRU"; // Right U-turn //$NON-NLS-1$
 std::vector<std::string> TurnType::predefinedTypes; 

TurnType::TurnType( const std::string& value, int exitOut, bool leftSide )
{
	initDataMember();
	this->value = value;
	this->exitOut = exitOut;
	this->leftSide = leftSide;
}

TurnType::TurnType( std::string value )
{
	initDataMember();
	this->value = value;
}

TurnType::TurnType( void )
{
	initDataMember();
}


TurnType::~TurnType(void)
{
}

TurnType TurnType::valueOf( const std::string& s, bool leftSide )
{
	for (size_t i=0; i< predefinedTypes.size();i++) {
		if (predefinedTypes[i]==(s)) {
			if (leftSide && TU == predefinedTypes[i]) {
				return TurnType(TRU);
			}
			return TurnType(predefinedTypes[i]);
		}
	}
	if (!s.empty() ) { //$NON-NLS-1$
		int out=0;
		if(sscanf(s.c_str(),"EXIT%d",&out)==1)
		{
			return getExitTurn(out, 0, leftSide);

		}
	}
	return TurnType("");
}

TurnType TurnType::getExitTurn( int out, float angle, bool leftSide )
{
	TurnType r = TurnType("EXIT", out, leftSide); //$NON-NLS-1$
	r.turnAngle=(angle);
	return r;
}

float TurnType::getTurnAngle()
{
	return turnAngle;
}

bool TurnType::isLeftSide()
{
	return leftSide;
}

void TurnType::setTurnAngle( float turnAngle )
{
	this->turnAngle = turnAngle;
}

std::string TurnType::getValue()
{
	return value;
}

int TurnType::getExitOut()
{
	return exitOut;
}

bool TurnType::isRoundAbout()
{
	return value==("EXIT"); //$NON-NLS-1$
}

void TurnType::setLanes( const std::vector<int>& lanes )
{
	this->lanes = lanes;
}

std::vector<int>& TurnType::getLanes()
{
	return lanes;
}

bool TurnType::keepLeft()
{
	return value==(KL);
}

bool TurnType::keepRight()
{
	return value==(KR);
}

bool TurnType::isSkipToSpeak()
{
	return skipToSpeak;
}

void TurnType::setSkipToSpeak( bool skipToSpeak )
{
	this->skipToSpeak = skipToSpeak;
}

std::string TurnType::toString()
{
	std::stringstream ss;
	ss<<"Take " << getExitOut() << " exit";
	if(isRoundAbout()){
		return ss.str();
	} else if(value==(C)) {
		return "Go ahead";
	} else if(value==(TSLL)) {
		return "Turn slightly left";
	} else if(value==(TL)) {
		return "Turn left";
	} else if(value==(TSHL)) {
		return "Turn sharply left";
	} else if(value==(TSLR)) {
		return "Turn slightly right";
	} else if(value==(TR)) {
		return "Turn right";
	} else if(value==(TSHR)) {
		return "Turn sharply right";
	} else if(value==(TU)) {
		return "Make uturn";
	} else if(value==(TRU)) {
		return "Make uturn";
	} else if(value==(KL)) {
		return "Keep left";
	} else if(value==(KR)) {
		return "Keep right";
	}
	return "";
}

void TurnType::initDataMember()
{
	this->exitOut = 0;
	this->leftSide = false;
	std::string _predefinedTypes[]= { C, KL, KR, TL, TSLL, TSHL, TR, TSLR, TSHR, TU, TRU };
	predefinedTypes.insert(predefinedTypes.end(),_predefinedTypes,_predefinedTypes+sizeof(_predefinedTypes)/sizeof(_predefinedTypes[0]));
	turnAngle=0.0f;
	skipToSpeak=false;

}

bool TurnType::isNull()
{
	return value.empty();
}