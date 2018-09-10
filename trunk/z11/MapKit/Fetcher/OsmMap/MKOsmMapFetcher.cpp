#include "MKOsmMapFetcher.h"
#include "binaryRead.h"
#include "renderRules.h"
#include "rendering.h"
#include <SkImageEncoder.h>
#include <SkGraphics.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "System/IO/MGBundle.h"
#include "System/IO/MGFileManager.h"
#include "System/Shared/Utility.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "System/Shared/MGLog.h"
#include "GraphicCore/MGImage.h"
#include <math.h>
using namespace std;
#include "osmand_main.h"
#define PART_WIDTH_DIGIT 3
#define PART_WIDTH (1<<PART_WIDTH_DIGIT)
#define EDGE_WIDTH 2
namespace z11
{
	//default folder,will init in MKOsmMapFetcher constructor
	std::string input_file="E:/Projects/osmand_workspace/Planet_117.obf";
	std::string renderingFileName="E:/Projects/osmand_workspace/default.render.xml";
	std::string imagesFileName="D:/test4/";
	RenderingRulesStorage* st=NULL;
	RenderingRulesStorage* initRenderingRulesStorage()
	{
		if (st==NULL)
		{
			st = new RenderingRulesStorage(renderingFileName.c_str());
			st->parseRulesFromXmlInputStream(renderingFileName.c_str(), NULL);
		}
		return st;
	}
	RenderingRuleSearchRequest* searchRequest =NULL;
	RenderingRuleSearchRequest *initRenderingRuleSearchRequest()
	{
		if (searchRequest==NULL)
		{
			 searchRequest=new RenderingRuleSearchRequest(st);
		}
		return searchRequest;
	}
	ResultPublisher *publisher=NULL;
	ResultPublisher *initResultPublisher()
	{
		if (publisher==NULL)
		{
			publisher = new ResultPublisher();
		}
		return publisher;
	}
	SkBitmap*  initBitmap(SkBitmap* &part_bitmap,int width,int height,bool noPolygons=false)
	{
		if (part_bitmap!=NULL)
		{
			if (!noPolygons&&part_bitmap->getConfig()==SkBitmap::kARGB_8888_Config)
			{
				delete_and_set_null(part_bitmap);
			}
			else if(noPolygons&&part_bitmap->getConfig()==SkBitmap::kRGB_565_Config)
			{
				delete_and_set_null(part_bitmap);
			}
		}
		if (part_bitmap==NULL)
		{
			part_bitmap = new SkBitmap();
			part_bitmap->setConfig(noPolygons?SkBitmap::kARGB_8888_Config:SkBitmap::kRGB_565_Config, width, height);
			part_bitmap->allocPixels();
		}
		return part_bitmap;
	}
	SkBitmap* bitmap =NULL;
	SkBitmap* initSkBitmap(bool noPolygons)
	{
		return initBitmap(bitmap,TILE_SIZE*PART_WIDTH, TILE_SIZE*PART_WIDTH,noPolygons);
	}
	SkBitmap* part_bitmap =NULL;
	SkBitmap* initPartSkBitmap(bool noPolygons)
	{
		return initBitmap(part_bitmap,TILE_SIZE,TILE_SIZE,noPolygons);
	}
	SkCanvas* canvas = NULL;
	SkBitmap::Config saved_config=SkBitmap::kNo_Config;
	SkCanvas* initSkCanvas()
	{
		if (saved_config!=bitmap->getConfig())
		{
			delete_and_set_null(canvas);
			saved_config=bitmap->getConfig();
		}
		if (canvas==NULL)
		{
			canvas = new SkCanvas(*bitmap);
			canvas->translate(-TILE_SIZE*EDGE_WIDTH,-TILE_SIZE*EDGE_WIDTH);
			//canvas->translate(-TILE_SIZE*4,-TILE_SIZE*4);
		}
		return canvas;
	}
	SkImageEncoder* enc=NULL;
	SkImageEncoder* initSkImageEncoder()
	{
		if (enc == NULL)
		{
			enc = SkImageEncoder::Create(SkImageEncoder::kPNG_Type);
		}
		return enc;
	}
	void split(SkBitmap* all,const std::vector<std::string> &filenames,bool noPolygons)
	{
		SkBitmap *part=initPartSkBitmap(noPolygons);
		int part_width=(int)sqrt((double)filenames.size());
		assert(part_width*part_width==filenames.size());
		assert(part->height()==TILE_SIZE);
		assert(part->width()==TILE_SIZE);
		assert(all->width()==part_width*TILE_SIZE);
		assert(all->height()==part_width*TILE_SIZE);

		unsigned char *src=(unsigned char *)all->getPixels();
		size_t src_row_bytes=all->rowBytes();
		for (int j=0;j<part_width;j++)
		{
			for (int i=0;i<part_width;i++)
			{
				unsigned char *dest=(unsigned char *)part->getPixels();
				size_t dest_row_bytes=part->rowBytes();
				{
					unsigned char *p=src+j*src_row_bytes*part->height()+i*dest_row_bytes;
					for (int k=0;k<part->height();k++)
					{
						memcpy(dest,p,dest_row_bytes);
						dest+=dest_row_bytes;
						p+=src_row_bytes;
					}
					
				}
				SkImageEncoder* enc = initSkImageEncoder();
				if (enc != NULL && !enc->encodeFile(filenames[j*part_width+i].c_str(), *part, 100))
				{

				}

			}
		}
	}
	void runSimpleRendering2( string renderingFileName, string resourceDir, RenderingInfo* info,bool noPolygons) {
		SkColor defaultMapColor = SK_ColorLTGRAY;

		if (info->width > 10000 || info->height > 10000) {
			osmand_log_print(LOG_ERROR, "We don't rendering images more than 10000x10000 ");
			return;
		}

		osmand_log_print(LOG_INFO, "Rendering info bounds(%d, %d, %d, %d) zoom(%d), width/height(%d/%d) tilewidth/tileheight(%d/%d) fileName(%s)",
			info->left, info->top, info->right, info->bottom, info->zoom, info->width, info->height, info->tileWX, info->tileHY, info->tileFileName.c_str());
		RenderingRulesStorage* st=initRenderingRulesStorage();
		RenderingRuleSearchRequest* searchRequest =initRenderingRuleSearchRequest();
		if ((searchRequest->getIntPropertyValue(st->PROPS.R_NO_POLOGONS)==TRUE_VALUE)!=noPolygons)
		{
			searchRequest->setBooleanFilter(st->PROPS.R_NO_POLOGONS,noPolygons);
			searchRequest->saveState();
		}
		
		ResultPublisher* publisher = initResultPublisher();
		SearchQuery q(info->left, info->right,info->top, info->bottom, searchRequest, publisher);
		q.zoom = info->zoom;

		ResultPublisher* res = searchObjectsForRendering(&q, true,0, "Nothing found");
		osmand_log_print(LOG_INFO, "Found %d objects", res->result.size());

		SkBitmap* bitmap = initSkBitmap(noPolygons);

		osmand_log_print(LOG_INFO, "Initializing rendering style and rendering context");
		ElapsedTimer initObjects;
		initObjects.start();

		RenderingContext rc;
		rc.setDefaultIconsDir(resourceDir);
		searchRequest->clearState();
		searchRequest->setIntFilter(st->PROPS.R_MINZOOM, info->zoom);
		if (searchRequest->searchRenderingAttribute(A_DEFAULT_COLOR)) {
			defaultMapColor = searchRequest->getIntPropertyValue(searchRequest->props()->R_ATTR_COLOR_VALUE);
		}
		searchRequest->clearState();
		searchRequest->setIntFilter(st->PROPS.R_MINZOOM, info->zoom);
		if (searchRequest->searchRenderingAttribute(A_SHADOW_RENDERING)) {
			rc.setShadowRenderingMode(searchRequest->getIntPropertyValue(searchRequest->props()->R_ATTR_INT_VALUE));
			//rc.setShadowRenderingColor(searchRequest->getIntPropertyValue(searchRequest->props()->R_SHADOW_COLOR));
		}
		rc.setLocation(
			((double)info->left)/getPowZoom(31-info->zoom),
			((double)info->top)/getPowZoom(31-info->zoom)
			);
		rc.setDimension(info->width, info->height);
		rc.setZoom(info->zoom);
		rc.setRotate(0);
		rc.setDensityScale(1);
		osmand_log_print(LOG_INFO, "Rendering image");
		initObjects.pause();
		SkCanvas* canvas =initSkCanvas();
		if (SkColorGetA(defaultMapColor) == 0)
		{
			canvas->drawColor(defaultMapColor,SkXfermode::kClear_Mode);
		}
		else
			canvas->drawColor(defaultMapColor);
		doRendering(res->result, canvas, searchRequest, &rc);
		osmand_log_print(LOG_INFO, "End Rendering image");
		osmand_log_print(LOG_INFO, "Native ok (init %d, rendering %d) ", initObjects.getElapsedTime(),
			rc.nativeOperations.getElapsedTime());
		//SkImageEncoder* enc = initSkImageEncoder();
		//if (enc != NULL && !enc->encodeFile(info->tileFileName.c_str(), *bitmap, 100)) {
		//	osmand_log_print(LOG_ERROR, "FAIL to save tile to %s", info->tileFileName.c_str());
		//} else {
		//	osmand_log_print(LOG_INFO, "Tile successfully saved to %s", info->tileFileName.c_str());
		//}
		deleteObjects(res->result);
		return;
	}

	RenderingInfo *info;
	MKOsmMapFetcher::MKOsmMapFetcher(void)
	{
		input_file=MGBundle::mainBundle()->pathWithFileName("MapData/osm/data/zhejiang.obf");
		renderingFileName=MGBundle::mainBundle()->pathWithFileName("MapData/osm/conf/default.render.xml");
		imagesFileName=MGBundle::mainBundle()->pathWithFileName("MapData/osm/icons/");
		noPolygons=false;
	}


	MKOsmMapFetcher::~MKOsmMapFetcher(void)
	{
		closeBinaryMapFile(input_file);
	}

	static MKOsmMapFetcher *instance=NULL;
	static MGMutex locker;
	MKOsmMapFetcher* MKOsmMapFetcher::getInstance()
	{
		if (instance==NULL)
		{
			locker.lock();
			if (instance==NULL)
			{
				instance = new MKOsmMapFetcher();
			}
			locker.unlock();
		}
		return instance;
	}

	MGImage * MKOsmMapFetcher::getImage( const MKGlobalTileIndex &_index,MKMapType map_type/*=MKMapTypeOsm*/ )
	{
		AUTO_LOCK(&fetch_locker);

		string tmp_path=getCacheFolderPath(_index);
		if (!MGFileManager::defaultManager()->fileExistsAtPath(tmp_path))
		{
			loadMapData();

			GlobalMercator mecator;
			MGIntPoint index=mecator.GoogleTile(_index);
			index.x=(index.x>>PART_WIDTH_DIGIT)<<PART_WIDTH_DIGIT;
			index.y=(index.y>>PART_WIDTH_DIGIT)<<PART_WIDTH_DIGIT;
			info->zoom=_index.z;
			float tileWX=2*EDGE_WIDTH+PART_WIDTH;
			float tileHY=2*EDGE_WIDTH+PART_WIDTH;
			info->tileWX=tileWX;
			info->tileHY=tileHY;
			info->left=index.x<<(31-info->zoom);
			info->top=index.y<<(31-info->zoom);
			info->left-=EDGE_WIDTH* (1 << (31 - info->zoom));
			info->top-=EDGE_WIDTH* (1 << (31 - info->zoom));
			info->right=info->left+tileWX * (1 << (31 - info->zoom));
			info->bottom=info->top+tileHY * (1 << (31 - info->zoom));
			info->width = tileWX * TILE_SIZE;
			info->height = tileHY * TILE_SIZE;

			info->tileFileName=getCacheFolderPath(_index);
			initMapData(noPolygons);
			runSimpleRendering2(info->renderingFileName, info->imagesFileName, info,noPolygons);
			vector<string> file_names;
			{
				MKGlobalTileIndex index_all;
				index_all.x=index.x;
				index_all.y=((1<<_index.z) - 1) - index.y;
				index_all.z=_index.z;
				for (int i=0;i<PART_WIDTH;i++)
				{
					MKGlobalTileIndex index2=index_all;
					index2.y-=i;
					for (int j=0;j<PART_WIDTH;j++)
					{
						file_names.push_back(getCacheFolderPath(index2));
						//MGLogD("filename[%d] %s",i*PART_WIDTH+j,file_names[i*PART_WIDTH+j].c_str());
						index2.x++;
					}
				}

			}

			split(bitmap,file_names,noPolygons);
		}
		return MGImage::imageWithContentsOfFile(tmp_path);
	}
	std::string MKOsmMapFetcher::getCacheFolderPath( const MKGlobalTileIndex &index,MKMapType type/*=MKMapTypeOsm */ )
	{
		char filename[256];
		std::string quad=GlobalMercator::QuadTree(index);
		std::string tmp_path;
		if (type==MKMapTypeOsm)
		{
			if (noPolygons)
				tmp_path=MGBundle::mainBundle()->tmpPathWithFileName("osm_caches_no_polygons/");
			else
				tmp_path=MGBundle::mainBundle()->tmpPathWithFileName("osm_caches/");
		}

		sprintf(filename,"%d/",index.z);
		tmp_path+=filename;
		if (!Utility::isFileORDirExist(tmp_path))
			Utility::createDirOnPath(tmp_path);
		sprintf(filename,"%s.png",quad.c_str());
		tmp_path+=filename;
		return tmp_path;
	}

	bool MKOsmMapFetcher::initMapData( bool noPolygons/*=false*/ )
	{
		initSkBitmap(noPolygons);
		initPartSkBitmap(noPolygons);
		return true;
	}

	bool MKOsmMapFetcher::loadMapData()
	{
		if (info == NULL)
		{
			info = new RenderingInfo();
			info->imagesFileName = imagesFileName;
			info->renderingFileName=renderingFileName;

			BinaryMapFile* mf = initBinaryMapFile(input_file);
			osmand_log_print(LOG_INFO, "Init %d (success) binary map file %s.", mf->version,
				mf->inputName.c_str());
		}
		return true;
	}
}
