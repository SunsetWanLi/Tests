#include "SettingsAdapter.h"
#include "DataReader.h"
#include "DataWriter.h"

namespace GIS_Embedded
{
	int SettingsAdapter::translate(byte_8 *data, int size, Settings ** settings)
	{
		//DataReader data_reader;
		//data_rreader.setStartPointer(data);
		*settings = new Settings();
		memcpy((void *)(*settings), (void *)data, size);

		return 0;
	}

	uint_32 SettingsAdapter::packSize(Settings &settings)
	{
		return sizeof(Settings);
	}

	int SettingsAdapter::pack(Settings &settings, byte_8 **data, int *size)
	{
		*size = sizeof(Settings);
		*data = new byte_8[*size];
		memcpy(*data, &settings, *size);
		return 0;
	}

}

/*
#include "stdafx.h"

#include "SettingsAdapter.h"
#include "DataReader.h"
#include "DataWriter.h"

namespace GIS_Embedded
{

	int SettingsAdapter::translate(byte_8 *data, int size, Settings * map_config)
	{
		DataReader data_reader;
		data_reader.setStartPointer(data);

		uint_32 data_size;

		map_config->alert = (bool)data_reader.getByte();

		map_config->auto_data_update = (bool)data_reader.getByte();

		map_config->auto_reCal = (bool)data_reader.getByte();

		map_config->by_car = (bool)data_reader.getByte();

		data_size = data_reader.getByte();
		for(int i = 0; i < data_size; i ++)
		{
			map_config->cross_line_layers.push_back(data_reader.getString());
		}

		data_size = data_reader.getByte();
		for(int i = 0; i < data_size; i ++)
		{
			map_config->cross_polygon_layers.push_back(data_reader.getString());
		}

		map_config->cross_scope_width = data_reader.getDouble();

		map_config->home_addr.d0 = data_reader.getDouble();
		map_config->home_addr.d1 = data_reader.getDouble();

		map_config->initial_scale = data_reader.getInt();

		map_config->language_type = data_reader.getInt();

		map_config->local_pos.d0 = data_reader.getDouble();
		map_config->local_pos.d1 = data_reader.getDouble();

		map_config->navi_scale = data_reader.getInt();

		map_config->night_color = data_reader.getByte();

		map_config->on_foot = data_reader.getByte();

		map_config->scale_pixel_width = data_reader.getInt();

		data_size = data_reader.getByte();
		for(int i = 0; i < data_size; i ++)
		{
			map_config->scale_ranks.push_back(data_reader.getInt());
		}

		map_config->street_label = data_reader.getByte();

		map_config->unit_type = data_reader.getInt();

		return 0;
	}

	uint_32 SettingsAdapter::packSize(Settings &map_config)
	{
		uint_32 size = 0;
		
		size += sizeof(byte_8); //map_config.alert
		size += sizeof(byte_8); //map_config.auto_data_update
		size += sizeof(byte_8); //map_config.auto_reCal
		size += sizeof(byte_8); //map_config.by_car

		//map_config.cross_line_layers
		size += sizeof(byte_8);
		for(int i = 0; i < map_config.cross_line_layers.size(); i ++)
		{
			size += sizeof(int);
			size += map_config.cross_line_layers[i].size() * sizeof(wchar_t);
		}

		//map_config.cross_polygon_layers
		size += sizeof(byte_8);
		for(int i = 0; i < map_config.cross_polygon_layers.size(); i ++)
		{
			size += sizeof(int);
			size += map_config.cross_polygon_layers[i].size() * sizeof(wchar_t);
		}

		size += sizeof(double); //map_config.cross_scope_width

		size += 2 * sizeof(double); //map_config.home_addr

		size += sizeof(int); //map_config.initial_scale

		size += sizeof(int); //map_config.language_type

		size += 2 * sizeof(double); //map_config.local_pos

		size += sizeof(int); //map_config.navi_scale

		size += sizeof(byte_8); //map_config.night_color

		size += sizeof(byte_8); //map_config.on_foot

		size += sizeof(int); //map_config.scale_pixel_width

		//map_config.scale_ranks
		size += sizeof(byte_8);
		size += map_config.scale_ranks.size() * sizeof(int);

		size += sizeof(byte_8); //map_config.street_label

		size += sizeof(int); //map_config.unit_type

		return size;
	}

	int SettingsAdapter::pack(Settings &map_config, byte_8 **data, int *size)
	{
		*size = packSize(map_config);
		*data = new byte_8[*size];

		DataWriter data_writer;
		data_writer.setStartPointer(*data);
		
		uint_32 data_size;

		data_writer.writeByte(map_config.alert);

		data_writer.writeByte(map_config.auto_data_update);

		data_writer.writeByte(map_config.auto_reCal);

		data_writer.writeByte(map_config.by_car);

		data_size = map_config.cross_line_layers.size();
		data_writer.writeByte(data_size);
		for(int i = 0; i < data_size; i ++)
		{
			data_writer.writeString(map_config.cross_line_layers[i]);
		}

		data_size = map_config.cross_line_layers.size();
		data_writer.writeByte(data_size);
		for(int i = 0; i < data_size; i ++)
		{
			data_writer.writeString(map_config.cross_line_layers[i]);
		}


		data_writer.writeDouble(map_config.cross_scope_width);

		data_writer.writeDouble(map_config.home_addr.d0);
		data_writer.writeDouble(map_config.home_addr.d1);

		data_writer.writeInt(map_config.initial_scale);

		data_writer.writeInt(map_config.language_type);

		data_writer.writeDouble(map_config.local_pos.d0);
		data_writer.writeDouble(map_config.local_pos.d1);

		data_writer.writeInt(map_config.navi_scale);

		data_writer.writeByte(map_config.night_color);

		data_writer.writeByte(map_config.on_foot);

		data_writer.writeInt(map_config.scale_pixel_width);

		data_size = map_config.scale_ranks.size();
		data_writer.writeByte(data_size);
		for(int i = 0; i < data_size; i ++)
		{
			data_writer.writeInt(map_config.scale_ranks[i]);
		}

		data_writer.writeByte(map_config.street_label);

		data_writer.writeInt(map_config.unit_type);

		return 0;
	}

}

*/