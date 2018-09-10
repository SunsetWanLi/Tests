#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/common/Counted.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/common/GreyscaleLuminanceSource.h>
#include <zxing/ReaderException.h>
using namespace zxing;
using namespace zxing::qrcode;
#include "../../../../GraphicCore/MGImage.h"
#include "../../../../System/Shared/MGLog.h"
#include "../../MGImageSource/MGImageSource.h"
#include "../../../../System/IO/MGFileManager.h"
#include <stdio.h>
using namespace z11;
#define LOG_DECODE_TIME
bool decode_image(MGImage* image,std::string &str)
{
	bool ret=true;
	if(image!=NULL)
	{
		zxing::Ref<LuminanceSource> source (new MGImageSource(image));
		zxing::Ref<HybridBinarizer> binarizer (new HybridBinarizer(source));
		zxing::Ref<BinaryBitmap> grayImage (new BinaryBitmap(binarizer));;
		QRCodeReader reader;
		DecodeHints hints(DecodeHints::DEFAULT_HINT);
		hints.setTryHarder(true); 

		try
		{
			zxing::Ref<Result> result = reader.decode(grayImage,DecodeHints(DecodeHints::BARCODEFORMAT_QR_CODE_HINT));
			str = result->getText()->getText().c_str();
		}
		catch(Exception& e)
		{
			ret=false;
			str = e.what();
		}
	}
	return ret;
}
bool decode_image(const string& path,std::string &str)
{
	MGTimeLogD(path.c_str());
	bool ret=true;
	MGImage* image =  ( MGImage::imageWithContentsOfFile(path) );
	ret=decode_image(image,str);
	delete_and_set_null(image);
	return ret;
}
void test()
{
	int count=0;
	int total=0;
	int counter[]={20,35,42,48,15,15};
	std::vector<std::string> failed_name;
	for(int j=1;j<=6;j++)
	{
		total+=counter[j-1];
		for(int i=1;i<=counter[j-1];i++)
		{
			char str[128];
			sprintf(str,"D:/test2/%d.bmp",i);
			sprintf(str,"D:/Downloads/ZXing-2.1-testdata/zxing-2.1/core/test/data/blackbox/qrcode-%d/%d.jpg",j,i);

			if(!MGFileManager::defaultManager()->fileExistsAtPath(str))
			{
				sprintf(str,"D:/Downloads/ZXing-2.1-testdata/zxing-2.1/core/test/data/blackbox/qrcode-%d/%d.png",j,i);
				if(!MGFileManager::defaultManager()->fileExistsAtPath(str))
				{
					total--;
					continue;
				}
			}

			string decode_string;
			bool flag=decode_image(str,decode_string);
			count+=flag;
			MGLogD("%s: %s .",str,flag?"success":"fail");
			if(flag)
				MGLogD("%s",decode_string.c_str());

		}
	}

	MGLogD("Sucess %d, Failed %d",count,total-count);
	//for (size_t i=0;i<failed_name.size();i++)
	//{
	//	MGLogD("Failed File %d Name: %s",i,failed_name[i].c_str());
	//}
}