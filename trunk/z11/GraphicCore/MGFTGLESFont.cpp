#include "MGFTGLESFont.h"
#include "../System/IO/MGBundle.h"
#include "../System/Shared/MGLog.h"
#include "../System/Shared/Utility.h"
#include "../ExternalLib/ftgles/FTGL/ftgles.h"
#include "../System/Device/MGDevice.h"
#include "../Platform/MGGL.h"
#include "MGFont.h"
#include <stdio.h>
#include "MGGraphicEngine.h"
#include "MGFTGLESFontContextManager.h"
#include "MGPersistenceImage.h"
#include "../Platform/MGFileUtils.h"
#include "../System/IO/MGFileManager.h"
#include "../CommonBase/MGString.h"
namespace z11
{

	MGMutex MGFTGLESFont::image_locker;
	std::map<MGFTGLESFontIndex,FTBBox> MGFTGLESFont::font2bbox;
	std::map<MGFTGLESFontIndex,MGImage *> MGFTGLESFont::font2texture;

	FTFont *MGFTGLESFont::m_font=NULL;
	FTCustomLayout *MGFTGLESFont::layouts=NULL;
	MGMutex MGFTGLESFont::layout_locker;

	MGFTGLESFont::~MGFTGLESFont(void)
	{
		MGFTGLESFontContextManager::getInstance()->pop(this);

	}

	void MGFTGLESFont::recoveryContext()
	{

		MGFTGLESFont::image_locker.lock();
		MGFTGLESFont::TextureIterator it;
		for(it=MGFTGLESFont::font2texture.begin();
			it!=MGFTGLESFont::font2texture.end();
			it++)
		{
			MGPersistenceImage *image=dynamic_cast<MGPersistenceImage *>(it->second);
			image->dirty();
			image->sendDataToContext();
		}
		MGFTGLESFont::image_locker.unlock();
	}
    
    void MGFTGLESFont::cleanCache()
    {
        MGFTGLESFont::image_locker.lock();
		MGFTGLESFont::TextureIterator it;
		for(it=MGFTGLESFont::font2texture.begin();
			it!=MGFTGLESFont::font2texture.end();
			++it)
		{
			delete_and_set_null(it->second);
		}
        MGFTGLESFont::font2texture.clear();
		MGFTGLESFont::image_locker.unlock();
        
        MGFTGLESFont::layout_locker.lock();
        MGFTGLESFont::font2bbox.clear();
        MGFTGLESFont::layout_locker.unlock();
    }

	void MGFTGLESFont::setFaceSize( size_t size )
	{
		m_size=size;
		assert(m_font!=NULL);
		//m_font->FaceSize(m_size,72);//only set on render
	}

	size_t MGFTGLESFont::getFaceSize()
	{
		return m_size;
	}

	MGFTGLESFont::MGFTGLESFont( const std::string &font_type )
	{
		init(font_type);
		MGFTGLESFontContextManager::getInstance()->push(this);
	}

	FTFont * MGFTGLESFont::getNativeFont()
	{
		return m_font;
	}

	FONT MGFTGLESFont::getNativeFont() const
	{
		MGLogD("MGFTGLESFont shouldn't be here");
		return NULL;
	}

	z11::MGSize MGFTGLESFont::getStringWidthAndHeight( const std::string& text,float width )
	{
        layout_locker.lock();
		layouts->GetFont()->FaceSize(m_size);
		
		float old_width=layouts->GetLineLength();
		layouts->SetLineLength(width);
		MGSize ret;
        {
            MGFTGLESFontIndex index;
            index.name=text;
            index.size=m_size;
            index.rect_width = width;
            FTBBox box;
            //MGFTGLESFont::image_locker.lock();
            MGFTGLESFont::MapIterator it=MGFTGLESFont::font2bbox.find(index);
            if (it==MGFTGLESFont::font2bbox.end())
            {
                AUTO_LOCK(&layout_locker);
                layouts->GetFont()->FaceSize(m_size);
                box=layouts->BBox(text.c_str(), -1, FTPoint());
                MGFTGLESFont::font2bbox[index]=box;
            }
            else
            {
                box=(it->second);
            }
            //MGFTGLESFont::image_locker.unlock();
            
            float height=box.Upper().Y()-box.Lower().Y();//’‚¿Ôµƒ2±∂÷ª «æ≠—È÷µ°≠°≠     //		
            float width=box.Upper().X()-box.Lower().X();	
            ret = MGSize(width,height);
        }
		layouts->SetLineLength(old_width);

        layout_locker.unlock();
		return ret;
	}
    
    z11::MGRect MGFTGLESFont::getStringRectForDrawingWidth( const std::string& text,float width )
	{
        MGRect ret;
		if (text.size() == 0)
		{
			return ret;
		}
        layout_locker.lock();
		layouts->GetFont()->FaceSize(m_size);
		
		float old_width=layouts->GetLineLength();
		layouts->SetLineLength(width);
        {
            MGFTGLESFontIndex index;
            index.name=text;
            index.size=m_size;
            index.rect_width = width;
            FTBBox box;
            //MGFTGLESFont::image_locker.lock();
            MGFTGLESFont::MapIterator it=MGFTGLESFont::font2bbox.find(index);
            if (it==MGFTGLESFont::font2bbox.end())
            {
                AUTO_LOCK(&layout_locker);
                layouts->GetFont()->FaceSize(m_size);
                box=layouts->BBox(text.c_str(), -1, FTPoint());
                MGFTGLESFont::font2bbox[index]=box;
            }
            else
            {
                box=(it->second);
            }
            //MGFTGLESFont::image_locker.unlock();
            
            float height=box.Upper().Y()-box.Lower().Y();//’‚¿Ôµƒ2±∂÷ª «æ≠—È÷µ°≠°≠     //
            float width=box.Upper().X()-box.Lower().X();
            ret = MGRectMake(0,0,width,height);
        }
		layouts->SetLineLength(old_width);
        
        layout_locker.unlock();
		return ret;
	}

	z11::MGSize MGFTGLESFont::getCharWidthAndHeight( wchar_t ch ) const
	{
		static MGSize size;
		return size;
	}

	void MGFTGLESFont::render( const string &_text, const z11::MGFont * _font, const MGRect& _rect,MGImage *cachedImage, MILineBreakMode mode, MITextAlignment text_alignment, RGBAColorRef blend )
	{
		MGFTGLESFont *_this=this;
		AUTO_LOCK(&layout_locker);
		if (m_font->FaceSize()!=m_size)
		{
			m_font->FaceSize(m_size,72);
		}
		

		MGFTGLESFontIndex index;
		index.name = _text;
		index.size = _font->getFontSize();
        index.rect_width = _rect.width;

		switch (mode) {
            case MILineBreakModeTailTruncation:
            {
                MGSize string_size = getStringWidthAndHeight(index.name,999999);
                if (string_size.width <= _rect.width)
                {
                    layouts->SetLineLength(999999);
                    break;
                }
                MGUTF8String utf8_str = index.name;
                MGSize omit_size=getStringWidthAndHeight("...",999999);
                uint_32 length = utf8_str.length();
                for (uint_32 location = 1; location <= length; ++location) {
                    MGUTF8String curr_utf8_str = utf8_str.substr(0, location);
                    string_size = getStringWidthAndHeight(curr_utf8_str.toSTLString(),999999);
                    if (string_size.width + omit_size.width > _rect.width) {
                        --location;
                        curr_utf8_str = utf8_str.substr(0, location);
                        index.name = curr_utf8_str.toSTLString() + "...";
                        break;
                    }
                }
                layouts->SetLineLength(999999);
                break;
            }
            case MILineBreakModeHeadTruncation:
            {
                MGSize string_size = getStringWidthAndHeight(index.name,999999);
                if (string_size.width <= _rect.width)
                {
                    layouts->SetLineLength(999999);
                    break;
                }
                MGUTF8String utf8_str = index.name;
                MGSize omit_size=getStringWidthAndHeight("...",999999);
                uint_32 length = utf8_str.length();
                for (uint_32 location = 1; location <= length; ++location) {
                    MGUTF8String curr_utf8_str = utf8_str.substr(length - location, location);
                    string_size = getStringWidthAndHeight(curr_utf8_str.toSTLString(),999999);
                    if (string_size.width + omit_size.width > _rect.width) {
                        --location;
                        curr_utf8_str = utf8_str.substr(length - location, location);
                        index.name = "..." + curr_utf8_str.toSTLString();
                        break;
                    }
                }
                layouts->SetLineLength(999999);
                break;
            }
            case MILineBreakModeMiddleTruncation:
            {
                MGSize string_size = getStringWidthAndHeight(index.name,999999);
                if (string_size.width <= _rect.width)
                {
                    layouts->SetLineLength(999999);
                    break;
                }
                MGUTF8String utf8_str = index.name;
                MGSize omit_size=getStringWidthAndHeight("...",999999);
                uint_32 length = utf8_str.length();
                for (uint_32 location = 1; location <= length; ++location) {
                    MGUTF8String before_utf8_str = utf8_str.substr(0, location);
                    MGSize before_size = getStringWidthAndHeight(before_utf8_str.toSTLString(),999999);
                    MGUTF8String after_utf8_str = utf8_str.substr(length - location, location);
                    MGSize after_size = getStringWidthAndHeight(after_utf8_str.toSTLString(),999999);
                    if (before_size.width + after_size.width + omit_size.width > _rect.width) {
                        --location;
                        before_utf8_str = utf8_str.substr(0, location);
                        after_utf8_str = utf8_str.substr(length - location, location);
                        index.name = before_utf8_str.toSTLString() + "..." + after_utf8_str.toSTLString();
                        break;
                    }
                }
                layouts->SetLineLength(999999);
                break;
            }
            case MILineBreakModeWordWrap:
            {
                layouts->SetLineLength(_rect.width);
                break;
            }
            case MILineBreakModeCharacterWrap:
            {
                layouts->SetLineLength(_rect.width);
                break;
            }
            case MILineBreakModeClip:
            {
                MGSize string_size = getStringWidthAndHeight(index.name,999999);
                if (string_size.width <= _rect.width)
                {
                    layouts->SetLineLength(999999);
                    break;
                }
                // Do the same as default.
            }
            default:
                // Do nothing but rendering.
                layouts->SetLineLength(999999);
                break;
        }

		MGFTGLESFont::image_locker.lock();
		FTBBox box;
		MGFTGLESFont::MapIterator it=MGFTGLESFont::font2bbox.find(index);
		if (it==MGFTGLESFont::font2bbox.end())
		{
			box=_this->layouts->BBox(index.name.c_str(), -1, FTPoint());	
			MGFTGLESFont::font2bbox[index]=box;
		}
		else
		{
			box=(it->second);
		}
	

		//MGLogD("box upper=%f,%f lower=%f,%f",box.Upper().X(),box.Upper().Y(),box.Lower().X(),box.Lower().Y());
		//MGLogD("rect x=%f,y=%f width=%f,height=%f",_rect.x,_rect.y,_rect.width,_rect.height);		
        float height=box.Upper().Y()-box.Lower().Y();
        height=MAXVALUE(height,_font->getFontSize());
		float width=box.Upper().X()-box.Lower().X();		
        float y=_rect.y;
		float x=_rect.x;		
		int offsetX=0;
		int offsetY=0;
		if (text_alignment&MITextAlignmentCenter)
		{
			offsetX=(_rect.width-width)/2;
		}
		if (text_alignment&MITextAlignmentRight)
		{
			offsetX=_rect.width-width;
		}
		if (text_alignment&MITextAlignmentMiddle)
		{
			offsetY=(_rect.height-height)/2;
		}
		if(text_alignment&MITextAlignmentBottom)
		{
			offsetY=_rect.height-height;
		}
		x+=offsetX;
		y+=offsetY;
		FTEfficientFont *efficient_font=dynamic_cast<FTEfficientFont *>(_this->getNativeFont());
		assert(efficient_font!=NULL);

		MGFTGLESFont::TextureIterator it2=MGFTGLESFont::font2texture.find(index);
		bool hasCached=true;
		bool needDraw=false;
		if (cachedImage==NULL)
		{
			if (it2==MGFTGLESFont::font2texture.end())
			{
				hasCached=false;
				int padding=MAXVALUE(_font->getFontSize()*0.15+0.5,3);//FTEfficientFontImpl->padding == 3
				cachedImage=MGPersistenceImage::fromBlank(8,width+padding,height+2*padding);
				//MGLogD("%s,%d,%d",index.name.c_str(),cachedImage->getWidth(),cachedImage->getHeight());
				MGFTGLESFont::font2texture[index]=cachedImage;
			}
			else
			{
				cachedImage=it2->second;
			}
			needDraw=true;
		}
		else
		{
			hasCached=false;
		}

		if (!hasCached)
		{
			efficient_font->setCanvas(cachedImage);
			string render_name=index.name;
			if (needDraw)
			{
				_this->layouts->Render(render_name.c_str(), -1, FTPoint(), FTGL::RENDER_FRONT);
			}
			else
			{
				_this->layouts->Render(render_name.c_str(), -1, FTPoint(x,y), FTGL::RENDER_FRONT);
			}
		}
		assert(cachedImage!=NULL);
		if (needDraw)
		{
			cachedImage->sendDataToContext();
			RGBAColorRef color_ref=_font->getColor();
			color_ref.m_alpha_component=color_ref.m_alpha_component*blend.m_alpha_component/255;
			MGGraphicEngine::sharedGraphicEngine()->drawImage(cachedImage,MGRectMake(x,y,cachedImage->getWidth(),cachedImage->getHeight()),color_ref);
        } else {
            cachedImage->sendDataToContext();
            RGBAColorRef color_ref=_font->getColor();
            color_ref.m_alpha_component=color_ref.m_alpha_component*blend.m_alpha_component/255;
            MGGraphicEngine::sharedGraphicEngine()->drawImage(cachedImage,MGRectMake(0,0,cachedImage->getWidth(),cachedImage->getHeight()),color_ref);
        }
        MGFTGLESFont::image_locker.unlock();
	}


	void MGFTGLESFont::init(const string &font_type)
	{
		m_font_type = font_type;
		if (m_font == NULL)
		{
			std::string font_path;
			MGFileManager* file_manager = MGFileManager::defaultManager();
            if (file_manager->fileExistsAtPath(MGFont::getDefaultFontPath()))
            {
                font_path = MGFont::getDefaultFontPath();
			}
			
			if (font_path.empty())
			{
#if MG_PLATFORM_ANDROID == MG_TARGET_PLATFORM
				font_path = "/system/fonts/DroidSansFallback.ttf";
        
                if (!file_manager->fileExistsAtPath(font_path))
                {
                    font_path = "/system/fonts/DFHEIA5A.ttf"; //android 5.0
                }
                if (!file_manager->fileExistsAtPath(font_path))
                {
                    font_path = "/system/fonts/NotoSansHans-Regular.otf"; //android 5.0
                }
#elif MG_PLATFORM_IOS == MG_TARGET_PLATFORM
				font_path = "/System/Library/Fonts/Cache/STHeiti-Light.ttc";
                m_font_type = "Heiti SC";
				if (!file_manager->fileExistsAtPath(font_path))
				{
					font_path = "/System/Library/Fonts/STHeiti Light.ttc"; //STHeitiSC
                    m_font_type = "Heiti SC";
				}
				if (!file_manager->fileExistsAtPath(font_path))
				{
					font_path = "/System/Library/Fonts/Cache/STHeitiTC-Light.ttc"; //STHeitiTC
                    m_font_type = "Heiti SC";
				}
                if(!file_manager->fileExistsAtPath(font_path))
                {
                    font_path = "/System/Library/Fonts/Core/STHeiti-Light.ttc";
                    m_font_type = "Heiti SC";
                }
                if(!file_manager->fileExistsAtPath(font_path))
                {
                    font_path = "/System/Library/Fonts/LanguageSupport/PingFang.ttc";
                    m_font_type = "PingFang SC";
                }
                
#elif MG_PLATFORM_WIN32 == MG_TARGET_PLATFORM
				font_path = "C:/WINDOWS/Fonts/STZHONGS.TTF"; //win 8
				if (!file_manager->fileExistsAtPath(font_path))
				{
					font_path = "C:/WINDOWS/Fonts/msyh.ttf"; //win 7
				}
				if (!file_manager->fileExistsAtPath(font_path))
				{
					font_path = "C:/WINDOWS/Fonts/simsun.ttc"; //win xp
				}
#else
				MGLogD("MGFTGLESFont:Unknown platform");
#endif
			}
			if (!file_manager->fileExistsAtPath(font_path))
			{
				MGLogD("MGFTGLESFont can't find %s",font_path.c_str());
			}
			m_font = new FTEfficientFont(font_path.c_str(), m_font_type.c_str());
			if (m_font->Error())
			{
				MGLogD("FTGLES font load failed ,please make sure the fonts path and name");
				delete_and_set_null(m_font);
			}
		}
		
		if (layouts == NULL)
		{
			layouts = new FTCustomLayout();
			layouts->SetLineSpacing(1.0f);
			layouts->SetAlignment(FTGL::ALIGN_LEFT);
			layouts->SetFont(m_font);
		}
	}

	vector<MGRect> MGFTGLESFont::BBoxVector( const std::string &text,float width ) const
	{
		vector<MGRect> ret;
		layout_locker.lock();
		layouts->GetFont()->FaceSize(m_size);

		float old_width=layouts->GetLineLength();
		layouts->SetLineLength(width);

		layouts->GetFont()->FaceSize(m_size);
		FTVector<FTBBox> bboxs;
		bboxs=layouts->BBoxVector(text.c_str(), -1, FTPoint());
		for (size_t i=0;i<bboxs.size();i++)
		{
			ret.push_back(
				MGRectMake(bboxs[i].Lower().X(),bboxs[i].Lower().Y(),
				bboxs[i].Upper().X()-bboxs[i].Lower().X(),
				bboxs[i].Upper().Y()-bboxs[i].Lower().Y()));
		}

		layouts->SetLineLength(old_width);

		layout_locker.unlock();
		return ret;

	}
}
