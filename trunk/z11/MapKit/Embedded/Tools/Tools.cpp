#include "Tools.h"
#include "string.h"

using namespace Embedded;

IndexOutOfBoundsException::IndexOutOfBoundsException(uint_32 i)
{
	std::ostringstream s;
	s << "Invalid index " << i;
	m_error = s.str();
}

std::string IndexOutOfBoundsException::what()
{
	return "IndexOutOfBoundsException: " + m_error;
}

IllegalArgumentException::IllegalArgumentException(std::string s) : m_error(s)
{
}

std::string IllegalArgumentException::what()
{
	return "IllegalArgumentException: " + m_error;
}

IllegalStateException::IllegalStateException(std::string s) : m_error(s)
{
}

std::string IllegalStateException::what()
{
	return "IllegalStateException: " + m_error + "\nPlease contact "/* + PACKAGE_BUGREPORT*/;
}

EndOfStreamException::EndOfStreamException(std::string s) : m_error(s)
{
}

std::string EndOfStreamException::what()
{
	return "EndOfStreamException: " + m_error;
}

ResourceLockedException::ResourceLockedException(std::string s) : m_error(s)
{
}

std::string ResourceLockedException::what()
{
	return "ResourceLockedException: " + m_error;
}

InvalidPageException::InvalidPageException(uint_32 id)
{
	std::ostringstream s;
	s << "Unknown page id " << id;
	m_error = s.str();
}

std::string InvalidPageException::what()
{
	return "InvalidPageException: " + m_error;
}

NotSupportedException::NotSupportedException(std::string s) : m_error(s)
{
}

std::string NotSupportedException::what()
{
	return "NotSupportedException: " + m_error;
}

Variant::Variant() : m_varType(VT_EMPTY)
{
}

PropertySet::PropertySet(const uint_8* data)
{
	loadFromByteArray(data);
}

uint_32 PropertySet::getByteArraySize() const
{
	uint_32 size = sizeof(uint_32);
	std::map<std::string, Variant>::const_iterator it;

	for (it = m_propertySet.begin(); it != m_propertySet.end(); ++it)
	{
		switch ((*it).second.m_varType)
		{
		case VT_SHORT:
			size += sizeof(int_16);
			break;
		case VT_LONG:
			size += sizeof(int_32);
			break;
		case VT_LONGLONG:
			size += sizeof(int_64);
			break;
		case VT_BYTE:
			size += sizeof(uint_8);
			break;
		case VT_FLOAT:
			size += sizeof(float);
			break;
		case VT_DOUBLE:
			size += sizeof(double);
			break;
		case VT_CHAR:
			size += sizeof(char);
			break;
		case VT_USHORT:
			size += sizeof(uint_16);
			break;
		case VT_ULONG:
			size += sizeof(uint_32);
			break;
		case VT_ULONGLONG:
			size += sizeof(uint_64);
			break;
		case VT_BOOL:
			size += sizeof(uint_8);
			break;
		default:
			throw NotSupportedException(
				"PropertySet::getSize: Unknown type."
				);
		}
		size += (*it).first.size() + 1 + sizeof(VariantType);
	}

	return size;
}


void PropertySet::loadFromByteArray(const uint_8* ptr)
{
	m_propertySet.clear();

	uint_32 numberOfProperties;
	memcpy(&numberOfProperties, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	Variant v;

	for (uint_32 cIndex = 0; cIndex < numberOfProperties; ++cIndex)
	{
		std::string s(reinterpret_cast<const char*>(ptr));
		ptr += s.size() + 1;
		memcpy(&(v.m_varType), ptr, sizeof(VariantType));
		ptr += sizeof(VariantType);

		switch (v.m_varType)
		{
		case VT_SHORT:
			int_16 s;
			memcpy(&s, ptr, sizeof(int_16));
			ptr += sizeof(int_16);
			v.m_val.iVal = s;
			break;
		case VT_LONG:
			int_32 l;
			memcpy(&l, ptr, sizeof(int_32));
			ptr += sizeof(int_32);
			v.m_val.lVal = l;
			break;
		case VT_LONGLONG:
			int_64 ll;
			memcpy(&ll, ptr, sizeof(int_64));
			ptr += sizeof(int_64);
			v.m_val.llVal = ll;
			break;
		case VT_BYTE:
			uint_8 b;
			memcpy(&b, ptr, sizeof(uint_8));
			ptr += sizeof(uint_8);
			v.m_val.bVal = b;
			break;
		case VT_FLOAT:
			float f;
			memcpy(&f, ptr, sizeof(float));
			ptr += sizeof(float);
			v.m_val.fltVal = f;
			break;
		case VT_DOUBLE:
			double d;
			memcpy(&d, ptr, sizeof(double));
			ptr += sizeof(double);
			v.m_val.dblVal = d;
			break;
		case VT_CHAR:
			char c;
			memcpy(&c, ptr, sizeof(char));
			ptr += sizeof(char);
			v.m_val.cVal = c;
			break;
		case VT_USHORT:
			uint_16 us;
			memcpy(&us, ptr, sizeof(uint_16));
			ptr += sizeof(uint_16);
			v.m_val.uiVal = us;
			break;
		case VT_ULONG:
			uint_32 ul;
			memcpy(&ul, ptr, sizeof(uint_32));
			ptr += sizeof(uint_32);
			v.m_val.ulVal = ul;
			break;
		case VT_ULONGLONG:
			uint_64 ull;
			memcpy(&ull, ptr, sizeof(uint_64));
			ptr += sizeof(uint_64);
			v.m_val.ullVal = ull;
			break;
		case VT_BOOL:
			uint_8 bl;
			memcpy(&bl, ptr, sizeof(uint_8));
			ptr += sizeof(uint_8);
			v.m_val.blVal = static_cast<bool>(bl);
			break;
		default:
			throw IllegalStateException(
				"PropertySet::PropertySet: Deserialization problem."
				);
		}

		m_propertySet.insert(std::pair<std::string, Variant>(s, v));
	}
}

void PropertySet::storeToByteArray(uint_8** data, uint_32& length) const
{
	length = getByteArraySize();
	*data = new uint_8[length];
	uint_8* ptr = *data;

	uint_32 numberOfProperties = m_propertySet.size();
	memcpy(ptr, &numberOfProperties, sizeof(uint_32));
	ptr += sizeof(uint_32);

	std::map<std::string, Variant>::const_iterator it;

	for (it = m_propertySet.begin(); it != m_propertySet.end(); ++it)
	{
		uint_32 strSize = (*it).first.size();
		memcpy(ptr, (*it).first.c_str(), strSize);
		ptr += strSize;
		*ptr = 0;
		++ptr;

		memcpy(ptr, &((*it).second.m_varType), sizeof(VariantType));
		ptr += sizeof(VariantType);

		switch ((*it).second.m_varType)
		{
		case VT_SHORT:
			memcpy(ptr, &((*it).second.m_val.iVal), sizeof(int_16));
			ptr += sizeof(int_16);
			break;
		case VT_LONG:
			memcpy(ptr, &((*it).second.m_val.lVal), sizeof(int_32));
			ptr += sizeof(int_32);
			break;
		case VT_LONGLONG:
			memcpy(ptr, &((*it).second.m_val.llVal), sizeof(int_64));
			ptr += sizeof(int_64);
			break;
		case VT_BYTE:
			memcpy(ptr, &((*it).second.m_val.bVal), sizeof(uint_8));
			ptr += sizeof(uint_8);
			break;
		case VT_FLOAT:
			memcpy(ptr, &((*it).second.m_val.fltVal), sizeof(float));
			ptr += sizeof(float);
			break;
		case VT_DOUBLE:
			memcpy(ptr, &((*it).second.m_val.dblVal), sizeof(double));
			ptr += sizeof(double);
			break;
		case VT_CHAR:
			memcpy(ptr, &((*it).second.m_val.cVal), sizeof(char));
			ptr += sizeof(char);
			break;
		case VT_USHORT:
			memcpy(ptr, &((*it).second.m_val.uiVal), sizeof(uint_16));
			ptr += sizeof(uint_16);
			break;
		case VT_ULONG:
			memcpy(ptr, &((*it).second.m_val.ulVal), sizeof(uint_32));
			ptr += sizeof(uint_32);
			break;
		case VT_ULONGLONG:
			memcpy(ptr, &((*it).second.m_val.ullVal), sizeof(uint_64));
			ptr += sizeof(uint_64);
			break;
		case VT_BOOL:
			uint_8 bl;
			bl = (*it).second.m_val.blVal;
			memcpy(ptr, &bl, sizeof(uint_8));
			ptr += sizeof(uint_8);
			break;
		default:
			throw NotSupportedException(
				"PropertySet::getData: Cannot serialize a variant of this type."
				);
		}
	}

	assert(ptr == (*data) + length);
}

Variant PropertySet::getProperty(const std::string& property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end())
		return (*it).second;
	else
		return Variant();
}

void PropertySet::setProperty(const std::string& property, Variant& v)
{
	m_propertySet.insert(std::pair<std::string, Variant>(property, v));
}

void PropertySet::removeProperty(const std::string& property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end())
		m_propertySet.erase(it);
}
