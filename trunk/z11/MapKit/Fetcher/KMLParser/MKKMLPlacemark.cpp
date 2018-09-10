#include "MKKMLPlacemark.h"
#include "CommonBase/CommonDef.h"
namespace z11
{

	MKKMLPlacemark::MKKMLPlacemark(void)
	{
		geometry=NULL;
		m_sound=NULL;
	}

	MKKMLPlacemark::~MKKMLPlacemark(void)
	{
		delete_and_set_null(geometry);
		delete_and_set_null(m_sound);
	}

}