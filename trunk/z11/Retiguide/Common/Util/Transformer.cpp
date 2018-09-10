#include "Transformer.h"
#include "../Types/Fraction.h"
#include "../../DataFetcher/DataReader.h"

namespace GIS_Embedded
{

	vector<CoordinateTable> Transformer::s_CoordinateTable;

	void Transformer::setTansformPara(const Scope& screen_scope,double offset_angle,int width,int height)
	{
		_offset_angle = offset_angle;

		_v_cos = cos(_offset_angle);
		_v_sin = sin(_offset_angle);

		mWidth = width;
		mHeight = height;

		if(fabs(_offset_angle) > DOUBLE_ZERO)
		{
			_rotato_x_para = (width>>1) - 1*_v_cos*(width>>1) + _v_sin*(height>>1);
			_rotato_y_para = (height>>1) - 1*_v_sin*(width>>1) - _v_cos*(height>>1);
		}

    	_screen_pix_width=width;
    	_screen_pix_height=height;
		_screen_scope.setScope(screen_scope);
		
		_x_center = (_screen_scope.getLeft()+_screen_scope.getRight())>>1;
		_y_center = (_screen_scope.getTop()+_screen_scope.getBottom())>>1;

		_Sx = Fraction(_screen_scope.getWidth(), _screen_pix_width);
		_Sx.reduct();
		_Sy = Fraction(_screen_scope.getHeight() , _screen_pix_height);
		_Sy.reduct();

		int tmp = int_64(screen_scope.getLeft()) * _Sx.denominator;
		_Tx = -int_64(screen_scope.getLeft()) * _Sx.denominator / _Sx.numerator;
		if(tmp % _Sx.numerator > (_Sx.numerator>>1) /*/ 2*/)
			_Tx --;

		tmp = int_64(screen_scope.getTop()) * _Sy.denominator;
		_Ty = -int_64(screen_scope.getTop()) * _Sy.denominator / _Sy.numerator;
		if(tmp % _Sy.numerator > (_Sx.numerator>>1) /*/ 2*/)
			_Ty --;

	}

// 	int Transformer::load3DCoordinateTable()
// 	{
// 		string file_path = RootDriverPath::getRootDriverPath() + "GIS_Embedded\\conf\\Coordinate.tbl";
// 		FILE* fp_in;
// 		fp_in = fopen(file_path.c_str(), "rb");
// 		if(fp_in == NULL)
// 		{
// 			return -1;
// 		}
// 
// 		int start = ftell(fp_in);
// 		fseek(fp_in,0,SEEK_END);
// 		int size = ftell(fp_in);
// 		fseek(fp_in , start, SEEK_SET);
// 
// 		byte_8 * data = new byte_8[size];
// 
// 		fread(data, size, 1, fp_in);
// 
// 		DataReader read;
// 		read.setStartPointer(data);
// 
// 		int table_sum = read.getInt();
// 		s_CoordinateTable.resize(table_sum);
// 		for(int i=0;i<table_sum;i++)
// 		{
// 			CoordinateTable& table = s_CoordinateTable[i];
// 			table.mScreenWidth = read.getInt();
// 			int row = read.getInt();
// 			int col = read.getInt();
// 			table.mRow = row;
// 			table.mCol = col;
// 
// 			///////////////
// 			table.mRowTable = new CoordinateTable::RowTable[row];
// 
// 			for(int row_i=0;row_i<row;row_i++)
// 			{
// 				CoordinateTable::RowTable& rowTabel = table.mRowTable[row_i];
// 				rowTabel.Y = read.getShort();
// 				//rowTabel.XHead = read.getShort();
// 				rowTabel.XSpace = read.getShort();
// 				short xRowTableSize = read.getShort();
// 				rowTabel.XRowTable.resize(xRowTableSize);
// 
// 				for(int col_i=0;col_i<xRowTableSize;col_i++)
// 				{
// 					rowTabel.XRowTable[col_i].first = read.getShort();
// 					rowTabel.XRowTable[col_i].second = read.getShort();
// 				}
// 			}
// 			///////////////
// 
// 			//s_CoordinateTable[i].mTable = new MGPoint[col*row];
// 
// 			//for(int row_i=0;row_i<row;row_i++)
// 			//	for(int col_i=0;col_i<col;col_i++)
// 			//	{
// 			//		s_CoordinateTable[i].mTable[row_i*col+col_i].x = read.getShort();
// 			//		s_CoordinateTable[i].mTable[row_i*col+col_i].y = read.getShort();
// 			//	}
// 		}
// 
// 		fclose(fp_in);
// 		delete[] data;
// 
// 		return 0;
// 	}

	int Transformer::transCoordinateScope(Scope* dataScope, Scope* transScope, int left,int top,int right,int bottom,const Scope& viewScope,bool isRotato)
	{
		int_32 lt_x,lt_y,rt_x,rt_y,rb_x,rb_y;
		mCoordinate.screen2map(left,top,&lt_x,&lt_y);
		mCoordinate.screen2map(right,bottom,&rb_x,&rb_y);
		mCoordinate.screen2map(right,top,&rt_x,&rt_y);

		int width = (rb_x - lt_x)*0.7; //按某一比例减小范围，提高速度
		int height = width*s_CoordinateTable[m_TableIndex].mRow/s_CoordinateTable[m_TableIndex].mCol;//lt_y - rb_y;

		int center_x = (viewScope.getLeft() + viewScope.getRight())/2;
		int center_y = (viewScope.getTop() + viewScope.getBottom())/2;

		int len = height*151/336; //将中心点往下移而做的调�?
		center_x -= len*_v_sin;
		center_y += len*_v_cos;

		transScope->setScope(center_x - width/2,center_y + height/2,center_x + width/2,center_y - height/2);

		dataScope->setScope(*transScope);
		if(isRotato)
			dataScope->resizeScope(1.4); //   (3/4)*(sqrt(2)/2)

		_screen_scope.setScope(*transScope);

		_Sx = Fraction(_screen_scope.getWidth(), _screen_pix_width);
		_Sx.reduct();
		_Sy = Fraction(_screen_scope.getHeight() , _screen_pix_height);
		_Sy.reduct();

		int tmp = int_64(_screen_scope.getLeft()) * _Sx.denominator;
		_Tx = -int_64(_screen_scope.getLeft()) * _Sx.denominator / _Sx.numerator;
		if(tmp % _Sx.numerator > (_Sx.numerator>>1) /*/ 2*/)
			_Tx --;

		tmp = int_64(_screen_scope.getTop()) * _Sy.denominator;
		_Ty = -int_64(_screen_scope.getTop()) * _Sy.denominator / _Sy.numerator;
		if(tmp % _Sy.numerator > (_Sx.numerator>>1) /*/ 2*/)
			_Ty --;


		return 0;
	}
	    
	int Transformer::translateFromScrToMap(int screen_x,int screen_y,int_32 *map_x,int_32 *map_y)
	{
		if(fabs(_offset_angle) <= DOUBLE_ZERO)
		{
			if (m_is_pymid)
			{
				*map_x = _screen_scope.getLeft() + screen_x / m_scale_width;
			*map_y = _screen_scope.getBottom() + (_screen_scope.getHeight() - screen_y / m_scale_height);

			}
			else
			{
				int x_ori = int_64(screen_x) * _Sx.numerator / _Sx.denominator;
				int y_ori = int_64(screen_y) * _Sy.numerator / _Sy.denominator;

				*map_x = x_ori + _screen_scope.getLeft();
				*map_y = _screen_scope.getTop() - y_ori;

			}
		}
		else
		{
			int tmp_x = screen_x-(_screen_pix_width>>1);
			int tmp_y = screen_y-(_screen_pix_height>>1);
			int x_ori = (int_32)(tmp_x * _v_cos + tmp_y * _v_sin);
			int y_ori = (int_32)(-tmp_x * _v_sin + tmp_y * _v_cos);

			*map_x = x_ori * _Sx.numerator /_Sx.denominator+ _x_center;
			*map_y = _y_center - y_ori * _Sy.numerator / _Sy.denominator;
		}

		return 0;
	}

	void Transformer::translateFromScrDisToMap(int x_begin,int x_end,int y_begin,int y_end,int_32 *map_x_dis,int_32 *map_y_dis)
	{
		y_begin += mHeight/2 - y_end;
		y_end = mHeight/2;

		int_32 map_begin_x,map_begin_y,map_end_x,map_end_y;

		translateFromScrToMap(x_begin,y_begin,&map_begin_x,&map_begin_y);
		translateFromScrToMap(x_end,y_end,&map_end_x,&map_end_y);

		*map_x_dis = -( map_end_x - map_begin_x);
		*map_y_dis = map_end_y - map_begin_y;

		//DEBUG_INFO(*map_x_dis);
		//DEBUG_INFO(",");
		//DEBUG_INFO(*map_x_dis);
		//DEBUG_INFO("\n");

	}

	int Transformer::transfromMapToScr(int_32 x,int_32 y,long *screen_x, long *screen_y)const
	{

		if (m_is_pymid)
		{
			*screen_x = (x - _screen_scope.getLeft()) * m_scale_width;
			*screen_y = (_screen_scope.getTop()-y) * m_scale_height;
		}
		else
		{
			long x_ori = int_64(x) * _Sx.denominator / _Sx.numerator + _Tx;
			long y_ori = - (int_64(y) * _Sy.denominator / _Sy.numerator + _Ty);

			if(fabs(_offset_angle) <= DOUBLE_ZERO)
			{
				*screen_x = x_ori;
				*screen_y = y_ori;
			}
			else
			{
				*screen_x = (long)(_rotato_x_para + x_ori*_v_cos - y_ori*_v_sin);
				*screen_y = (long)(_rotato_y_para + x_ori*_v_sin + y_ori*_v_cos);
			}

		}


		return 0;

	}
	
	int Transformer::transfromMapToScr(int_32 x,int_32 y,float *screen_x,float *screen_y)const
	{
		long temp_x, temp_y;
		int ret =transfromMapToScr(x, y, &temp_x, &temp_y);
		*screen_x = temp_x;
		*screen_y = temp_y;
		return ret;

	}

	int Transformer::transformPos2DsToArray(vector<Pos2DEx>& pos2ds,z11::MGPoint* points)const
	{
		int sum = pos2ds.size();
		return transformPos2DsToArray(pos2ds, sum, points);
	}

	int Transformer::transformPos2DsToArray(vector<Pos2DEx>& pos2ds,int sum, z11::MGPoint* points)const
	{
		int i=0;
		for(i=0;i<sum;i++)
		{
			if (m_is_pymid)
			{
				points[i].x = (pos2ds[i].d0 - _screen_scope.getLeft()) * m_scale_width;
				points[i].y = (_screen_scope.getTop()-pos2ds[i].d1) * m_scale_height;
			}
			else
			{
				points[i].x = int_64(pos2ds[i].d0) * _Sx.denominator / _Sx.numerator + _Tx;
				points[i].y = - (int_64(pos2ds[i].d1) * _Sy.denominator / _Sy.numerator + _Ty);
			}
		}
		return sum;
	}

	int Transformer::transformPos2DsToArray(vector<Pos2DEx>& pos2ds,GIS_Embedded::Scope scope, z11::MGPoint* points)const
{
	int i=0;
	int x,y;
	int j=0;
	int sum = pos2ds.size();
	for(i=0;i<sum;i++)
	{
		if (m_is_pymid)
		{
			x = (pos2ds[i].d0 - _screen_scope.getLeft()) * m_scale_width;
			y = (_screen_scope.getTop()-pos2ds[i].d1) * m_scale_height;
			if (scope.isContain(x,y))
			{
				points[j].x = x;
				points[j].y = y;
				j++;
			}
		}
		else
		{
			x = int_64(pos2ds[i].d0) * _Sx.denominator / _Sx.numerator + _Tx;
			y = - (int_64(pos2ds[i].d1) * _Sy.denominator / _Sy.numerator + _Ty);
			if (scope.isContain(x,y))
			{
				points[j].x = x;
				points[j].y = y;
				j++;
			}
		}
	}
	return sum;
}
	int Transformer::transformPos2DsToArray(vector<Pos2D>& pos2ds,int sum, z11::MGPoint* points)const
	{
		int i=0;
		if(fabs(_offset_angle) <= DOUBLE_ZERO)
		{
			for(i=0;i<sum;i++)
			{
				points[i].x = int_64(pos2ds[i].d0) * _Sx.denominator / _Sx.numerator + _Tx;
				points[i].y = - (int_64(pos2ds[i].d1) * _Sy.denominator / _Sy.numerator + _Ty);
			}
		}
		else if(fabs(_offset_angle) > DOUBLE_ZERO)
		{
			int x_ori,y_ori;
			for(i=0;i<sum;i++)
			{
				x_ori = int_64(pos2ds[i].d0) * _Sx.denominator / _Sx.numerator + _Tx;
				y_ori = - (int_64(pos2ds[i].d1) * _Sy.denominator / _Sy.numerator + _Ty);

				points[i].x = (int)(_rotato_x_para + x_ori*_v_cos - y_ori*_v_sin);
				points[i].y = (int)(_rotato_y_para + x_ori*_v_sin + y_ori*_v_cos);
			}
		}

		if(_visual_state != 0)
		{
			//for(i=0;i<sum;i++)
			//points[i].y += _screen_pix_height*4/9;//((_screen_pix_height*3)>>3);

			assert(sum < 2500);

			static z11::MGPoint dest[3000];
			int clip_sum = clipByRect(dest, points, sum, 0,0, _screen_pix_width-1,_screen_pix_height-1);

			assert(clip_sum <= sum+COORDINATE_CLIP_BUFFER);

			if(clip_sum<2)
				return 0;

			for(i=0;i<clip_sum;i++)
			{
				assert(dest[i].y >=0 && dest[i].y < s_CoordinateTable[m_TableIndex].mRow);
				assert(dest[i].x >=0 && dest[i].x < s_CoordinateTable[m_TableIndex].mCol);

				points[i].x = calXFrom3DTable(s_CoordinateTable[m_TableIndex],dest[i].x,dest[i].y);//s_CoordinateTable[m_TableIndex].mTableRow[dest[i].y].RowX[dest[i].x];
				points[i].y = s_CoordinateTable[m_TableIndex].mRowTable[(int)(dest[i].y + 0.5)].Y;
				//points[i].x = s_CoordinateTable[m_TableIndex].mTable[dest[i].y*s_CoordinateTable[m_TableIndex].mCol+dest[i].x].x;
				//points[i].y = s_CoordinateTable[m_TableIndex].mTable[dest[i].y*s_CoordinateTable[m_TableIndex].mCol+dest[i].x].y;
			}

			return clip_sum;
		}

		return sum;
	}

// 	int Transformer::transformPointsToMgPoints(MGPoint* points,int sum,vector<MGPoint>& mg_points)
// 	{
// 		int i=0;
// 		for(i=0;i<sum;i++)
// 		{
// 			mg_points[i].x = *points)[i].x;
// 			mg_points[i].y = *points)[i].y;
// 		}
// 		return sum;
// 
// 	}

	long Transformer::calXFrom3DTable(CoordinateTable& table,int width,int height) const
	{
		long retX = -1;

		vector<pair<short,short> >& xRowTable = table.mRowTable[height].XRowTable;

		if(width == 0)
		{
			retX = xRowTable[0].second;
			return retX;
		}

		int begin=0,end=xRowTable.size()-1,index = xRowTable.size()/2;

		//int testTimer1 = 0;

		while(index != end)
		{
			//testTimer1 ++;

			if( xRowTable[index].first <= width && xRowTable[index+1].first >= width)
			{
				if(xRowTable[index+1].first == width)
					index++;

				break;
			}

			if(xRowTable[index].first < width)
			{
				begin = index;
				index = (begin+end)/2;
			}
			else if(xRowTable[index].first > width)
			{
				end = index;
				index = (begin+end)/2;
			}
		}

		//int testTimer2 =0;
		//for(int i=1,sum = xRowTable.size();i<sum;++i)
		//{
		//	testTimer2++;
		//	if(width >= xRowTable[i-1].first && width <= xRowTable[i].first)
		//	{
		//		break;
		//	}
		//}

		//DEBUG_INFO(testTimer1);
		//DEBUG_INFO(",");
		//DEBUG_INFO(testTimer2);
		//DEBUG_INFO(",");
		//DEBUG_INFO(xRowTable.size());
		//DEBUG_INFO("\n");

		retX = xRowTable[0].second + width*table.mRowTable[height].XSpace + xRowTable[index].second;

		return retX;

		//short xHead = table.mRowTable[height].XHead;
		//short space  = table.mRowTable[height].XSpace;

		//vector<pair<short,short> >& xRowTable = table.mRowTable[height].XRowTable;

		//if(xRowTable.size() ==0)
		//{
		//	retX = xHead + width*space;
		//	return retX;
		//}
		//else if(width < xRowTable[0].first)
		//{
		//	retX = xHead + width*space;
		//	return retX;
		//}
		//else if(width >= xRowTable[xRowTable.size()-1].first)
		//{
		//	retX = xHead + width*space + xRowTable[xRowTable.size()-1].second;
		//	return retX;
		//}

		//for(int i=1,sum = xRowTable.size();i<sum;++i)
		//{
		//	if(width >= xRowTable[i-1].first && width < xRowTable[i].first)
		//	{
		//		retX = xHead + width*space + xRowTable[i-1].second;
		//		return retX;
		//	}
		//}

		//assert(-1);

		//return retX;
	}

	int Transformer::clipByRect(z11::MGPoint* dest_pts, const z11::MGPoint* src_pts, int src_sum, int x1, int y1, int x2, int y2)
	{
		// 区域裁减算法，依次将四条边外面的删掉�?
		// 首先是左�?
		static z11::MGPoint* temp_pts = NULL;
		static uint_32 current_size = 0;
		if(current_size < src_sum * 2)
		{
			current_size = src_sum * 2;
			delete []temp_pts;
			temp_pts = new z11::MGPoint[current_size];
		}

		long i, k;
		long size = src_sum;
		bool inflag;
		// 始点如果在里面，首先加入
		if( size > 0 && src_pts[0].x >= x1 )
		{
			temp_pts[0] = src_pts[0];
			k = 1;
			inflag = true;
		}
		else
		{
			k = 0; 
			inflag = false;
		}
		for( i = 1; i < size; i++ )
		{
			if( true == inflag )
			{
				// 都在里面，加入终点，
				if( src_pts[i].x >= x1 )
					temp_pts[k++] = src_pts[i];
				// 由里到外，加入交界点�?
				else 
				{
					int y;
					y = src_pts[i - 1].y + ( src_pts[i].y - src_pts[i - 1].y )
						* (x1 - src_pts[i - 1].x)
						/ ( src_pts[i].x - src_pts[i - 1].x );
					temp_pts[k].x = x1;
					temp_pts[k].y = y;
					k++;
					inflag = false;
				}
			}
			else
			{
				// 由外到内，加入交界点和终点�?
				if( src_pts[i].x >= x1 )
				{
					int y;
					y = src_pts[i - 1].y + ( src_pts[i].y - src_pts[i - 1].y )
						* (x1 - src_pts[i - 1].x)
						/ ( src_pts[i].x - src_pts[i - 1].x );
					temp_pts[k].x = x1;
					temp_pts[k].y = y;
					k++;
					temp_pts[k++] = src_pts[i];
					inflag = true;
				}
				// 都在外边，不�?
				//	else if( src_pts[i].x < x1 && src_pts[i + 1].x < x1 )
				//		;
			}
		}
		// 0601；如果原来首尾点重合，新的首尾点不重合，则强制重合，保证面切割的正确性�?
		if( k >= 2 && src_pts[0].x == src_pts[size - 1].x 
			&& src_pts[0].y == src_pts[size - 1].y 
			&& (temp_pts[0].x != temp_pts[k - 1].x 
			|| temp_pts[0].y != temp_pts[k - 1].y)  )
		{
			// 增加一个点�?
			temp_pts[k++] = temp_pts[0];
		}

		// 然后是右边，
		size = k;
		if( size > 0 && temp_pts[0].x <= x2 )
		{
			dest_pts[0] = temp_pts[0];
			k = 1;
			inflag = true;
		}
		else
		{
			k = 0;
			inflag = false;
		}
		for( i = 1; i < size; i++ )
		{
			if( true == inflag )
			{
				// 都在里面，加入终点，
				if( temp_pts[i].x <= x2 )
					dest_pts[k++] = temp_pts[i];
				// 由里到外，加入交界点�?
				else 
				{
					int y;
					y = temp_pts[i - 1].y + ( temp_pts[i].y - temp_pts[i - 1].y ) 
						* (x2 - temp_pts[i - 1].x)
						/ ( temp_pts[i].x - temp_pts[i - 1].x ) ;
					dest_pts[k].x = x2;
					dest_pts[k].y = y;
					k++;
					inflag = false;
				}
			}
			else
			{
				// 由外到内，加入交界点和终点�?
				if( temp_pts[i].x <= x2 )
				{
					int y;
					y = temp_pts[i - 1].y + ( temp_pts[i].y - temp_pts[i - 1].y ) 
						* (x2 - temp_pts[i - 1].x)
						/ ( temp_pts[i].x - temp_pts[i - 1].x ) ;
					dest_pts[k].x = x2;
					dest_pts[k].y = y;
					k++;
					dest_pts[k++] = temp_pts[i];
					inflag = true;
				}
				// 都在外边，不�?
			}
		}
		if( k >= 2 && temp_pts[0].x == temp_pts[size - 1].x 
			&& temp_pts[0].y == temp_pts[size - 1].y 
			&& (dest_pts[0].x != dest_pts[k - 1].x 
			|| dest_pts[0].y != dest_pts[k - 1].y) )
		{
			// 增加一个点�?
			dest_pts[k++] = dest_pts[0];
		}

		// 首先是下�?
		size = k;
		if( k > 0 && dest_pts[0].y >= y1 )
		{
			temp_pts[0] = dest_pts[0];
			k = 1;
			inflag = true;
		}
		else
		{
			k = 0;
			inflag = false;
		}
		for( i = 1; i < size; i++ )
		{
			if( true == inflag )
			{
				// 都在里面，加入终点，
				if( dest_pts[i].y >= y1 )
					temp_pts[k++] = dest_pts[i];
				// 由里到外，加入交界点�?
				else 
				{
					int x;
					x = dest_pts[i - 1].x + ( dest_pts[i].x - dest_pts[i - 1].x ) 
						* (y1 - dest_pts[i - 1].y)
						/ ( dest_pts[i].y - dest_pts[i - 1].y ) ;
					temp_pts[k].x = x;
					temp_pts[k].y = y1;
					k++;
					inflag = false;
				}
			}
			else
			{
				// 由外到内，加入交界点和终点�?
				if( dest_pts[i].y >= y1 )
				{
					int x;
					x = dest_pts[i - 1].x + ( dest_pts[i].x - dest_pts[i - 1].x )
						* (y1 - dest_pts[i - 1].y)
						/ ( dest_pts[i].y - dest_pts[i - 1].y ) ;
					temp_pts[k].x = x;
					temp_pts[k].y = y1;
					k++;
					temp_pts[k++] = dest_pts[i];
					inflag = true;
				}
				// 都在外边，不�?
			}
		}
		if( k >= 2 && dest_pts[0].x == dest_pts[size - 1].x 
			&& dest_pts[0].y == dest_pts[size - 1].y 
			&& (temp_pts[0].x != temp_pts[k - 1].x 
			|| temp_pts[0].y != temp_pts[k - 1].y) )
		{
			// 增加一个点�?
			temp_pts[k++] = temp_pts[0];
		}

		// 最后是上边�?
		size = k;
		if( k > 0 && temp_pts[0].y <= y2 )
		{
			dest_pts[0] = temp_pts[0];
			k = 1;
			inflag = true;
		}
		else
		{
			k = 0;
			inflag = false;
		}
		for( i = 1; i < size; i++ )
		{
			if( true == inflag )
			{
				// 都在里面，加入终点，
				if( temp_pts[i].y <= y2)
					dest_pts[k++] = temp_pts[i];
				// 由里到外，加入交界点�?
				else 
				{
					int x;
					x = temp_pts[i - 1].x + ( temp_pts[i].x - temp_pts[i - 1].x ) 
						* (y2 - temp_pts[i - 1].y)
						/ ( temp_pts[i].y - temp_pts[i - 1].y ) ;
					dest_pts[k].x = x;
					dest_pts[k].y = y2;
					k++;
					inflag = false;
				}
			}
			else
			{
				// 由外到内，加入交界点和终点�?
				if( temp_pts[i].y <= y2 )
				{
					int x;
					x = temp_pts[i - 1].x + ( temp_pts[i].x - temp_pts[i - 1].x ) 
						* (y2 - temp_pts[i - 1].y)
						/ ( temp_pts[i].y - temp_pts[i - 1].y ) ;
					dest_pts[k].x = x;
					dest_pts[k].y = y2;
					k++;
					dest_pts[k++] = temp_pts[i];
					inflag = true;
				}
				// 都在外边，不�?
			}
		}
		if( k >= 2 && temp_pts[0].x == temp_pts[size - 1].x 
			&& temp_pts[0].y == temp_pts[size - 1].y 
			&& (dest_pts[0].x != dest_pts[k - 1].x 
			|| dest_pts[0].y != dest_pts[k - 1].y) )
		{
			// 增加一个点�?
			dest_pts[k++] = dest_pts[0];
		}

		//delete[] temp_pts;
		

		//add by my 删除重复�?
		int valid_index = 0;
		for(int index = 1; index < k; index ++)
		{
			if(!(dest_pts[index].x == dest_pts[valid_index].x && dest_pts[index].y == dest_pts[valid_index].y))
			{
				++ valid_index;
				if(valid_index != index)
					dest_pts[valid_index] = dest_pts[index];
			}
		}

		//by my
		return valid_index + 1;
		//return k;

	}

}
