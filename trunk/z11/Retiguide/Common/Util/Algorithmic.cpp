
#include "Algorithmic.h"

namespace GIS_Embedded
{
	bool Algorithmic::lineCutOff(double x1, double y1, double x2, double y2,const Scope& select_scope)
	{
        if(select_scope.isContain(x1, y1))
            return true;
        if(select_scope.isContain(x2, y2))
            return true;
		if(x1 <= select_scope.getLeft() && x2 <= select_scope.getLeft())
            return false;
		if(x1 >= select_scope.getRight() && x2 >= select_scope.getRight())
            return false;
		if(y1 >= select_scope.getTop() && y2 >= select_scope.getTop())
            return false;
		if(y1 <= select_scope.getBottom() && y2 <= select_scope.getBottom())
            return false;
        double x = (x1 + x2) / 2;
        double y = (y1 + y2) / 2;
        return lineCutOff(x1, y1, x, y, select_scope) || lineCutOff(x2, y2, x, y, select_scope);
    }

	int Algorithmic::encode(int x, int y, UserScreenRect& screen_scope)    
	{
        int code = 0;
        if(x < screen_scope._left)
            code |= 0x1;
        else
        if(x > screen_scope._right)
            code |= 0x2;
        if(y < screen_scope._top)
            code |= 0x8;
        else
        if(y > screen_scope._bottom)
            code |= 0x4;
        return code;
    }

	unsigned char Algorithmic::encode(int x,int y,const z11::MGRect2& screen)
	{
        unsigned char code = 0;
		if(x < screen.left)
            code |= 0x1;
        else if(x > screen.right)
            code |= 0x2;

        if(y < screen.top)
            code |= 0x8;
        else if(y > screen.bottom)
            code |= 0x4;
        return code;
	}

	int Algorithmic::encode(double x,double y,const Scope &screen_scope)
	{
		int code = 0;
        if(x < screen_scope.m_left)
            code |= 0x1;
        else
        if(x > screen_scope.m_right)
            code |= 0x2;
        if(y > screen_scope.m_top)
            code |= 0x8;
        else
        if(y < screen_scope.m_bottom)
            code |= 0x4;
        return code;

	}

	void Algorithmic::lineCutOff(int x1, int y1, int x2, int y2, UserScreenRect& screen_scope,vector<ScreenPos2D>* screen_pos2d)    
	{
        int x = 0;
        int y = 0;
        int code1 = encode(x1, y1, screen_scope);
        for(int code2 = encode(x2, y2, screen_scope); code1 != 0 || code2 != 0;)
        {
            if((code1 & code2) != 0)
                return;
            int code = code1;
            if(code == 0)
                code = code2;
            if((0x1 & code) != 0)
            {
                x = screen_scope._left;
                y = y1 + ((y2 - y1) * (screen_scope._left - x1)) / (x2 - x1);
            } else
            if((0x2 & code) != 0)
            {
                x = screen_scope._right;
                y = y1 + ((y2 - y1) * (screen_scope._right - x1)) / (x2 - x1);
            } else
            if((0x8 & code) != 0)
            {
                y = screen_scope._top;
                x = x1 + ((x2 - x1) * (screen_scope._top - y1)) / (y2 - y1);
            } else
            if((0x4 & code) != 0)
            {
                y = screen_scope._bottom;
                x = x1 + ((x2 - x1) * (screen_scope._bottom - y1)) / (y2 - y1);
            }
            if(code == code1)
            {
                x1 = x;
                y1 = y;
                code1 = encode(x1, y1, screen_scope);
            } else
            {
                x2 = x;
                y2 = y;
                code2 = encode(x2, y2, screen_scope);
            }
        }
		screen_pos2d->push_back(ScreenPos2D(x1, y1));
		screen_pos2d->push_back(ScreenPos2D(x2, y2));

    }

	bool Algorithmic::lineCutOff(double x1,double y1,double x2,double y2,const Scope &screen_scope,vector<Pos2D> *screen_pos2d)
	{
		bool all_in_scope = true;
		double x = 0;
        double y = 0;
        int code1 = encode(x1, y1, screen_scope);
        for(int code2 = encode(x2, y2, screen_scope); code1 != 0 || code2 != 0;)
        {
			all_in_scope = false;
            if((code1 & code2) != 0)
                return all_in_scope;
            int code = code1; //code取在scope外面的点
            if(code == 0)
                code = code2;
            if((0x1 & code) != 0) //左
            {
                x = screen_scope.m_left;
                y = y1 + ((y2 - y1) * (screen_scope.m_left - x1)) / (x2 - x1);
            } else
            if((0x2 & code) != 0) //右
            {
                x = screen_scope.m_right;
                y = y1 + ((y2 - y1) * (screen_scope.m_right - x1)) / (x2 - x1);
            } else
            if((0x8 & code) != 0) //上
            {
                y = screen_scope.m_top;
                x = x1 + ((x2 - x1) * (screen_scope.m_top - y1)) / (y2 - y1);
            } else
            if((0x4 & code) != 0) //下
            {
                y = screen_scope.m_bottom;
                x = x1 + ((x2 - x1) * (screen_scope.m_bottom - y1)) / (y2 - y1);
            }
            if(code == code1)
            {
                x1 = x;
                y1 = y;
                code1 = encode(x1, y1, screen_scope);
            } else
            {
                x2 = x;
                y2 = y;
                code2 = encode(x2, y2, screen_scope);
            }
        }
		screen_pos2d->push_back(Pos2D(x1, y1));
		screen_pos2d->push_back(Pos2D(x2, y2));

		return all_in_scope;
	}

	void Algorithmic::polygonCutOff(int x1, int y1, int x2, int y2, UserScreenRect& screen_scope,vector<ScreenPos2D>* screen_pos2d)  
	{
        int x = 0;
        int y = 0;
        int old_x1 = x1;
        int old_x2 = x2;
        int old_y1 = y1;
        int old_y2 = y2;
        int code1 = encode(x1, y1, screen_scope);
        for(int code2 = encode(x2, y2, screen_scope); code1 != 0 || code2 != 0;)
        {
            int code;
            if((code1 & code2) != 0)
            {
                code1 = encode(old_x1, old_y1, screen_scope);
                code2 = encode(old_x2, old_y2, screen_scope);
                code = code1 ^ code2;
                switch(code)
                {
                case 9: // '\t'
					screen_pos2d->push_back(ScreenPos2D(screen_scope._left, screen_scope._top));
                    break;

                case 10: // '\n'
                    screen_pos2d->push_back(ScreenPos2D(screen_scope._right, screen_scope._top));
                    break;

                case 5: // '\005'
                    screen_pos2d->push_back(ScreenPos2D(screen_scope._left, screen_scope._bottom));
                    break;

                case 6: // '\006'
                    screen_pos2d->push_back(ScreenPos2D(screen_scope._right, screen_scope._bottom));
                    break;

                case 7: // '\007'
                case 8: // '\b'
                default:
                    ;
                }
				return;
            }
            code = code1;
            if(code == 0)
                code = code2;
            if((0x1 & code) != 0)//<left
            {
                x = screen_scope._left;
                y = y1 + ((y2 - y1) * (screen_scope._left - x1)) / (x2 - x1);
            } 
			else if((0x2 & code) != 0)//>right
            {
                x = screen_scope._right;
                y = y1 + ((y2 - y1) * (screen_scope._right - x1)) / (x2 - x1);
            } 
			else if((0x8 & code) != 0)//<top
            {
                y = screen_scope._top;
                x = x1 + ((x2 - x1) * (screen_scope._top - y1)) / (y2 - y1);
            } 
			else if((0x4 & code) != 0)//>bottom
            {
                y = screen_scope._bottom;
                x = x1 + ((x2 - x1) * (screen_scope._bottom - y1)) / (y2 - y1);
            }
            if(code == code1)
            {
                x1 = x;
                y1 = y;
                code1 = encode(x1, y1, screen_scope);
            } 
			else
            {
                x2 = x;
                y2 = y;
                code2 = encode(x2, y2, screen_scope);
            }
        }

		screen_pos2d->push_back(ScreenPos2D(x1, y1));
		screen_pos2d->push_back(ScreenPos2D(x2, y2));
    }

	int Algorithmic::encodeY(double value, double y)   
	{
        int ret = 0;
        if(value < y)
            ret |= 0x8;
        if(value > y)
            ret |= 0x4;
        return ret;
    }

	int Algorithmic::encodeX(double value, double x)  
	{
        int ret = 0;
        if(value < x)
            ret |= 0x1;
        if(value > x)
            ret |= 0x2;
        return ret;
    }

	int Algorithmic::lineCutByXline(Pos2D& pos1, Pos2D& pos2, double y,Pos2D *pos)  
	{
        int code0 = encodeY(pos1.d1, y);
        int code1 = encodeY(pos2.d1, y);
        int code = code1 & code0;
        if(code != 0)
		{
            return -1;
		}
		if((code0 | code1) == 12)
        {
            double x = pos1.d0 + ((y - pos1.d1) * (pos2.d0 - pos1.d0)) / (pos2.d1 - pos1.d1);
			pos->d0 = x;
			pos->d1 = y;
			return 0;//new Pos2D(x,y);
        }
		if(pos1.d1 == y){
			pos->d0 = pos1.d0;
			pos->d1 = pos1.d1;
			return 0;//new Pos2D(pos1.d0,pos1.d1);
		}
        if(pos2.d1 == y){
			pos->d0 = pos2.d0;
			pos->d1 = pos2.d1;
			return 0;//new Pos2D(pos2.d0,pos2.d1);
		}
		else 
			return -1;//NULL;

    }

	int Algorithmic::lineCutByYline(Pos2D& pos1, Pos2D& pos2, double x,Pos2D* pos)   
	{
        int code0 = encodeX(pos1.d0, x);
        int code1 = encodeX(pos2.d1, x);
        int code = code1 & code0;
        if(code != 0)
		{
            return -1;//NULL;
		}
        if((code0 | code1) == 3)
        {
            double y = pos1.d1 + ((x - pos1.d0) * (pos2.d1 - pos1.d1)) / (pos2.d0 - pos1.d0);
			pos->d0 = x;
			pos->d1 = y;
			return 0;//new Pos2D(x,y);
        }
        if(pos1.d0 == x){
			pos->d0 = pos1.d0;
			pos->d1 = pos1.d1;
			return 0;//new Pos2D(pos1.d0,pos1.d1);
		}
        if(pos2.d0 == x){
			pos->d0 = pos2.d0;
			pos->d1 = pos2.d1;
			return 0;//new Pos2D(pos2.d0,pos2.d1);
		}
		else
			return -1;//NULL; 
    }

	int Algorithmic::computeLineIntersect(Pos2D& p1, Pos2D& p2, Pos2D& p3, Pos2D& p4,Pos2D *pos)   
	{
        if((p1.d0 == p2.d0 && p1.d1 == p2.d1) || (p3.d1 == p4.d1 && p3.d0 == p4.d0))
            return -1;//NULL;
        if(p3.d0 == p4.d0)
        {
            Pos2D ret;
			lineCutByYline(p1, p2, p3.d0, &ret);
            if((ret.d1 - p3.d1) * (ret.d1 - p4.d1) <= 0.0)
			{
				pos->d0 = ret.d0;
				pos->d1 = ret.d1;
                return 0;//ret;
			}
        }
        if(p3.d1 == p4.d1)
        {
            Pos2D ret;
			lineCutByXline(p1, p2, p3.d1, &ret);
            if((ret.d0 - p3.d0) * (ret.d0 - p4.d0) <= 0.0)
			{
				pos->d0 = ret.d0;
				pos->d1 = ret.d1;
				return 0;
			}
        }
		return -1;//NULL;
    }

	int Algorithmic::transAngleToClock(double angleInDegree)
	{
		int rotate_angle = (int)angleInDegree;
		rotate_angle = rotate_angle>=0?rotate_angle:360+rotate_angle;
		int clock = rotate_angle/30+rotate_angle%30/15;
		return clock;
	}


	/////////////////////////////////////////////////////////////////////
	bool Algorithmic::lineCutOff(int x1, int y1, int x2, int y2,const Scope& select_scope)
	{
		if(select_scope.isContain(x1, y1))
			return true;
		if(select_scope.isContain(x2, y2))
			return true;
		if(x1 <= select_scope.getLeft() && x2 <= select_scope.getLeft())
			return false;
		if(x1 >= select_scope.getRight() && x2 >= select_scope.getRight())
			return false;
		if(y1 >= select_scope.getTop() && y2 >= select_scope.getTop())
			return false;
		if(y1 <= select_scope.getBottom() && y2 <= select_scope.getBottom())
			return false;
		int x = (x1 + x2) / 2;
		int y = (y1 + y2) / 2;
		return lineCutOff(x1, y1, x, y, select_scope) || lineCutOff(x2, y2, x, y, select_scope);
	}

	int Algorithmic::encode(int x,int y,const Scope &screen_scope)
	{
		int code = 0;
		if(x < screen_scope.m_left)
			code |= 0x1;
		else
			if(x > screen_scope.m_right)
				code |= 0x2;
		if(y > screen_scope.m_top)
			code |= 0x8;
		else
			if(y < screen_scope.m_bottom)
				code |= 0x4;
		return code;

	}

	bool Algorithmic::lineCutOff(int x1,int y1,int x2,int y2,const Scope &screen_scope,vector<Pos2DEx> *screen_pos2d)
	{
		bool all_in_scope = true;
		int x = 0;
		int y = 0;
		int code1 = encode(x1, y1, screen_scope);
		for(int code2 = encode(x2, y2, screen_scope); code1 != 0 || code2 != 0;)
		{
			all_in_scope = false;
			if((code1 & code2) != 0)
				return all_in_scope;
			int code = code1; //code取在scope外面的点
			if(code == 0)
				code = code2;
			if((0x1 & code) != 0) //左
			{
				x = screen_scope.m_left;
				y = y1 + ((y2 - y1) * (screen_scope.m_left - x1)) / (x2 - x1);
			}
			else if((0x2 & code) != 0) //右
			{
				x = screen_scope.m_right;
				y = y1 + ((y2 - y1) * (screen_scope.m_right - x1)) / (x2 - x1);
			} 
			else if((0x8 & code) != 0) //上
			{
				y = screen_scope.m_top;
				x = x1 + ((x2 - x1) * (screen_scope.m_top - y1)) / (y2 - y1);
			} 
			else if((0x4 & code) != 0) //下
			{
				y = screen_scope.m_bottom;
				x = x1 + ((x2 - x1) * (screen_scope.m_bottom - y1)) / (y2 - y1);
			}
			if(code == code1) //第一点在scope外面
			{
				x1 = x;
				y1 = y;
				code1 = encode(x1, y1, screen_scope);
			}
			else
			{
				x2 = x;
				y2 = y;
				code2 = encode(x2, y2, screen_scope);
			}
		}
		screen_pos2d->push_back(Pos2DEx(x1, y1));
		screen_pos2d->push_back(Pos2DEx(x2, y2));

		return all_in_scope;
	}

	int Algorithmic::encodeY(int value, int y)   
	{
		int ret = 0;
		if(value < y)
			ret |= 0x8;
		if(value > y)
			ret |= 0x4;
		return ret;
	}

	int Algorithmic::encodeX(int value, int x)  
	{
		int ret = 0;
		if(value < x)
			ret |= 0x1;
		if(value > x)
			ret |= 0x2;
		return ret;
	}

	int Algorithmic::lineCutByXline(Pos2DEx& pos1, Pos2DEx& pos2, int y,Pos2DEx *pos)  
	{
		int code0 = encodeY(pos1.d1, y);
		int code1 = encodeY(pos2.d1, y);
		int code = code1 & code0;
		if(code != 0)
		{
			return -1;
		}
		if((code0 | code1) == 12)
		{
			int x = pos1.d0 + ((y - pos1.d1) * (pos2.d0 - pos1.d0)) / (pos2.d1 - pos1.d1);
			pos->d0 = x;
			pos->d1 = y;
			return 0;//new Pos2DEx(x,y);
		}
		if(pos1.d1 == y){
			pos->d0 = pos1.d0;
			pos->d1 = pos1.d1;
			return 0;//new Pos2DEx(pos1.d0,pos1.d1);
		}
		if(pos2.d1 == y){
			pos->d0 = pos2.d0;
			pos->d1 = pos2.d1;
			return 0;//new Pos2DEx(pos2.d0,pos2.d1);
		}
		else 
			return -1;//NULL;

	}

	int Algorithmic::lineCutByYline(Pos2DEx& pos1, Pos2DEx& pos2, int x,Pos2DEx* pos)   
	{
		int code0 = encodeX(pos1.d0, x);
		int code1 = encodeX(pos2.d1, x);
		int code = code1 & code0;
		if(code != 0)
		{
			return -1;//NULL;
		}
		if((code0 | code1) == 3)
		{
			int y = pos1.d1 + ((x - pos1.d0) * (pos2.d1 - pos1.d1)) / (pos2.d0 - pos1.d0);
			pos->d0 = x;
			pos->d1 = y;
			return 0;//new Pos2DEx(x,y);
		}
		if(pos1.d0 == x){
			pos->d0 = pos1.d0;
			pos->d1 = pos1.d1;
			return 0;//new Pos2DEx(pos1.d0,pos1.d1);
		}
		if(pos2.d0 == x){
			pos->d0 = pos2.d0;
			pos->d1 = pos2.d1;
			return 0;//new Pos2DEx(pos2.d0,pos2.d1);
		}
		else
			return -1;//NULL; 
	}

	int Algorithmic::computeLineIntersect(Pos2DEx& p1, Pos2DEx& p2, Pos2DEx& p3, Pos2DEx& p4,Pos2DEx *pos)   
	{
		if((p1.d0 == p2.d0 && p1.d1 == p2.d1) || (p3.d1 == p4.d1 && p3.d0 == p4.d0))
			return -1;//NULL;
		if(p3.d0 == p4.d0)
		{
			Pos2DEx ret;
			lineCutByYline(p1, p2, p3.d0, &ret);
			if((ret.d1 - p3.d1) * (ret.d1 - p4.d1) <= 0.0)
			{
				pos->d0 = ret.d0;
				pos->d1 = ret.d1;
				return 0;//ret;
			}
		}
		if(p3.d1 == p4.d1)
		{
			Pos2DEx ret;
			lineCutByXline(p1, p2, p3.d1, &ret);
			if((ret.d0 - p3.d0) * (ret.d0 - p4.d0) <= 0.0)
			{
				pos->d0 = ret.d0;
				pos->d1 = ret.d1;
				return 0;
			}
		}
		return -1;//NULL;
	}

	bool Algorithmic::lineCutOffByRect(z11::MGPoint* ret1, z11::MGPoint* ret2, int x1, int y1, int x2, int y2, const z11::MGRect2& rect)
	{
        int x = 0;
        int y = 0;
		unsigned char code1 = encode(x1,y1,rect);
		unsigned char code2 = encode(x2,y2,rect);

		while(code1 != 0 || code2 != 0)
        {
            if((code1 & code2) != 0)
                return false;

            unsigned char code = code1==0?code2:code1;

            if((0x1 & code) != 0)
            {
                x = rect.left;
                y = y1 + ((y2 - y1) * (rect.left - x1)) / (x2 - x1);
            }
			else if((0x2 & code) != 0)
            {
                x = rect.right;
                y = y1 + ((y2 - y1) * (rect.right - x1)) / (x2 - x1);
            }
			else if((0x8 & code) != 0)
            {
                y = rect.top;
                x = x1 + ((x2 - x1) * (rect.top - y1)) / (y2 - y1);
            }
			else if((0x4 & code) != 0)
            {
                y = rect.bottom;
                x = x1 + ((x2 - x1) * (rect.bottom - y1)) / (y2 - y1);
            }

            if(code == code1)
            {
                x1 = x;
                y1 = y;
                code1 = encode(x1, y1, rect);
            } else
            {
                x2 = x;
                y2 = y;
                code2 = encode(x2, y2, rect);
            }
        }

		ret1->x = x1, ret1->y = y1;
		ret2->x = x2, ret2->y = y2;

		return true;

	}

	////////////////////////////////////////////////////////////

	int Algorithmic::hBitMapAntiAliasing(int scren_width,int scren_height,byte_8* src,byte_8* dest)
	{
		if(src == NULL || dest == NULL)
			return -1;

		int srcWidth = scren_width<<1;//scale * scren_width;
		int srcHeight = scren_height<<1;//scale * scren_height;
		int srcPitch = srcWidth<<2;//bitmap_bits*srcWidth/8; //bytes in each row

		int dstWidth = scren_width;
		int dstHeight = scren_height;
		int dstPitch = dstWidth<<2;//bitmap_bits*dstWidth/8 ;
		
		int gridSize = 4;/*scale * scale;*/
		int resultRed, resultGreen, resultBlue;
		int dstOffset;
		int srcX, srcY, srcOffset;
		int tmpX, tmpY, tmpOffset;

		int BytePerPixel = 4;/*bitmap_bits/8;*/

		for (int y=0; y<dstHeight; y++)
		{
			srcX = 0;
			srcY = (y<<1)*srcPitch + srcPitch;//(y * scale) * srcPitch + srcPitch*(scale-1);
			for (int x=0; x<dstWidth; x++)
			{
				srcX = x<<3;//(x * scale) * BytePerPixel;
				srcOffset = srcY + srcX;

				resultRed = resultGreen = resultBlue = 0;
				tmpY = 0;
				for (int i=0; i<2; i++)
				{
					tmpX = 0;
					for (int j=0; j<2; j++)
					{
						tmpOffset = srcOffset + tmpY + tmpX;

						resultBlue += src[tmpOffset];//r
						resultGreen += src[tmpOffset + 1];//g
						resultRed += src[tmpOffset + 2];//b

						tmpX += BytePerPixel;
					}
					tmpY -= srcPitch;
				}

				dstOffset = y*dstPitch + (x<<2);//y*dstPitch + x*BytePerPixel;

				//resultRed /= gridSize;
				//resultGreen /= gridSize;
				//resultBlue /= gridSize;

				dest[dstOffset] = (resultBlue>>2);//resultBlue
				dest[dstOffset + 1] = (resultGreen>>2);//resultGreen;
				dest[dstOffset + 2] = (resultRed>>2);//resultRed;

			}
		}

		return 0;
	}

}