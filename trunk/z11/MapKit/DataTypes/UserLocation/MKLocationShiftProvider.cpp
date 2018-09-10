#include "MKLocationShiftProvider.h"
#include "System/IO/MGBundle.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/Utility.h"
#include "System/IO/MGFileManager.h"
#include "Platform/MGFileUtils.h"
#include "../../Service/MKSearch.h"
#include <stdio.h>
#include <math.h>
using namespace z11;

namespace z11 {
    
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
    typedef struct offset_data {
        short lng;    //12151表示121.51  //
        short lat;    //3130表示31.30  //
        short x_off;  //地图x轴偏移像素值  //
        short y_off;  //地图y轴偏移像素值  //
    }offset_data;
    static double lngToPixel(double lng, int zoom) {
        return (lng + 180) * (256L << zoom) / 360;
    }
    static double latToPixel(double lat, int zoom) {
        double siny =sin(lat * M_PI / 180);
        double y = log((1 + siny) / (1 - siny));
        return (128 << zoom) * (1 - y / (2 * M_PI));
    }
    static double pixelToLng(double pixelX, int zoom) {
        return pixelX * 360 / (256L << zoom) - 180;
    }
    static double pixelToLat(double pixelY, int zoom) {
        double y = 2 * M_PI * (1 - pixelY / (128 << zoom));
        double z = pow(M_E, y);
        double siny = (z - 1) / (z + 1);
        return asin(siny) * 180 / M_PI;
    }
    static int compare_offset_data(const void *data11, const void *data22){
        offset_data *data1=(offset_data *)data11;
        offset_data *data2=(offset_data *)data22;
        int det_lng = (data1->lng)-(data2->lng);
        if (det_lng!=0) {
            return det_lng;
        }
        else{
            return (data1->lat)-(data2->lat);
        }
    }
    static int compare_offset_lng_data(const void *data11, const void *data22){
        offset_data *data1=(offset_data *)data11;
        offset_data *data2=(offset_data *)data22;
        int det_lng = (data1->lng)-(data2->lng);
        if (det_lng!=0) {
            return det_lng;
        }
        else{
            return 0;
        }
    }
    offset_data *buf=NULL;
    long long buflen=0;
    MGData *data=NULL;
    /*
     在win32上
     全国加载时间 1610ms
     浙江省加载时间 0ms
     */
    void initWGSTable(const string &offset_data_name)
    {
        MGLogD("initWGSTable %s",offset_data_name.c_str());
        LogStart();
        string filename=MGBundle::mainBundle()->pathWithFileName(offset_data_name);
        if (!Utility::isFileORDirExist(filename))
        {
            filename=MGBundle::mainBundle()->tmpPathWithFileName(offset_data_name);
            if (!Utility::isFileORDirExist(filename))
            {
                return ;
            }
        }
        
        if (data==NULL) {
            data=MGData::dataWithContentsOfFile(filename);
        }
        
        if (data!=NULL)
        {
            buf=(offset_data *)data->bytes();
            buflen=data->length();
            assert(buf!=NULL);
            assert(buflen!=0);
        }
        LogEnd("initWGSTable ");
    }
    //浙江省范围，大致为(31.5,117.5),(27.2,123.3)  //
    void WGS2MarsInZhejiang(double *lat, double *lng)
    {
        //首先优先测试浙江的数据  //
        if (buf==NULL)
        {
            initWGSTable("MapData/offset_zj.dat");
        }
        //如果没有的话测试全国数据  //
        if (buf==NULL)
        {
            initWGSTable("MapData/offset.dat");
            if (buf==NULL)
            {
                z11::MKLocationShiftProvider::chinaShiftEnabled=false;
                return ;
            }
            else
            {
                std::string filename=MGBundle::mainBundle()->pathWithFileName("MapData/offset_zj.dat");
                FILE *file = fopen(filename.c_str(), "wb");
                if (file!=NULL)
                {
                    for (int i=11750;i<12330;i++)
                    {
                        offset_data search_data;
                        search_data.lat = 2720;
                        search_data.lng = i;
                        offset_data *ret = (offset_data*)bsearch(&search_data, buf, buflen/sizeof(offset_data), sizeof(offset_data), compare_offset_data);//折半查找
                        if (ret==NULL)
                        {
                            ret= (offset_data*)bsearch(&search_data, buf, buflen/sizeof(offset_data), sizeof(offset_data), compare_offset_lng_data);//折半查找
                            if (ret!=NULL)
                            {
                                if (ret->lat>2720)
                                {
                                    int j=0;
                                    while (ret[j].lng==i&&ret[j].lat>=2720)
                                    {
                                        j--;
                                    }
                                    ret=ret+j+1;
                                }
                                else
                                {
                                    
                                    int j=0;
                                    while (ret[j].lng==i&&ret[j].lat<2720)
                                    {
                                        j++;
                                    }
                                    ret=ret+j-1;
                                }
                            }
                            else
                            {
                                continue;
                            }
                        }
                        
                        int j=0;
                        for (j=0;ret[j].lat<3150;j++);
                        fwrite(ret,j*sizeof(offset_data),1,file);
                    }
                    fclose(file);
                }
            }
        }
        
        //NSData *offsetFileData = [NSData dataWithContentsOfFile:@"offset.dat"];
        //const void *buf = [offsetFileData bytes]; //byte buf for content of file "offset.dat"
        //long long buflen = [offsetFileData length]; //length of byte buf for content of file "offset.dat"
        offset_data search_data;
        search_data.lat = (int)(*lat*100);
        search_data.lng = (int)(*lng*100);
        offset_data *ret = (offset_data*)bsearch(&search_data, buf, buflen/sizeof(offset_data), sizeof(offset_data), compare_offset_data);//折半查找
        if (ret!=NULL)
        {
            double pixY = latToPixel(*lat, 18);
            double pixX = lngToPixel(*lng, 18);
            pixY += ret->y_off;
            pixX += ret->x_off;
            *lat = pixelToLat(pixY, 18);
            *lng = pixelToLng(pixX, 18);
        }
        else
        {
            //MGLogD("(%lf,%lf) out of Zhejiang bounds",*lat,*lng);
        }
        
    }
    void WGS2Mars(double *lat, double *lng){
        //return ;
        if (buf==NULL)
        {
            initWGSTable("MapData/offset.dat");
        }
        if (buf==NULL)
        {
            z11::MKLocationShiftProvider::chinaShiftEnabled=false;
            return ;
        }
        //NSData *offsetFileData = [NSData dataWithContentsOfFile:@"offset.dat"];
        //const void *buf = [offsetFileData bytes]; //byte buf for content of file "offset.dat"
        //long long buflen = [offsetFileData length]; //length of byte buf for content of file "offset.dat"
        offset_data search_data;
        search_data.lat = (int)(*lat*100);
        search_data.lng = (int)(*lng*100);
        offset_data *ret = (offset_data*)bsearch(&search_data, buf, buflen/sizeof(offset_data), sizeof(offset_data), compare_offset_data);//折半查找
        if (ret!=NULL)
        {
            double pixY = latToPixel(*lat, 18);
            double pixX = lngToPixel(*lng, 18);
            pixY += ret->y_off;
            pixX += ret->x_off;
            *lat = pixelToLat(pixY, 18);
            *lng = pixelToLng(pixX, 18);
        }
        else
        {
            MGLogD("(%lf,%lf) out of China bounds",*lat,*lng);
        }
    }
}
    namespace z11
    {
        bool MKLocationShiftProvider::chinaShiftEnabled=true;
        const double MKLocationShiftProvider::earth_radius = 6378245.0;
        const double MKLocationShiftProvider::ee = 0.00669342162296594323;
        MKLocationCoordinate2D MKLocationShiftProvider::applyChinaLocationShift( const MKLocationCoordinate2D &coord )
        {
            if (chinaShiftEnabled)
            {
                MKLocationCoordinate2D ret=coord;
                //WGS2MarsInZhejiang(&ret.latitude,&ret.longitude);
                transform(coord.latitude,coord.longitude,ret.latitude,ret.longitude);
                return ret;
            }
            else
                return coord;
        }
        
        
        void MKLocationShiftProvider::transform(double wgLat, double wgLon, double& mgLat, double& mgLon)
        {
            if (outOfChina(wgLat, wgLon))
            {
                mgLat = wgLat;
                mgLon = wgLon;
                //MGLogD("out of China!");
                return;
            }
            double dLat = transformLat(wgLon - 105.0, wgLat - 35.0);
            double dLon = transformLon(wgLon - 105.0, wgLat - 35.0);
            double radLat = wgLat / 180.0 * M_PI;
            double magic = sin(radLat);
            magic = 1 - ee * magic * magic;
            double sqrtMagic = sqrt(magic);
            dLat = (dLat * 180.0) / ((earth_radius * (1 - ee)) / (magic * sqrtMagic) * M_PI);
            dLon = (dLon * 180.0) / (earth_radius / sqrtMagic * cos(radLat) * M_PI);
            mgLat = wgLat + dLat;
            mgLon = wgLon + dLon;
        }
        
        bool MKLocationShiftProvider::outOfChina(double lat, double lon)
        {
            if (lon < 72.004 || lon > 137.8347)
                return true;
            if (lat < 0.8293 || lat > 55.8271)
                return true;
            return false;
        }
        
        double MKLocationShiftProvider::transformLat(double x, double y)
        {
            double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x));
            ret += (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0;
            ret += (20.0 * sin(y * M_PI) + 40.0 * sin(y / 3.0 * M_PI)) * 2.0 / 3.0;
            ret += (160.0 * sin(y / 12.0 * M_PI) + 320 * sin(y * M_PI / 30.0)) * 2.0 / 3.0;
            return ret;
        }
        
        double MKLocationShiftProvider::transformLon(double x, double y)
        {
            double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
            ret += (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0;
            ret += (20.0 * sin(x * M_PI) + 40.0 * sin(x / 3.0 * M_PI)) * 2.0 / 3.0;
            ret += (150.0 * sin(x / 12.0 * M_PI) + 300.0 * sin(x / 30.0 * M_PI)) * 2.0 / 3.0;
            return ret;
        }
        
    }
    
    
    
#include "math.h"
#include "stdio.h"
    
    double casm_rr ;
    unsigned int casm_t1;
    unsigned int casm_t2;
    double casm_x1;
    double casm_y1;
    double casm_x2;
    double casm_y2;
    double casm_f;
    double yj_sin2(double x)
    {
        double tt ;
        double ss;
        int ff ;
        double s2;
        int cc;
        ff=0;
        if (x<0)
        {
            x=-x;
            ff=1;
        }
        cc=x/6.28318530717959;
        tt=x-cc*6.28318530717959;
        if (tt>3.1415926535897932)
        {
            tt=tt-3.1415926535897932;
            if (ff==1)
                ff=0;
            else if (ff==0)
                ff=1;
        }
        x=tt;
        ss=x;
        s2=x;
        tt=tt*tt;
        s2=s2*tt;
        ss=ss-s2* 0.166666666666667;
        s2=s2*tt;
        ss=ss+s2* 8.33333333333333E-03;
        s2=s2*tt;
        ss=ss-s2* 1.98412698412698E-04;
        s2=s2*tt;
        ss=ss+s2* 2.75573192239859E-06;
        s2=s2*tt;
        ss=ss-s2* 2.50521083854417E-08;
        if (ff==1)
            ss=-ss;
        return ss;
    }
    double Transform_yj5(double x , double y )
    {
        double tt;
        tt = 300 + 1 * x + 2 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(sqrt(x*x));
        tt = tt + (20 *yj_sin2(18.849555921538764 * x ) + 20 * yj_sin2(6.283185307179588 * x))*0.6667;
        tt = tt + (20 * yj_sin2(3.141592653589794 * x ) + 40 * yj_sin2(1.047197551196598 * x))*0.6667;
        tt = tt + (150 * yj_sin2(0.2617993877991495 * x) + 300 * yj_sin2(0.1047197551196598 * x))*0.6667;
        return tt;
    }
    double Transform_yjy5(double x , double y )
    {
        double tt ;
        tt = -100 +  2 * x + 3 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(sqrt(x*x));
        tt = tt + (20 * yj_sin2(18.849555921538764 * x) + 20 * yj_sin2(6.283185307179588 * x))*0.6667;
        tt = tt + (20 * yj_sin2(3.141592653589794 * y)+ 40 * yj_sin2(1.047197551196598 * y))*0.6667;
        tt = tt + (160 * yj_sin2(0.2617993877991495 * y) + 320 * yj_sin2(0.1047197551196598 * y))*0.6667;
        return tt;
    }
    double Transform_jy5(double x , double xx )
    {
        double n ;
        double a ;
        double e ;
        a = 6378245;
        e = 0.00669342;
        n = sqrt (1 - e * yj_sin2(x * 0.0174532925199433) * yj_sin2(x * 0.0174532925199433) );
        n = (xx * 180) /(a / n * cos(x * 0.0174532925199433) * 3.1415926) ;
        return n;
    }
    double Transform_jyj5(double x , double yy )
    {
        double m ;
        double a ;
        double e ;
        double mm;
        a = 6378245;
        e = 0.00669342;
        mm= 1 - e * yj_sin2(x * 0.0174532925199433) * yj_sin2(x * 0.0174532925199433) ;
        m = (a * (1 - e)) / (mm * sqrt(mm));
        return (yy * 180) / (m * 3.1415926);
    }
    double r_yj()
    {
        int casm_a ;
        int casm_c ;
        casm_a = 314159269;
        casm_c = 453806245;
        return 0;
    }
    double random_yj()
    {
        int t;
        int casm_a ;
        int casm_c ;
        casm_a = 314159269;
        casm_c = 453806245;
        casm_rr = casm_a * casm_rr + casm_c ;
        t = casm_rr /2 ;
        casm_rr = casm_rr - t * 2;
        casm_rr = casm_rr / 2 ;
        return (casm_rr);
    }
    void IniCasm(unsigned int w_time, unsigned int w_lng, unsigned int w_lat)
    {
        int tt;
        casm_t1 = w_time ;
        casm_t2 = w_time ;
        tt=w_time/0.357;
        casm_rr=w_time-tt*0.357;
        if (w_time==0)
            casm_rr=0.3;
        casm_x1 = w_lng;
        casm_y1 = w_lat;
        casm_x2 = w_lng;
        casm_y2 = w_lat;
        casm_f=3;
    }
    unsigned int wgtochina_lb(int wg_flag, unsigned int wg_lng, unsigned int wg_lat, int wg_heit,  int wg_week, unsigned int wg_time, unsigned  int *china_lng, unsigned int *china_lat)
    {
        double x_add ;
        double y_add ;
        double h_add ;
        double x_l;
        double y_l;
        double casm_v ;
        double t1_t2;
        double x1_x2;
        double y1_y2;
        
        
        
        if (wg_heit>5000)
        {
            *china_lng = 0 ;
            *china_lat = 0;
            return 0xFFFF95FF;
        }
        x_l =  wg_lng;
        x_l =  x_l / 3686400.0;
        y_l =  wg_lat ;
        y_l =  y_l / 3686400.0;
        if (x_l < 72.004)
        {
            *china_lng = 0 ;
            *china_lat = 0;
            return 0xFFFF95FF;
        }
        if ( x_l > 137.8347)
        {
            *china_lng = 0 ;
            *china_lat = 0;
            return 0xFFFF95FF;
        }
        if (y_l < 0.8293)
        {
            *china_lng = 0 ;
            *china_lat = 0;
            return 0xFFFF95FF;
        }
        if ( y_l > 55.8271)
        {
            *china_lng = 0 ;
            *china_lat = 0;
            return 0xFFFF95FF;
        }
        if (wg_flag ==0)
        {
            IniCasm(wg_time,wg_lng,wg_lat);
            *china_lng = wg_lng;
            *china_lat = wg_lat;
            return 0x00000000;
        }
        
        casm_t2= wg_time ;
        t1_t2 =(double)(casm_t2 - casm_t1)/1000.0;
        if ( t1_t2<=0 )
        {
            casm_t1= casm_t2 ;
            casm_f=casm_f+1;
            casm_x1 = casm_x2;
            casm_f=casm_f+1;
            casm_y1 = casm_y2;
            casm_f=casm_f+1;
        }
        else
        {
            if ( t1_t2 > 120 )
            {
                if (casm_f == 3)
                {
                    casm_f=0;
                    casm_x2 = wg_lng;
                    casm_y2 = wg_lat;
                    x1_x2 = casm_x2 - casm_x1;
                    y1_y2 = casm_y2 - casm_y1;
                    casm_v = sqrt (x1_x2 * x1_x2 + y1_y2 * y1_y2 ) /t1_t2;
                    if (casm_v  > 3185)
                    {
                        *china_lng = 0 ;
                        *china_lat = 0;
                        return (0xFFFF95FF);
                    }
                    
                }
                casm_t1= casm_t2 ;
                casm_f=casm_f+1;
                casm_x1 = casm_x2;
                casm_f=casm_f+1;
                casm_y1 = casm_y2;
                casm_f=casm_f+1;
            }
        }
        x_add = Transform_yj5(x_l - 105, y_l - 35) ;
        y_add = Transform_yjy5(x_l - 105, y_l - 35) ;
        h_add = wg_heit;
        
        x_add = x_add + h_add * 0.001 + yj_sin2(wg_time*0.0174532925199433)+random_yj();
        y_add = y_add + h_add * 0.001 + yj_sin2(wg_time*0.0174532925199433)+random_yj();
        *china_lng = (x_l + Transform_jy5(y_l, x_add)) * 3686400;
        *china_lat = (y_l + Transform_jyj5(y_l, y_add)) * 3686400;
        return (0x00000000);
    }
