#include "MGString.h"
#include "../System/Shared/MGLog.h"
namespace z11
{

	z11::MGString::MGString( const std::string &str ) :m_string(str)
	{

	}

	MGString & z11::MGString::operator=( const std::string &str )
	{
		m_string=str;
		return *this;
	}
    
    MGString z11::MGString::operator+( const MGString &str )
	{
        MGString ret;
        ret.m_string = m_string + str.m_string;
        return ret;
	}

	const std::string & z11::MGString::toSTLString() const
	{
		return m_string;
	}


	MGString * MGString::initWithSTLString( const std::string &str )
	{
		return new MGString(str);
	}

	bool MGString::isEqual( MGObject * rhs)
	{
		MGString *rightString=dynamic_cast<MGString *>(rhs);
		if (rightString == NULL)
		{
			MGLogD("May not support dynamic_cast");
			rightString = static_cast<MGString *>(rhs);
		}
		return rightString->toSTLString()==this->toSTLString();
	}
    
    MGUTF8String& z11::MGUTF8String::operator=( const std::string &str )
	{
		m_string=str;
		return *this;
	}
    
    MGUTF8String z11::MGUTF8String::operator+( const MGUTF8String &str )
    {
        MGUTF8String ret;
        ret.m_string = m_string + str.m_string;
        return ret;
    }

    uint_32 MGUTF8String::length() const
    {
        uint_32 ret = 0;
        string content = m_string;
        while (!content.empty())
        {
            uint_32 step = 1;
            char lastchar = 0;
            lastchar = content[0];
            
            if ((lastchar & 0xF8) == 0xF0) // |1111|0xxx| 4字节编码字符 //
            {
                step = 4;
            } else if ((lastchar & 0xF0) == 0xE0) // |1110|xxxx| 3字节编码字符 //
            {
                step = 3;
            } else if ((lastchar & 0xE0) == 0xC0) // |110x|xxxx| 2字节编码字符 //
            {
                step = 2;
            } else if ((lastchar & 0x80) == 0x00) // |0xxx|xxxx| 1字节编码字符 //
                step = 1;
            if (step > content.length()) {
                ret = 0;   // invalide UTF8 string //
                break;
            }
            content = content.substr(step,content.length());
            ++ret;
        }
        return ret;
    }
    
    uint_32 MGUTF8String::size() const
    {
        return length();
    }
    
    void MGUTF8String::clear()
    {
        m_string.clear();
    }
    
    bool MGUTF8String::empty() const
    {
        return m_string.empty();
    }
    
    MGUTF8String MGUTF8String::substr(const uint_32& b_location, const uint_32& f_length)
    {
        uint_32 size = length();
        uint_32 e_location = b_location + f_length;
        assert(b_location <= e_location);
        //assert(e_location <= size);
        if(e_location > size)
            e_location = size;

        if (b_location == 0 && e_location == size)
        {
            MGUTF8String ret;
            ret = m_string;
            return ret;
        }
        if (b_location == e_location) {
            MGUTF8String ret;
            ret.clear();
            return ret;
        }
        char lastchar = 0;
        uint_32 location = 0;
        size_t step = 0;
        size_t b_position = 0;
        size_t e_position = 0;
        while (step <= m_string.length())
        {
            if (location == b_location)
            {
                b_position = step;
            }
            if (location == e_location)
            {
                e_position = step;
                break;
            }
            lastchar = m_string[step];
            if ((lastchar & 0xF8) == 0xF0) // |1111|0xxx| 4字节编码字符 //
            {
                step += 4;
            } else if ((lastchar & 0xF0) == 0xE0) // |1110|xxxx| 3字节编码字符 //
            {
                step += 3;
            } else if ((lastchar & 0xE0) == 0xC0) // |110x|xxxx| 2字节编码字符 //
            {
                step += 2;
            } else if ((lastchar & 0x80) == 0x00) // |0xxx|xxxx| 1字节编码字符 //
                step += 1;
            if (step > m_string.length()) {
                // invalide UTF8 string //
                break;
            }
            ++location;
        }
        MGUTF8String ret;
        size_t f_size = e_position - b_position;
        ret = m_string.substr(b_position, f_size);
        return ret;
    }
    
    MGUTF8String MGUTF8String::substr(const uint_32& b_location)
    {
        uint_32 size = length();
        return substr(b_location, size - b_location);
    }
    
    MGUTF8String& MGUTF8String::replace(const uint_32& b_location, const uint_32& r_size, const MGUTF8String& text)
    {
        uint_32 size = length();
        assert(r_size >= 0);
        assert(b_location + r_size <= size);
        uint_32 e_location = b_location + r_size;
        char lastchar = 0;
        uint_32 location = 0;
        size_t step = 0;
        size_t b_position = 0;
        size_t e_position = 0;
        while (step <= m_string.length())
        {
            if (location == b_location)
            {
                b_position = step;
            }
            if (location == e_location)
            {
                e_position = step;
                break;
            }
            lastchar = m_string[step];
            if ((lastchar & 0xF8) == 0xF0) // |1111|0xxx| 4字节编码字符 //
            {
                step += 4;
            } else if ((lastchar & 0xF0) == 0xE0) // |1110|xxxx| 3字节编码字符 //
            {
                step += 3;
            } else if ((lastchar & 0xE0) == 0xC0) // |110x|xxxx| 2字节编码字符 //
            {
                step += 2;
            } else if ((lastchar & 0x80) == 0x00) // |0xxx|xxxx| 1字节编码字符 //
                step += 1;
                if (step > m_string.length()) {
                    // invalide UTF8 string //
                    break;
                }
            ++location;
        }
        assert(e_position >= b_position);
        m_string.replace(b_position, e_position - b_position, text.toSTLString());
        return *this;
    }
    
    void MGUTF8String::cleanSpecialChar(const char &special_char)
    {
        size_t it = m_string.find(special_char);
        while (it != string::npos) {
            m_string.erase(it);
            it = m_string.find(special_char);
        }
    }
}
