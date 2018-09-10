#include "CustomizeNode.h"
#include "CustomizeDocument.h"
#include "../MIControls/MIBaseTextView.h"
#include "../MIControls/MIButton.h"
#include "../MIControls/MITableViewCellAccessoryButton.h"
#include "../MIControls/MILabel.h"
#include "../MIControls/MISwitch.h"
#include "../MIControls/MIPictureView.h"
#include "../MIControls/MIScrollView.h"
#include "../MIControls/MIImageView.h"
#include "../MIControls/MITableView.h"
#include "../MIControls/MITableViewCell.h"
#include "../MIControls/MITextField.h"
#include "../MIControls/MITextButton.h"
#include "../MIControls/MIEditableTextView.h"
#include "../MIControls/MIScrollView.h"
#include "../MIControls/MISegmentedControl.h"
#include "../MIControls/MITextView.h"
#include "../MIControls/MITemplateTableCell.h"
#include "../MIControls/MITemplatePickerDelegate.h"
#include "../MIControls/MIPickerView.h"
#include "../MIControls/MISlide.h"
#include <iostream>
#include "../../System/Device/MGDevice.h"
#include "../../System/Shared/MGLog.h"
#include "../../MapKit/Views/MapViews/MKMapView.h"
#include "../MILayoutEngine/MILayoutEngine.h"

using namespace rapidxml;
using namespace std;
namespace z11{
	extern int z_base;
}

namespace z11
{
	void resize_font_custom(float &a)
	{
		if (a<1&&a>0)
		{
			a*=MGDevice::getScreenWidth();
		}

	}
	void scale_rect(MGRect &rect,CustomizeDocument *doc,ProportionPriority priority=NONE)
	{
		float width= doc->root_rect.width;
		float height = doc->root_rect.height;
		float w_per_h = width/height ;
		float wh = doc->width/doc->height ;
	
		if (rect.x<5)
		{
			rect.x*=width;
		}
		if (rect.y<5)
		{
			rect.y*=height;
		}

		if(w_per_h!=wh){
			if(priority==WIDTHFIRST){
				height = width/wh;
			}
			else if(priority==HEIGHTFIRST){
				width = height*wh;
			}
		}
		
		if (rect.width<5)
		{
			rect.width*=width;
		}
		if (rect.height<5)
		{
			rect.height*=height;
		}	
	}

	CustomizeNode * CustomizeNode::loadXML( xml_node<>* node,CustomizeDocument *layout,MIViewController *controller)
	{
		CustomizeNode *ret=NULL;
		
		string type=node->name();
		if (type == "MITextField")
		{
			ret=new CustomizeMITextFieldNode();
		}
		else if (type=="MILabel")
		{
			ret=new CustomizeMILabelNode();
		}
		else if(type=="MITextView"||type=="MITextArea")
		{
			ret=new CustomizeMITextViewNode();
		}
		else if(type=="MIButton")
		{
			ret=new CustomizeMIButtonNode();
		}
		else if(type=="MISubView"||type=="MIView")
		{
			ret=new CustomizeMIViewNode();
		}
		else if(type=="MIScrollView")
		{
			ret=new CustomizeMIScrollViewNode();
		}
		else if(type=="MISegmentedControl")
		{
			ret=new CustomizeSegmentedControlNode();
		}
		else if(type=="MITableView")
		{
			ret=new CustomizeTableViewNode();
		}
		else if(type=="MIPickerView")
		{
			ret=new CustomizePickerViewNode();
		}
		else
			ret=new CustomizeNode();

		ret->m_layout_docment=layout;
		ret->_layoutController=controller;
		ret->load(node);
		
		return ret;
	}
	
	MIView * CustomizeNode::create()
	{
		MIView *ret=newNode();
		if (ret!=NULL)
		{
			setBackgroundColor(ret);
			scale_rect(m_rect,m_layout_docment,priority);
			ret->initWithFrame(m_rect);
			if(m_is_hide=="false")
				ret->hide();
			else
				ret->show();
			if(user_interaction=="false")
				ret->_view_flags.user_interaction_enabled=0;
			else
				ret->_view_flags.user_interaction_enabled=1;

		}
		return ret;
	}

	MIView * CustomizeNode::newNode()
	{
		MIView *ret=NULL;
		if(m_type == "MIPictureView")
		{
			ret=new MIPictureView();
		}
		else if(m_type == "MISwitch")
		{
			ret=new MISwitch();
		}
		else if(m_type == "MIImageView")
		{
			ret=new MIImageView();
		}
		else if(m_type == "MISlide")
		{
			ret=new MISlide();
		}
		return ret;
	}
	CustomizeNode *CustomizeNode::parent()
	{
		return m_parent;
	}
	void CustomizeNode::setBackgroundColor( MIView *ret )
	{
		MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_background);
		if (color!=NULL)
		{
			ret->setBackgroundColor(color);
		}
	}

	void CustomizeNode::load( rapidxml::xml_node<>* node )
	{
		CustomizeNode *ret=this;
		assert(node->name()!="");
		ret->m_type=node->name();
		assert(node->first_node("id"));
		ret->m_id=node->first_node("id")->value();

		xml_node<>* u_node=node->first_node("user_interaction");
		if(u_node!=NULL){
			user_interaction=u_node->value();
		}
		if (xml_node<> *_node=node->first_node("priority"))
		{
			string w=_node->value();
			if(w=="height"){
				ret->priority=HEIGHTFIRST;
			}
			else if(w=="width"){
				ret->priority=WIDTHFIRST;
			}
			else
				ret->priority=NONE;
		}
		xml_node<>*position_node = node->first_node("css");
		if(position_node){
			sscanf(position_node->first_node("left")->value(),"%f",&ret->m_rect.x);
			sscanf(position_node->first_node("top")->value(),"%f",&ret->m_rect.y);
			sscanf(position_node->first_node("width")->value(),"%f",&ret->m_rect.width);
			sscanf(position_node->first_node("height")->value(),"%f",&ret->m_rect.height);
			if(position_node->first_node("display"))
				ret->m_is_hide = position_node->first_node("display")->value();
			if (xml_node<> *_node=position_node->first_node("background"))
			{
				string bg=_node->value();
				string::size_type pos1=bg.find("@drawable/");
				if(pos1!=string::npos);
				else{
					bg="@drawable/"+bg;
				}
				ret->m_background=bg;
			}
		}
		else;
			//ret->m_rect=MGRectMake(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
		

	}

	void CustomizeNode::addSubLayoutNode(CustomizeNode *_node)
	{
		subCustomizeNode.push_back(_node);
		_node->m_parent=this;
	}

	CustomizeRootNode::CustomizeRootNode()
	{
		m_layout_docment=NULL;
	}
	void CustomizeRootNode::load(rapidxml::xml_node<>* node,MIView *view)
	{

		if (xml_node<> *_node=node->first_node("background"))
		{
			string bg=_node->value();
			string::size_type pos1=bg.find("@drawable/");
			MGColor *color=NULL;
			if(pos1!=string::npos){
				color=MILayoutEngine::sharedEngine().findColorById(bg);
			}
			else{
				color=MILayoutEngine::sharedEngine().findColorById("@drawable/"+bg);
			}
			
			if (color!=NULL)
			{
				view->setBackgroundColor(color);
			}
		}
				
	}

    CustomizeRootNode* CustomizeRootNode::loadXML(rapidxml::xml_node<>* root,CustomizeDocument *doc,MIViewController *controller)
    {
        CustomizeRootNode *ret=new CustomizeRootNode();
		if (xml_node<> *_node=root->first_node("id"))
		{
			ret->m_id=_node->value();
		}
		if (xml_node<> *_node=root->first_node("background"))
		{
			string bg=_node->value();
			string::size_type pos1=bg.find("@drawable/");
			if(pos1!=string::npos);
			else{
				bg="@drawable/"+bg;
			}
			ret->m_background=bg;
		}
		if (xml_node<> *_node=root->first_node("width"))
		{
			string w=_node->value();
			doc->width=Utility::BuiltInTypeFromString<float>(w);
		}
		if (xml_node<> *_node=root->first_node("height"))
		{
			string w=_node->value();
			doc->height=Utility::BuiltInTypeFromString<float>(w);
		}
		if(root->first_node("display"))
			ret->m_is_hide = root->first_node("display")->value();
		ret->m_layout_docment=doc;

        xml_node<>* node = root->first_node("children");
		for (;node!=NULL;node=node->next_sibling("children"))
		{
			xml_node<>*child_node = node->first_node();
			for (;child_node!=NULL;child_node=child_node->next_sibling()){
				CustomizeNode *layout_node=CustomizeNode::loadXML(child_node,doc,controller);
				doc->addNodeById(layout_node->m_id,layout_node);
				ret->subCustomizeNode.push_back(layout_node);
			}
		}
		doc->root_view_name=ret->m_id;
		return ret;
    }

	MIView * CustomizeRootNode::create( MIView *_view )
	{
		if (_view==NULL)
		{
			return NULL;
		}
		_view->setFrame(m_layout_docment->root_rect);
		if (m_background!="")
		{
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_background);
			if (color!=NULL)
			{
				_view->setBackgroundColor(color);
			}
		}

		vector<CustomizeNode*>::iterator it = subCustomizeNode.begin();
		for (;it!=subCustomizeNode.end();it++)
		{
			MIView *view=(*it)->create();
			if (view!=NULL)
			{
				m_layout_docment->addViewById((*it)->m_id,view);
				_view->addSubview(view);
			}
		}
		m_layout_docment->addViewById(m_id,_view);
		return _view;
	}
	void CustomizeBaseTextViewNode::initBaseTextWithView(MIView* view)
	{
		if(m_type == "MITextView")
		{
			MITextView* _view = static_cast<MITextView*>(view);
			string type="@data/";
			if (m_text.find(type)!=string::npos)
			{
				/*	MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
				_view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);*/
			}
			else
			{
				_view->setText(m_text);
			}
			if(m_font_size>0)
			{
				_view->getFont()->setFaceSize(m_font_size);
			}
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_font_color);
			if (color!=NULL)
			{
				_view->setTextColor(color->m_color);
				delete_and_set_null(color);
			}
			if (m_text_alignment != "")
			{
				setTextAlignment(_view, m_text_alignment);
			}
		}
		else if(m_type == "MITextField")
		{
			MITextField* _view = static_cast<MITextField*>(view);
			string type="@data/";
			if (m_text.find(type)!=string::npos)
			{
				/*	MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
				_view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);*/
			}
			else
			{
				_view->setText(m_text);
			}
			if(m_font_size>0)
			{
				_view->getFont()->setFaceSize(m_font_size);
			}
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_font_color);
			if (color!=NULL)
			{
				_view->setTextColor(color->m_color);
				delete_and_set_null(color);
			}
			if (m_text_alignment != "")
			{
				setTextAlignment(_view, m_text_alignment);
			}
		}
		else if(m_type== "MILabel")
		{
			MILabel* _view = static_cast<MILabel*>(view);
			string type="@data/";
			if (m_text.find(type)!=string::npos)
			{
				/*		MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
				_view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);*/
			}
			else
			{
				_view->setText(m_text);
			}
			if(m_font_size>0)
			{
				_view->getFont()->setFaceSize(m_font_size);
			}
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_font_color);
			if (color!=NULL)
			{
				_view->getFont()->setColor(color->m_color);
				delete_and_set_null(color);
			}
			if (m_text_alignment != "")
			{
				setTextAlignment(_view, m_text_alignment);
			}
		}
	}
	void CustomizeBaseTextViewNode::setTextAlignment( MIView *view,string _text_alignment )
	{
		MITextAlignment alignment;
		if (_text_alignment == "LeftTop")
			alignment=MITextAlignmentLeftTop;
		else if(_text_alignment == "LeftMiddle")
			alignment=MITextAlignmentLeftMiddle;
		else if(_text_alignment == "LeftBottom")
			alignment=MITextAlignmentLeftBottom;
		else if(_text_alignment == "CenterTop")
			alignment=MITextAlignmentCenterTop;
		else if(_text_alignment == "CenterMiddle")
			alignment=MITextAlignmentCenterMiddle;
		else if(_text_alignment == "CenterBottom")
			alignment=MITextAlignmentCenterBottom;
		else if(_text_alignment == "RightTop")
			alignment=MITextAlignmentRightTop;
		else if(_text_alignment == "RightMiddle")
			alignment=MITextAlignmentRightMiddle;
		else if(_text_alignment == "RightBottom")
			alignment=MITextAlignmentRightBottom;

		MIKeyboardType keyboardtype;
		if(keboard_type=="ASCIICapable")
			keyboardtype=MIKeyboardTypeASCIICapable;
		else if(keboard_type=="NumbersAndPunctuation")
			keyboardtype=MIKeyboardTypeNumbersAndPunctuation;
		else if(keboard_type=="URL")
			keyboardtype=MIKeyboardTypeURL;
		else if(keboard_type=="NumberPad")
			keyboardtype=MIKeyboardTypeNumberPad;
		else if(keboard_type=="PhonePad")
			keyboardtype=MIKeyboardTypePhonePad;
		else if(keboard_type=="NamePhonePad")
			keyboardtype=MIKeyboardTypeNamePhonePad;
		else if(keboard_type=="EmailAddress")
			keyboardtype=MIKeyboardTypeEmailAddress;
		else if(keboard_type=="DecimalPad")
			keyboardtype=MIKeyboardTypeDecimalPad;
		else if(keboard_type=="Twitter")
			keyboardtype=MIKeyboardTypeTwitter;
		else
			keyboardtype=MIKeyboardTypeDefault;

		if(m_type == "MITextView")
		{
			MITextView* _view = static_cast<MITextView*>(view);
			_view->setTextAlignment(alignment);
			_view->keyboard_type=keyboardtype;
		}
		else if(m_type == "MITextField")
		{
			MITextField* _view = static_cast<MITextField*>(view);
			_view->setTextAlignment(alignment);
			_view->keyboard_type=keyboardtype;
		}
		else if(m_type == "MILabel")
		{
			MILabel* _view = static_cast<MILabel*>(view);
			_view->setTextAlignment(alignment);
		}
		//view->setTextAlignment(alignment);
	}
	
	void CustomizeBaseTextViewNode::load( rapidxml::xml_node<>* node )
	{
		CustomizeNode::load(node);
		xml_node<>*_node;
		xml_node<> *css_node=node->first_node("css");
		if ((_node=node->first_node("text")) != NULL)
		{
			m_text=_node->value();
		}
		if ((_node=css_node->first_node("font-size")) != NULL)
		{
			sscanf(_node->value(),"%f",&m_font_size);
			m_font_size= m_font_size/m_layout_docment->width;
			resize_font_custom(m_font_size);
		}
		if ((_node=css_node->first_node("color")) != NULL)
		{
			m_font_color=_node->value();
		}
		if ((_node=css_node->first_node("text-align")) != NULL)
		{
			m_text_alignment=_node->value();
		}
		if ((_node=node->first_node("keyboard_type")) != NULL)
		{
			keboard_type=_node->value();
		}
	}

	MIView * CustomizeMITextViewNode::newNode()
	{
		if(m_type == "MITextField")
		{
			MITextField *ret=new MITextField();
			initBaseTextWithView(ret);
			return ret;
		}
		else if(m_type== "MITextView")
		{
			MITextView *ret=new MITextView();
			initBaseTextWithView(ret);
			if(display_mode=="Normal")
				ret->setDisplayMode(MITextViewDisplayModeNormal);
			else
				ret->setDisplayMode(MITextViewDisplayModeAutoReturn);
			ret->setEditable(editable);
			return ret;
		}
		return NULL;
	}

	void CustomizeMITextViewNode::load( rapidxml::xml_node<>* node )
	{
		CustomizeBaseTextViewNode::load(node);
		xml_node<> *_node=NULL;
		if ((_node=node->first_node("editable")) != NULL)
		{
			if(_node->value()=="false")
				editable=false;
			else
				editable=true;
		}
		if ((_node=node->first_node("display_mode")) != NULL)
		{
			display_mode=_node->value();
		}
	}

	
	MIView * CustomizeMITextFieldNode::newNode()
	{
		MITextField *ret=new MITextField();
		initBaseTextWithView(ret);
		ret->setPlaceholder(m_place_holder);
		return ret;
	}

	void CustomizeMITextFieldNode::load( rapidxml::xml_node<>* node )
	{
		CustomizeBaseTextViewNode::load(node);
		xml_node<> *_node=NULL;
		if ((_node=node->first_node("placeholder")) != NULL)
		{
			m_place_holder=_node->value();
		}
	}


	MIView * CustomizeMILabelNode::newNode()
	{
		MILabel *ret=new MILabel();
		initBaseTextWithView(ret);
		return ret;
	}

	void CustomizeMILabelNode::load( rapidxml::xml_node<>* node )
	{
		CustomizeBaseTextViewNode::load(node);
	}

	void CustomizeMILabelNode::setTextAlignment(MILabel* view, string _text_alignment)
	{
		MITextAlignment alignment;
		if (_text_alignment == "LeftTop")
			alignment=MITextAlignmentLeftTop;
		else if(_text_alignment == "LeftMiddle")
			alignment=MITextAlignmentLeftMiddle;
		else if(_text_alignment == "LeftBottom")
			alignment=MITextAlignmentLeftBottom;
		else if(_text_alignment == "CenterTop")
			alignment=MITextAlignmentCenterTop;
		else if(_text_alignment == "CenterMiddle")
			alignment=MITextAlignmentCenterMiddle;
		else if(_text_alignment == "CenterBottom")
			alignment=MITextAlignmentCenterBottom;
		else if(_text_alignment == "RightTop")
			alignment=MITextAlignmentRightTop;
		else if(_text_alignment == "RightMiddle")
			alignment=MITextAlignmentRightMiddle;
		else if(_text_alignment == "RightBottom")
			alignment=MITextAlignmentRightBottom;
		view->setTextAlignment(alignment);
	}

	void CustomizeMIButtonNode::load( rapidxml::xml_node<>* node )
	{
		CustomizeNode::load(node);
		xml_node<> *_node=NULL;
		if ((_node=node->first_node("type")) != NULL)
		{
			m_button_type=_node->value();
		}
		if (xml_node<> *label_node=node->first_node("MILabel"))
		{
			if((_node=label_node->first_node("text")) != NULL)
			{
				m_label_text=_node->value();
			}
			if ((_node=label_node->first_node("font-size")) != NULL)
			{
				sscanf(_node->value(),"%f",&m_label_font_size);
				//float ss;
				m_label_font_size= m_label_font_size/m_layout_docment->width;
				resize_font_custom(m_label_font_size);
			}
			if ((_node=label_node->first_node("color")) != NULL)
			{
				m_label_font_color=_node->value();
			}
			if ((_node=label_node->first_node("text-align")) != NULL)
			{
				m_label_text_alignment=_node->value();
			}
		}
	}

	MIView * CustomizeMIButtonNode::newNode()
	{
		MIButton *ret=NULL;
		if (m_button_type=="MIButtonTypeRoundedRect")
		{
			ret=MIButton::buttonWithType(MIButtonTypeRoundedRect);
		}
		else
		{
			ret=new MIButton();
		}
		if (m_label_text!="")
		{
			ret->titleLabel()->setText(m_label_text);
		}
		if(m_label_font_size>0)
		{
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_label_font_color);
			if (color!=NULL)
			{
				ret->titleLabel()->setFontSizeAndColor(m_label_font_size,color->m_color);
				delete_and_set_null(color);
			}
			else{
				ret->titleLabel()->setFontSizeAndColor(m_label_font_size);
			}
		}
		if (m_label_text_alignment != "")
		{
			CustomizeMILabelNode::setTextAlignment(ret->titleLabel(),m_label_text_alignment);
		}
		return ret;
	}


	void CustomizeMIViewNode::load(rapidxml::xml_node<>* node)
	{
		CustomizeNode::load(node);
		xml_node<>* _node =  node->first_node("from_file");
		if(_node!=NULL){
			xml_node<>* sub_node =  _node->first_node("file_name");
			if(sub_node!=NULL){
				file_name=sub_node->value();
			}
			if(file_name!=""){
				is_from_file=true;
				xml_node<>*position_node = node->first_node("css");
				if(position_node){
					sscanf(position_node->first_node("left")->value(),"%f",&sub_rect.x);
					sscanf(position_node->first_node("top")->value(),"%f",&sub_rect.y);
					sscanf(position_node->first_node("width")->value(),"%f",&sub_rect.width);
					sscanf(position_node->first_node("height")->value(),"%f",&sub_rect.height);
					if(position_node->first_node("display"))
						is_hide = position_node->first_node("display")->value();
				}
				else{
					sub_rect=MGRectMake(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
					is_hide="true";
				}
				return;
			}		
		}
		
		_node = node->first_node("children");
		is_from_file=false;
		for (;_node!=NULL;_node=_node->next_sibling("children"))
		{
			xml_node<>*child_node = _node->first_node();
			for (;child_node!=NULL;child_node=child_node->next_sibling()){
				CustomizeNode *layout_node=CustomizeNode::loadXML(child_node,m_layout_docment,_layoutController);
				m_layout_docment->addNodeById(layout_node->m_id,layout_node);
				subCustomizeNode.push_back(layout_node);
			}
		}	
		
	}
	MIView* CustomizeMIViewNode::create()
	{
		MIImageView *ret = new MIImageView();
		if(is_from_file==false){
			scale_rect(m_rect,m_layout_docment,priority);
			ret->initWithFrame(m_rect);
			setBackgroundColor(ret);
			if(m_is_hide=="false")
				ret->hide();
			else
				ret->show();
			if(user_interaction=="false")
				ret->_view_flags.user_interaction_enabled=0;
			else
				ret->_view_flags.user_interaction_enabled=1;
			for (CustomizeNodeIt it=subCustomizeNode.begin();it!=subCustomizeNode.end();it++)
			{
				MIView *view=(*it)->create();
				if (view!=NULL)
				{
					m_layout_docment->addViewById((*it)->m_id,view);
					ret->addSubview(view);
				}
			}
			m_layout_docment->addViewById(m_id,ret);
		}
		else{
			//scale_rect(sub_rect);
			scale_rect(sub_rect,m_layout_docment,priority);
			ret->initWithFrame(sub_rect);
			doc = CustomizeDocument::setCustomizeView(ret,MGBundle::mainBundle()->layoutsPathWithFileName(file_name),_layoutController);
			for(ViewIt it=doc->view_map.begin();it!=doc->view_map.end();it++){
				m_layout_docment->addViewById(it->first,it->second);
			}
			for(MyNodeIt it=doc->node_map.begin();it!=doc->node_map.end();it++){
				m_layout_docment->addNodeById(it->first,it->second);
			}
			if(is_hide=="true"){
				ret->show();
				m_layout_docment->current_view=doc->root_view_name;
			}
			else
				ret->hide();

 			MIView *mmview = doc->findViewById(doc->root_view_name);
 			mmview->setFrame(sub_rect);
			m_layout_docment->addViewById(m_id,ret);
		}
		return ret;
	}
	CustomizeMIScrollViewNode::CustomizeMIScrollViewNode()
	{
		vertical_scroll_enabled=1;
		vertical_bounce_enabled=1;
		vertical_bouncing=1;
		horizontal_scroll_enabled=1;
		horizontal_bounce_enabled=1;
		horizontal_bouncing=1;
		zoom_enable=1;
		zoom_bouncing=1;
		always_bounce_horizontal=1;
		always_bounce_vertical=1;
		vertical_indicator_show=1;
		horizontal_indicator_show=1;
		inertia_enable=1;
		decelarate_enable=1;
		passing_vertical_scroll_enable=0;
		passing_horizontal_scroll_enable=0;
		hide_keyboard_when_scrolling=1;
	}

	void CustomizeMIScrollViewNode::load(rapidxml::xml_node<>* node)
	{
		CustomizeNode::load(node);
		xml_node<>* _node = node->first_node("children");
		for (;_node!=NULL;_node=_node->next_sibling("children"))
		{
			xml_node<>*child_node = _node->first_node();
			for (;child_node!=NULL;child_node=child_node->next_sibling()){
				CustomizeNode *layout_node=CustomizeNode::loadXML(child_node,m_layout_docment,_layoutController);
				m_layout_docment->addNodeById(layout_node->m_id,layout_node);
				subCustomizeNode.push_back(layout_node);
			}
		}
		xml_node<>* x_node;
		if ((x_node=node->first_node("scroll_view_flag")) != NULL)
		{
			xml_node<>* _node;
			if ((_node=x_node->first_node("vertical_scroll_enabled")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					vertical_scroll_enabled=1;
				else
					vertical_scroll_enabled=0;
			}
			if ((_node=x_node->first_node("vertical_bounce_enabled")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					vertical_bounce_enabled=1;
				else
					vertical_bounce_enabled=0;
			}
			if ((_node=x_node->first_node("vertical_bouncing_enabled")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					vertical_bouncing=1;
				else
					vertical_bouncing=0;
			}
			if ((_node=x_node->first_node("horizontal_scroll_enabled")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					horizontal_scroll_enabled=1;
				else
					horizontal_scroll_enabled=0;
			}
			if ((_node=x_node->first_node("horizontal_bounce_enabled")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					horizontal_bounce_enabled=1;
				else
					horizontal_bounce_enabled=0;
			}
			if ((_node=x_node->first_node("horizontal_bouncing")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					horizontal_bouncing=1;
				else
					horizontal_bouncing=0;
			}
			if ((_node=x_node->first_node("zoom_enable")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					zoom_enable=1;
				else
					zoom_enable=0;
			}
			if ((_node=x_node->first_node("zoom_bouncing")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					zoom_bouncing=1;
				else
					zoom_bouncing=0;
			}
			if ((_node=x_node->first_node("always_bounce_horizontal")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					always_bounce_horizontal=1;
				else
					always_bounce_horizontal=0;
			}
			if ((_node=x_node->first_node("always_bounce_vertical")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					always_bounce_vertical=1;
				else
					always_bounce_vertical=0;
			}
			if ((_node=x_node->first_node("vertical_indicator_show")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					vertical_indicator_show=1;
				else
					vertical_indicator_show=0;
			}
			if ((_node=x_node->first_node("horizontal_indicator_show")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					horizontal_indicator_show=1;
				else
					horizontal_indicator_show=0;
			}
			if ((_node=x_node->first_node("inertia_enable")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					inertia_enable=1;
				else
					inertia_enable=0;
			}
			if ((_node=x_node->first_node("decelarate_enable")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					decelarate_enable=1;
				else
					decelarate_enable=0;
			}
			if ((_node=x_node->first_node("passing_vertical_scroll_enable")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					passing_vertical_scroll_enable=1;
				else
					passing_vertical_scroll_enable=0;
			}
			if ((_node=x_node->first_node("passing_horizontal_scroll_enable")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					passing_horizontal_scroll_enable=1;
				else
					passing_horizontal_scroll_enable=0;
			}
			if ((_node=x_node->first_node("hide_keyboard_when_scrolling")) != NULL)
			{
				string _bool = _node->value();
				if(_bool=="true")
					hide_keyboard_when_scrolling=1;
				else
					hide_keyboard_when_scrolling=0;
			}
		}
	
	}
	MIView* CustomizeMIScrollViewNode::create()
	{
		MIScrollView *ret = new MIScrollView();
		scale_rect(m_rect,m_layout_docment,priority);
		ret->_scroll_view_flags.vertical_scroll_enabled=vertical_scroll_enabled;
		ret->_scroll_view_flags.vertical_bounce_enabled=vertical_bounce_enabled;
		ret->_scroll_view_flags.vertical_bouncing=vertical_bouncing;
		ret->_scroll_view_flags.horizontal_scroll_enabled=horizontal_scroll_enabled;
		ret->_scroll_view_flags.horizontal_bounce_enabled=horizontal_bounce_enabled;
		ret->_scroll_view_flags.horizontal_bouncing=horizontal_bouncing;
		ret->_scroll_view_flags.zoom_enable=zoom_enable;
		ret->_scroll_view_flags.zoom_bouncing=zoom_bouncing;
		ret->_scroll_view_flags.always_bounce_horizontal=always_bounce_horizontal;
		ret->_scroll_view_flags.always_bounce_vertical=always_bounce_vertical;
		ret->_scroll_view_flags.vertical_indicator_show=vertical_indicator_show;
		ret->_scroll_view_flags.horizontal_indicator_show=horizontal_indicator_show;
		ret->_scroll_view_flags.inertia_enable=inertia_enable;
		ret->_scroll_view_flags.	decelarate_enable=decelarate_enable;
		ret->_scroll_view_flags.passing_vertical_scroll_enable=passing_vertical_scroll_enable;
		ret->_scroll_view_flags.passing_horizontal_scroll_enable=passing_horizontal_scroll_enable;
		ret->_scroll_view_flags.hide_keyboard_when_scrolling=hide_keyboard_when_scrolling;
		ret->initWithFrame(m_rect);
		setBackgroundColor(ret);
		if(m_is_hide=="false")
			ret->hide();
		else
			ret->show();
		if(user_interaction=="false")
			ret->_view_flags.user_interaction_enabled=0;
		else
			ret->_view_flags.user_interaction_enabled=1;
		for (CustomizeNodeIt it=subCustomizeNode.begin();it!=subCustomizeNode.end();it++)
		{
			MIView *view=(*it)->create();
			if (view!=NULL)
			{
				m_layout_docment->addViewById((*it)->m_id,view);
				ret->addSubview(view);
			}
		}
		return ret;
	}

	void CustomizeSegmentedControlNode::load(rapidxml::xml_node<>* node)
	{
		CustomizeNode::load(node);
		if (xml_node<> *_node=node->first_node("items"))
		{
			m_items=_node->value();
		}
	}

	MIView *CustomizeSegmentedControlNode::newNode()
	{
		MISegmentedControl *ret=new MISegmentedControl();
		if (m_items!="")
		{
			vector<std::string> items;
			string temp_items=m_items;
			string::size_type idx=0;
			do
			{
				idx=temp_items.find_first_of(',');
				items.push_back(temp_items.substr(0,idx));
				if (idx!=string::npos)
					temp_items=temp_items.substr(idx+1);
			} while (idx!=string::npos);
			if (!items.empty())
			{
				ret->initWithItems(items);
			}
		}
		return ret;
	}

	void CustomizeTableViewNode::load(rapidxml::xml_node<>* node)
	{
		CustomizeNode::load(node);
		xml_node<>*css_node = node->first_node("css");
		if (xml_node<> *_node=css_node->first_node("border-spacing"))
		{
			separator_style=_node->value();
		}
		
// 		for(xml_node<>*cell_node = node->first_node("MITableViewCell");cell_node!=NULL;cell_node = cell_node->next_sibling("MITableViewCell")){		
// 			CustomizeNode *layout_node=CustomizeNode::loadXML(cell_node,m_layout_docment,_layoutController);
// 			m_layout_docment->addNodeById(layout_node->m_id,layout_node);
// 			subCustomizeNode.push_back(layout_node);
// 		}
	}
	MIView *CustomizeTableViewNode::create()
	{
		MITableView *ret = new MITableView();
		scale_rect(m_rect,m_layout_docment,priority);
		setBackgroundColor(ret);
		if(m_is_hide=="false")
			ret->hide();
		else
			ret->show();
		if(user_interaction=="false")
			ret->_view_flags.user_interaction_enabled=0;
		else
			ret->_view_flags.user_interaction_enabled=1;
		if(separator_style=="MITableViewCellSeparatorStyleSingleLine")
			ret->m_separator_style=MITableViewCellSeparatorStyleSingleLine;
		else if(separator_style=="MITableViewCellSeparatorStyleSingleLineEtched")
			ret->m_separator_style=MITableViewCellSeparatorStyleSingleLineEtched;
		else
			ret->m_separator_style=MITableViewCellSeparatorStyleNone;

// 		for (CustomizeNodeIt it=subCustomizeNode.begin();it!=subCustomizeNode.end();it++)
// 		{
// 			MIView *view=(*it)->create();
// 			if (view!=NULL)
// 			{
// 				m_layout_docment->addViewById((*it)->m_id,view);
// 				ret->table_cells.push_back((MITableViewCell*)view);
// 				CustomizeTableCellNode *table_cell_node = dynamic_cast<CustomizeTableCellNode*>(*it);
// 				assert(table_cell_node!=NULL);
// 				ret->cell_height.push_back(table_cell_node->height);
// 			}
// 		}
		m_layout_docment->addViewById(m_id,ret);
		ret->initWithFrame(m_rect);
		return ret;
	}

	void CustomizePickerViewNode::load(rapidxml::xml_node<>* node)
	{
		CustomizeNode::load(node);
	}
	
	MIView* CustomizePickerViewNode::create()
	{
		MIPickerView *ret = new MIPickerView();
		scale_rect(m_rect,m_layout_docment,priority);
		ret->initWithFrame(m_rect);
		setBackgroundColor(ret);
		if(m_is_hide=="false")
			ret->hide();
		else
			ret->show();
		if(user_interaction=="false")
			ret->_view_flags.user_interaction_enabled=0;
		else
			ret->_view_flags.user_interaction_enabled=1;
		return ret;
	}
	
	CustomizeMapNode* CustomizeMapNode::loadXML(rapidxml::xml_node<>* root,CustomizeDocument *doc,MIViewController *controller)
	{
		CustomizeMapNode *ret=new CustomizeMapNode();
	//	ret->map_delegate = dynamic_cast<MKMapViewDelegate*>(controller);
		if (xml_node<> *_node=root->first_node("id"))
		{
			ret->m_id=_node->value();
		}
		z_base=12;
		if (xml_node<> *_node=root->first_node("z_base"))
		{
			sscanf(_node->value(),"%d",&z_base);
		}

		if(root->first_node("display"))
			ret->m_is_hide = root->first_node("display")->value();

		ret->simulate_gps_data="";
		if (xml_node<> *_node=root->first_node("simulate_gps_data"))
		{
			ret->simulate_gps_data=_node->value();
		}
		ret->show_user_location=true;
		if (xml_node<> *_node=root->first_node("show_user_location"))
		{
			if(_node->value()=="false")
				ret->show_user_location=false;				
		}
		if (xml_node<> *_node=root->first_node("map_type"))
		{
			ret->map_type=_node->value();
		}
		if (xml_node<> *_node=root->first_node("center_coordinate"))
		{
			ret->center_coordinate=_node->value();
		}

		ret->m_layout_docment=doc;
		doc->addNodeById(ret->m_id,ret);
		xml_node<>* node = root->first_node("children");
		for (;node!=NULL;node=node->next_sibling("children"))
		{
			xml_node<>*child_node = node->first_node();
			for (;child_node!=NULL;child_node=child_node->next_sibling()){
				CustomizeNode *layout_node=CustomizeNode::loadXML(child_node,doc,controller);
				doc->addNodeById(layout_node->m_id,layout_node);
				ret->subCustomizeNode.push_back(layout_node);
			}
		}
		doc->root_view_name=ret->m_id;
		return ret;
	}
	MIView *CustomizeMapNode::create(MIView *_view)
	{
		if (_view==NULL)
		{
			return NULL;
		}
		MKMapView *map_view = new MKMapView();
		map_view->delegate = map_delegate;
		map_view->initWithFrame(MGRectMake(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight()));
		_view->addSubview(map_view);
		if(map_type=="MKMapTypeSatellite")
			map_view->setMapType(MKMapTypeSatellite);
		else if(map_type=="MKMapTypeHybrid")
			map_view->setMapType(MKMapTypeHybrid);
		else if(map_type=="MKMapTypeCustom")
			map_view->setMapType(MKMapTypeCustom);
		else if(map_type=="MKMapTypeCustomTraffic")
			map_view->setMapType(MKMapTypeCustomTraffic);
		else if(map_type=="MKMapTypeOsm")
			map_view->setMapType(MKMapTypeOsm);
		else if(map_type=="MKMapTypeCount")
			map_view->setMapType(MKMapTypeCount);
		else
			map_view->setMapType(MKMapTypeStandard);	
		map_view->setShowsUserLocation(show_user_location);
		if(simulate_gps_data!=""){
			string file_path = MGBundle::mainBundle()->pathWithFileName(simulate_gps_data);
			map_view->useSimulateGPSData(file_path);
		}
		string::size_type i = center_coordinate.find(",");
		double lat=0.0;
		double lon=0.0;
		if(i!=string::npos){
			sscanf((center_coordinate.substr(0,i)).c_str(),"%lf",&lat);
			sscanf((center_coordinate.substr(i+1)).c_str(),"%lf",&lon);
			map_view->setCenterCoordinate(MKLocationCoordinate2D(lat,lon));
		}
		m_layout_docment->addViewById(m_id,map_view);
		vector<CustomizeNode*>::iterator it = subCustomizeNode.begin();
		for (;it!=subCustomizeNode.end();it++)
		{
			MIView *view=(*it)->create();
			if (view!=NULL)
			{
				m_layout_docment->addViewById((*it)->m_id,view);
				_view->addSubview(view);
			}
		}
		return _view;
	}
	CustomizeMapNode::CustomizeMapNode()
	{
		m_layout_docment=NULL;
	}

	
}