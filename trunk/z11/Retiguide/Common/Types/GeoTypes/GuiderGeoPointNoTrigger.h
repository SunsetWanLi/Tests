#pragma once 
#include "GuiderGeoPoint.h"
#include "CommonBase/MGWString.h"
#include <vector>

using namespace std;

namespace GIS_Embedded
{
	class GuiderGeoPointNoTrigger: public GuiderGeoPoint
	{
	
	protected:
		vector<z11::MGWString> m_names;
		vector<z11::MGWString> m_descriptions;
		z11::MGWString m_key;
		bool m_bHasName;
		bool m_bHasDescription;
	public:
		virtual bool hasTriggerArea() const { return false; }
	public:
		GuiderGeoPointNoTrigger():m_bHasDescription(false),m_bHasName(false)
		{}
		void addName(const z11::MGWString &name);
		void addDescription(const z11::MGWString& description);
		const z11::MGWString getName() const;
		const z11::MGWString getDescription() const;
		void setKey(const z11::MGWString &key);

	};
	
}