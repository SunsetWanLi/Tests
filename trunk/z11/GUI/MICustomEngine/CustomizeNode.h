#pragma  once
#include <string>
#include <map>
#include "../../CommonBase/MGStructure.h"
#include "../../GraphicCore/GraphicStructure.h"
#include "../../GraphicCore/MGColor.h"
#include "../../ExternalLib/xml/rapidxml.hpp"
#include "../../ExternalLib/xml/rapidxml_utils.hpp"
#include "../../ExternalLib/xml/rapidxml_print.hpp"
#include "../../System/Device/MGDevice.h"

namespace z11
{
	class MIView ;
	class MIScrollView;
	class MILabel;
	class MIViewController;
	class MKMapViewDelegate;
	class MITableViewDataSource;
	class MITableViewDelegate;
	class MIPickerViewDataSource;
	class MIPickerViewDelegate;
	class CustomizeView;
 	class CustomizeDocument;
 	class CustomizeNode;
 	class CustomizeRootNode;
 	class CustomizeMapNode;
 	class CustomizeBaseTextViewNode;
 	class CustomizeMIButtonNode;
 	class CustomizeMILabelNode;
 	class CustomizeMITextFieldNode;
 	class CustomizeMITextViewNode;

	typedef std::map<std::string,CustomizeNode*> NodeMap;
	typedef NodeMap::iterator MyNodeIt;

	typedef std::map<std::string,RGBAColorRef> ColorMap;
	typedef ColorMap::iterator ColorIt;

	typedef std::map<std::string,MIView *>ViewMap;
	typedef ViewMap::iterator ViewIt;

	typedef vector<CustomizeNode*>::iterator CustomizeNodeIt;

	enum ProportionPriority{
		NONE=0,
		WIDTHFIRST,
		HEIGHTFIRST
	};
	class CustomizeNode
	{
	public:
		std::string m_type;
		std::string m_id;
		MGRect m_rect;
		std::string m_background;
		std::string m_is_hide;
		MIViewController *_layoutController;
		CustomizeDocument *m_layout_docment;
		string user_interaction;
		ProportionPriority priority;
		CustomizeNode(const std::string& type,const std::string& id,const MGRect& rect,const std::string& background):m_type(type),m_id(id),m_rect(rect),m_background(background){}
		CustomizeNode(){m_parent=NULL; m_is_hide="true";user_interaction="true";priority=NONE;}
		static CustomizeNode *loadXML (rapidxml::xml_node<>* node,CustomizeDocument *layout,MIViewController *controller=NULL);
        virtual ~CustomizeNode(){}
		virtual CustomizeNode *parent();
		void addSubLayoutNode(CustomizeNode *_node);
		vector<CustomizeNode*> subCustomizeNode;
		CustomizeNode *m_parent;
		virtual MIView *create();
	protected:
		
		virtual void load(rapidxml::xml_node<>* node);
		virtual MIView *newNode();
		void setBackgroundColor(MIView *ret);
		
	};

	class CustomizeRootNode:public CustomizeNode
	{
	public :
		static CustomizeRootNode* loadXML(rapidxml::xml_node<>* node,CustomizeDocument *doc,MIViewController *controller=NULL);
		virtual MIView *create(MIView *_view);
		CustomizeView* initCustomizeView(MGRect rect=MGRectMake(0.0,0.0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight()));
		CustomizeRootNode();
	protected:
		void load(rapidxml::xml_node<>* node,MIView *view=NULL);
		
	};
	class CustomizeMapNode:public CustomizeNode
	{
	public :
		string simulate_gps_data;
		bool show_user_location;
		string map_type;
		string center_coordinate;
		static CustomizeMapNode* loadXML(rapidxml::xml_node<>* node,CustomizeDocument *doc,MIViewController *controller=NULL);
		virtual MIView *create(MIView *_view);
		CustomizeMapNode();
	protected:
		MKMapViewDelegate *map_delegate;

	};

	class CustomizeBaseTextViewNode : public CustomizeNode
	{
	public:
		virtual ~CustomizeBaseTextViewNode()
		{}
		std::string		m_text;
		float			m_font_size;
		std::string		m_font_color;
		std::string		m_text_alignment;
		std::string keboard_type;
		CustomizeBaseTextViewNode():m_font_size(-1){}
		virtual void load(rapidxml::xml_node<>* node);
		void setTextAlignment(MIView* view, string _text_alignment);
	protected:
		virtual void initBaseTextWithView(MIView* view);
	};

	 //typedef enum MITextViewDisplayMode;
	class CustomizeMITextViewNode:public CustomizeBaseTextViewNode
	{
	public :
		bool editable;
		std::string display_mode; 
		CustomizeMITextViewNode(){}
		virtual void load(rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();
	};

	class CustomizeMITextFieldNode:public CustomizeBaseTextViewNode
	{
	public :
		std::string		m_place_holder;
		CustomizeMITextFieldNode(){}
		virtual void load(rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();

	};


	class CustomizeMILabelNode:public CustomizeBaseTextViewNode
	{
	public:
		CustomizeMILabelNode(){}
		virtual void load(rapidxml::xml_node<>* node);
		static void setTextAlignment(MILabel* view, string _text_alignment);
	protected:
		virtual MIView *newNode();
	};

	class CustomizeMIButtonNode:public CustomizeNode
	{
	public:
		string m_button_type;
		string m_label_text;
		float		m_label_font_size;
		std::string	m_label_font_color;
		std::string m_label_text_alignment;
		CustomizeMIButtonNode():m_label_font_size(-1){}
		virtual void load(rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();
	};

	class CustomizeMIViewNode:public CustomizeNode
	{
	public:
		CustomizeMIViewNode(){is_from_file=false;}
		~CustomizeMIViewNode(){}
		bool is_from_file;
		MGRect sub_rect;
		string file_name;
		string is_hide;
		string cur_view;
		CustomizeDocument *doc;
		virtual void load(rapidxml::xml_node<>* node);
		virtual MIView* create();
	};

	class CustomizeMIScrollViewNode:public CustomizeNode
	{
	public:
		CustomizeMIScrollViewNode();
		byte_8 vertical_scroll_enabled;
		byte_8 vertical_bounce_enabled;
		byte_8 vertical_bouncing;
		byte_8 horizontal_scroll_enabled;
		byte_8 horizontal_bounce_enabled;
		byte_8 horizontal_bouncing;
		byte_8 zoom_enable;
		byte_8 zoom_bouncing;
		byte_8 always_bounce_horizontal;
		byte_8 always_bounce_vertical;
		//scroll indicator//
		byte_8 vertical_indicator_show;
		byte_8 horizontal_indicator_show;
		//scroll inertia//
		byte_8 inertia_enable;
		byte_8 decelarate_enable;
		// passing direction scroll //
		byte_8 passing_vertical_scroll_enable;
		byte_8 passing_horizontal_scroll_enable;
		// keyboard displaying //
		byte_8 hide_keyboard_when_scrolling;

		virtual void load(rapidxml::xml_node<>* node);
		virtual MIView* create();
	};

	class CustomizeSegmentedControlNode:public CustomizeNode
	{
	public :
		std::string m_items;
		virtual void load(rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();
	};
	
	class CustomizeTableViewNode:public CustomizeNode
	{
	public :
		CustomizeTableViewNode(){table_delegate=NULL;data_source=NULL;}
		MITableViewDelegate *table_delegate;
		MITableViewDataSource *data_source;
		std::string separator_style;
		virtual void load(rapidxml::xml_node<>* node);
	protected:
		virtual MIView* create();
	};

	class CustomizePickerViewNode:public CustomizeNode
	{
	public :
		CustomizePickerViewNode(){picker_delegate=NULL;data_source=NULL;}
		MIPickerViewDelegate *picker_delegate;
		MIPickerViewDataSource *data_source;
		virtual void load(rapidxml::xml_node<>* node);
	protected:
		virtual MIView* create();
	};
	typedef uint_32 MIViewAnimationOptions;
	
}