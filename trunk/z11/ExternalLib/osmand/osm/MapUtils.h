#pragma once
class MapUtils
{
public:
	MapUtils(void);
	~MapUtils(void);
public :
		/**
	 * @param diff align difference between 2 angles ]-180, 180] 
	 * @return 
	 */
	static double degreesDiff(double a1, double a2);	
	/**
	 * @param diff align difference between 2 angles ]-PI, PI] 
	 * @return 
	 */
	static double alignAngleDifference(double diff);
};

