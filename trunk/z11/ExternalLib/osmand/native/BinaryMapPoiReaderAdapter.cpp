#include "BinaryMapPoiReaderAdapter.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "proto/osmand_odb.pb.h"
#include "proto/osmand_index.pb.h"
#include "osmand_log.h"
#include "../utils/CodedInputStreamInstance.h"
#undef min
#undef max
#include "Amenity.h"
#include "SearchRequest.h"

using namespace std;
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::FileInputStream;

extern std::map< std::string, BinaryMapFile* > openFiles;
extern bool skipUnknownFields(CodedInputStream* input, int tag) ;
extern bool readInt(CodedInputStream* input, uint32_t* sz );

extern int WireFormatLiteReadPrimitiveInt32(CodedInputStream* input);
extern uint32_t WireFormatLiteReadPrimitiveUInt32(CodedInputStream* input);
extern uint32_t WireFormatLiteGetTagFieldNumber(uint32_t tag);
extern bool WireFormatLiteReadString(CodedInputStream* input,std::string *value);
uint32_t readUInt32(CodedInputStream* input)
{
	return WireFormatLiteReadPrimitiveUInt32(input);
}
int readSInt32(CodedInputStream* input)
{
	return WireFormatLiteReadPrimitiveInt32(input);
}
int readRawVarint32(CodedInputStream* input)
{
	int ret=0;
	ret=WireFormatLiteReadPrimitiveInt32(input);
	return ret;
}
static const  int ZOOM_TO_SKIP_FILTER = 3;
typedef bool boolean;
typedef UNORDERED(map)<int ,long> TIntLongHashMap;
typedef UNORDERED(set)<long> TLongHashSet;
void readPoiBoundariesIndex(CodedInputStream *input,PoiRegion *region)
{
	uint32_t latlon=0;
	uint32_t tag;
	while ((tag = input->ReadTag()) != 0) {
		switch (WireFormatLiteGetTagFieldNumber(tag)) 
		{
		case OsmAndTileBox::kLeftFieldNumber:
			{
				latlon=WireFormatLiteReadPrimitiveUInt32(input);
				region->leftLongitude=get31LongitudeX(latlon);
				break;
			}
		case OsmAndTileBox::kRightFieldNumber:
			{
				latlon=WireFormatLiteReadPrimitiveUInt32(input);
				region->rightLongitude=get31LongitudeX(latlon);
				break;
			}
		case OsmAndTileBox::kTopFieldNumber:
			{
				latlon=WireFormatLiteReadPrimitiveUInt32(input);
				region->topLatitude=get31LatitudeY(latlon);
				break;
			}
		case OsmAndTileBox::kBottomFieldNumber:
			{
				latlon=WireFormatLiteReadPrimitiveUInt32(input);
				region->bottomLatitude=get31LatitudeY(latlon);
				break; 
			}
		default:
			skipUnknownFields(input,tag);
			break;
		}
	}
}
bool readPoiIndex(CodedInputStream *input,PoiRegion *region,bool readCategories)
{
	uint32_t tag;
	uint32_t defaultId = 1;
	while ((tag = input->ReadTag()) != 0) {
		switch (WireFormatLiteGetTagFieldNumber(tag)) 
		{
		case OsmAndPoiIndex::kNameFieldNumber:{
			DO_((WireFormatLiteReadString(input, &region->name)));
			break;
			}
		case OsmAndPoiIndex::kBoundariesFieldNumber:{
			int len=0;
			len=WireFormatLiteReadPrimitiveInt32(input);
			int oldLimit = input->PushLimit(len);
			readPoiBoundariesIndex(input, region);
			input->PopLimit(oldLimit);
			break;
			}
		case OsmAndPoiIndex::kCategoriesTableFieldNumber:
			{
				if (!readCategories)
				{
					input->Seek(region->filePointer+region->length);
					return true;
				}

				break;
			}
		case OsmAndPoiIndex::kBoxesFieldNumber:
			//input->Skip(input->getBy
			input->Seek(region->filePointer+region->length);
			return true;
			break;
		default:
			skipUnknownFields(input,tag);
			break;
		}
	}

	return true;
}

bool  readBoxField(CodedInputStream *input,int left31, int right31, int top31, int bottom31,
	int px, int py, int pzoom, TIntLongHashMap& offsetsMap, TLongHashSet* skipTiles, SearchRequest<Amenity>& req, PoiRegion* region) 
{
	req.numberOfReadSubtrees++;
	int zoomToSkip = req.zoom + ZOOM_TO_SKIP_FILTER;
	boolean checkBox = true;
	boolean existsCategories = false;
	int zoom = pzoom;
	int dy = py;
	int dx = px;
	uint32_t tag;
	while((tag = input->ReadTag()) != 0){
		if(req.isCancelled()){
			return false;
		}
		switch (WireFormatLiteGetTagFieldNumber(tag))  
		{
		case OsmAndPoiBox::kZoomFieldNumber:
			zoom = readUInt32(input) + pzoom;
			break;
		case OsmAndPoiBox::kLeftFieldNumber:
			dx = readSInt32(input);
			break;
		case OsmAndPoiBox::kTopFieldNumber:
			dy = readSInt32(input);
			break;
		case OsmAndPoiBox::kCategoriesFieldNumber:
			/*			if(req.poiTypeFilter == null){
			skipUnknownField(t);
			} else*/ 
			{
				int length = readRawVarint32(input);
				int oldLimit =  input->PushLimit(length);
				//boolean check = checkCategories(req, region);
				input->Skip(length);
				input->PopLimit(oldLimit);
				//if(!check){
				//	codedIS.skipRawBytes(codedIS.getBytesUntilLimit());
				//	return false;
				//}
				existsCategories = true;
			}
			break;

		case OsmAndPoiBox::kSubBoxesFieldNumber:
			{
				int x = dx + (px << (zoom - pzoom));
				int y = dy + (py << (zoom - pzoom));
				checkBox=false;
				if(checkBox){
					unsigned int xL = x << (31 - zoom);
					unsigned int xR = (x + 1) << (31 - zoom);
					unsigned int yT = y << (31 - zoom);
					unsigned int yB = (y + 1) << (31 - zoom);
					// check intersection
					if(left31 > xR || xL > right31 || bottom31 < yT || yB < top31){
						input->Skip(input->BytesUntilLimit());
						return false;
					}
					req.numberOfAcceptedSubtrees++;
					checkBox = false;
				}

				int length = readSInt32(input);
				int oldLimit = input->PushLimit(length);
				boolean exists = readBoxField(input,left31, right31, top31, bottom31, x, y, zoom, offsetsMap, skipTiles, req, region);
				input->PopLimit(oldLimit);

				if (skipTiles != NULL && zoom >= zoomToSkip && exists) {
					long val = ((((long) x) >> (zoom - zoomToSkip)) << zoomToSkip) | (((long) y) >> (zoom - zoomToSkip));
					if(skipTiles->find(val)!=skipTiles->end()){
						input->Skip(input->BytesUntilLimit());
						return true;
					}
				}
			} 
			break;
		case OsmAndPoiBox::kShiftToDataFieldNumber:
			{
				int x = dx + (px << (zoom - pzoom));
				int y = dy + (py << (zoom - pzoom));
				long l = ((((x << zoom) | y) << 5) | zoom);
				offsetsMap[readSInt32(input)]=l;
				if(skipTiles != NULL && zoom >= zoomToSkip){
					long val = ((((long) x) >> (zoom - zoomToSkip)) << zoomToSkip) | (((long) y) >> (zoom - zoomToSkip));
					skipTiles->insert(val);
				}
			}	break;
		default:
			skipUnknownFields(input,tag);
			break;
		}
	}
	return existsCategories;
}

void searchPoiIndex(CodedInputStream *input,int left31, int right31, int top31, int bottom31,SearchRequest<Amenity>& req,PoiRegion *region)
{
	int indexOffset=region->filePointer;
	int tag=0;
	uint32_t length=0;
	int oldLimit =0;
	int zoomToSkip = 31;
	TIntLongHashMap offsetsMap;
	TLongHashSet* skipTiles=NULL;
	while ((tag = input->ReadTag()) != 0) {
		switch (WireFormatLiteGetTagFieldNumber(tag) )
		{
		case OsmAndPoiIndex::kBoxesFieldNumber:
			readInt(input,&length);
			oldLimit = input->PushLimit(length);
			readBoxField(input,left31, right31, top31, bottom31, 0, 0, 0, offsetsMap,  skipTiles, req, region);
			input->PopLimit(oldLimit);
			break;
		case OsmAndPoiIndex::kPoiDataFieldNumber:
			{
				for (TIntLongHashMap::iterator it=offsetsMap.begin();it!=offsetsMap.end();it++)
				{
					input->Seek(indexOffset  + it->first);
					uint32_t len = 0 ;
					readInt(input,&len);
					int oldLimit = input->PushLimit(len);
					//readPoiData(left31, right31, top31, bottom31, req, region, skipTiles, zoomToSkip);
					input->PopLimit(oldLimit);
				}
				input->Seek(indexOffset +length);
				return ;	
			}

		default:
			skipUnknownFields(input,tag);
			break;
		}
	}

}

vector<Amenity> searchPoi(SearchRequest<Amenity> &req)
{
	map<std::string, BinaryMapFile*>::iterator i = openFiles.begin();
	for (; i != openFiles.end() && true; i++) {
		BinaryMapFile* file = i->second;
		if (file!=NULL)
		{
			vector<PoiRegion*>::iterator poiRegion=file->poiIndexes.begin();
			for (;poiRegion!=file->poiIndexes.end();poiRegion++)
			{

				SHARED_PTR<CodedInputStream> cisi=CodedInputStreamInstance::getInstance(file->routefd);
				CodedInputStream& cis=*cisi.get();
				cis.Seek((*poiRegion)->filePointer);
				uint32_t old = cis.PushLimit((*poiRegion)->length);
				searchPoiIndex(&cis,get31TileNumberX(120),get31TileNumberX(121),get31TileNumberY(31),get31TileNumberY(30),req,*poiRegion);
				cis.PopLimit(old);
			}

		}

	}
	return std::vector<Amenity>();
}