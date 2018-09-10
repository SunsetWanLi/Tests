#include "MILayoutNode.h"
#include "MILayoutEngine.h"
// #include "../MIControls/MIMapView.h"
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
#include <iostream>
#include "../../System/Device/MGDevice.h"
#include "../../System/Shared/MGLog.h"
using namespace rapidxml;
using namespace std;
namespace z11
{
	void resize_font(float &a)
	{
		if (a<1&&a>0)
		{
			a*=MGDevice::getScreenWidth();
		}

	}

	MILayoutNode * MILayoutNode::loadXML( xml_node<>* node )
	{
		MILayoutNode *ret=NULL;

		string type=node->name();
		if (type == "MITextField")
		{
			ret=new MITextFieldLayoutNode();
		}
		else if (type=="MILabel")
		{
			ret=new MILabelLayoutNode();
		}
		else if(type=="MITextView"||type=="MITextArea")
		{
			ret=new MITextViewLayoutNode();
		}
		else if(type=="MISegmentedControl")
		{
			ret=new MISegmentedControlLayoutNode();
		}
		else if(type=="MIButton")
		{
			ret=new MIButtonLayoutNode();
		}
		else if(type=="MITableView")
		{
			ret=new MITableViewLayoutNode();
		}
		else if(type=="MIPickerView")
		{
			ret=new MIPickViewLayoutNode();
		}
		else
			ret=new MILayoutNode();

		ret->load(node);
		xml_attribute<> *attribute=node->first_attribute("id");
		if(attribute!=NULL)
		{
            ret->m_id=attribute->value();
		}

		return ret;
	}
	void scale_rect(MIView* root, MGRect &rect)
	{
		uint_32 width;
		uint_32 height;
        if (root == NULL) {
            width=MGDevice::getScreenWidth();
            height=MGDevice::getScreenHeight();
        } else {
            width = root->getBounds().width;
            height = root->getBounds().height;
        }
		if (rect.x<5)
		{
			rect.x*=width;
		}
		if (rect.y<5)
		{
			rect.y*=height;
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
	MIView * MILayoutNode::create(MIView* root, bool according_view)
	{
		MIView *ret=newNode();
		if (ret!=NULL)
		{
			setBackgroundColor(ret);
            if (according_view) {
                scale_rect(root, m_rect);
            } else {
                scale_rect(NULL, m_rect);
            }
			//MGLogD("rect,%f,%f,%f,%f",m_rect.x,m_rect.y,m_rect.width,m_rect.height);
			ret->initWithFrame(m_rect);
		}
		return ret;
	}

	MIView * MILayoutNode::newNode()
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
		return ret;
	}

	void MILayoutNode::setBackgroundColor( MIView *ret )
	{
		MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_background);
		if (color!=NULL)
		{
			ret->setBackgroundColor(color);
		}
	}

	void MILayoutNode::load( rapidxml::xml_node<>* node )
	{
		MILayoutNode *ret=this;
		ret->m_type=node->name();
		sscanf(node->first_attribute("x")->value(),"%f",&ret->m_rect.x);
		sscanf(node->first_attribute("y")->value(),"%f",&ret->m_rect.y);
		sscanf(node->first_attribute("width")->value(),"%f",&ret->m_rect.width);
		sscanf(node->first_attribute("height")->value(),"%f",&ret->m_rect.height);
		if (xml_attribute<> *attribute=node->first_attribute("background"))
		{
			ret->m_background=attribute->value();
		}

	}

	void MILayoutNode::addSubLayoutNode( MILayoutNode *_node )
	{
		subLayoutNode.push_back(_node);
		_node->m_parent=this;
	}

	xml_node<>* MILayoutNode::save( xml_document<> *doc ,xml_node<>* node )
	{
		if (node==NULL)
		{
			node =  doc->allocate_node(node_element,m_type.c_str(),"");
		}
		if (m_id!="")
		{
			node->append_attribute(doc->allocate_attribute("id",m_id.c_str()));
		}
		if (m_rect.width!=0)
		{
			char str[16];
			sprintf(str,"%.0f",m_rect.x);
			node->append_attribute(doc->allocate_attribute("x",doc->allocate_string(str)));
			sprintf(str,"%.0f",m_rect.y);
			node->append_attribute(doc->allocate_attribute("y",doc->allocate_string(str)));
			sprintf(str,"%.0f",m_rect.width);
			node->append_attribute(doc->allocate_attribute("width",doc->allocate_string(str)));
			sprintf(str,"%.0f",m_rect.height);
			node->append_attribute(doc->allocate_attribute("height",doc->allocate_string(str)));
		}
		if(m_background!="")
		{
			node->append_attribute(doc->allocate_attribute("background",m_background.c_str()));
		}
		return node;
	}

	bool MILayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		if (_attribute=="id")
		{
			m_id=_value;
		}
		else if (_attribute == "background")
		{
			m_background=_value;
		}
		else if(_attribute == "rect")
		{
			sscanf(_value.c_str(),"%f,%f,%f,%f",&m_rect.x,&m_rect.y,&m_rect.width,&m_rect.height);
		}
		else if(_attribute == "x")
		{
			sscanf(_value.c_str(),"%f",&m_rect.x);
		}
		else if(_attribute == "y")
		{
			sscanf(_value.c_str(),"%f",&m_rect.y);
		}
		else if(_attribute == "width")
		{
			sscanf(_value.c_str(),"%f",&m_rect.width);
		}
		else if(_attribute == "height")
		{
			sscanf(_value.c_str(),"%f",&m_rect.height);
		}
		return true;
	}

	MILayoutNode * MILayoutNode::parent()
	{
		return m_parent; 
	}








	void MIBaseTextViewLayoutNode::initBaseTextWithView(MIView* view)
	{
		//temporary
		if(m_type == "MITextView")
		{
			MITextView* _view = static_cast<MITextView*>(view);
			string type="@data/";
			if (m_text.find(type)!=string::npos)
			{
				MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
				_view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);
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
				MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
				_view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);
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
				MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
				_view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);
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
		//string type="@data/";
		//if (m_text.find(type)!=string::npos)
		//{
		//	MITableViewLayoutNode* _layout_node=(MITableViewLayoutNode*)parent()->parent();
		//	view->setText(_layout_node->data[m_text.substr(type.size())][_layout_node->current_cell]);
		//}
		//else
		//{
		//	view->setText(m_text);
		//}
		//if(m_font_size>0)
		//{
		//	MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_font_color);
		//	if (color!=NULL)
		//	{
		//		view->setFontSizeAndColor(m_font_size,color->m_color);
		//		delete_and_set_null(color);
		//	}
		//}
		//if (m_text_alignment != "")
		//{
		//	setTextAlignment(view, m_text_alignment);
		//}
	}

	void MIBaseTextViewLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MILayoutNode::load(node);
		xml_attribute<> *attribute=NULL;
		if ((attribute=node->first_attribute("text")) != NULL)
		{
			m_text=attribute->value();
		}
		if ((attribute=node->first_attribute("font_size")) != NULL)
		{
			sscanf(attribute->value(),"%f",&m_font_size);
			resize_font(m_font_size);
		}
		if ((attribute=node->first_attribute("font_color")) != NULL)
		{
			m_font_color=attribute->value();
		}
		if ((attribute=node->first_attribute("text_alignment")) != NULL)
		{
			m_text_alignment=attribute->value();
		}

	}

	rapidxml::xml_node<>* MIBaseTextViewLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		MILayoutNode::save(doc,node);
		if (m_text!="")
		{
			node->append_attribute(doc->allocate_attribute("text",doc->allocate_string(m_text.c_str())));
		}
		if (m_font_size>0)
		{
			char str[16];
			sprintf(str,"%.3f",m_font_size);
			node->append_attribute(doc->allocate_attribute("font_size",doc->allocate_string(str)));
			if (m_font_color!="")
			{
				node->append_attribute(doc->allocate_attribute("font_color",doc->allocate_string(m_font_color.c_str())));
			}
		}
		if (m_text_alignment!="")
		{
			node->append_attribute(doc->allocate_attribute("text_alignment",doc->allocate_string(m_text_alignment.c_str())));
		}
		return node;
	}

	bool MIBaseTextViewLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		if (_attribute=="text")
		{
			m_text=_value;
		}
		else if(_attribute=="font_size")
		{
			sscanf(_value.c_str(),"%f",&m_font_size);
		}
		else if(_attribute=="font_color")
		{
			m_font_color=_value;
		}
		else if (_attribute=="text_alignment")
		{
			m_text_alignment=_value;
		}
		else
		{
			MILayoutNode::addAttribute(_attribute,_value);
		}
		return true;
	}

	void MIBaseTextViewLayoutNode::setTextAlignment( MIView *view,string _text_alignment )
	{
		MITextAlignment alignment;
		if (_text_alignment == "MITextAlignmentLeftTop")
			alignment=MITextAlignmentLeftTop;
		else if(_text_alignment == "MITextAlignmentLeftMiddle")
			alignment=MITextAlignmentLeftMiddle;
		else if(_text_alignment == "MITextAlignmentLeftBottom")
			alignment=MITextAlignmentLeftBottom;
		else if(_text_alignment == "MITextAlignmentCenterTop")
			alignment=MITextAlignmentCenterTop;
		else if(_text_alignment == "MITextAlignmentCenterMiddle")
			alignment=MITextAlignmentCenterMiddle;
		else if(_text_alignment == "MITextAlignmentCenterBottom")
			alignment=MITextAlignmentCenterBottom;
		else if(_text_alignment == "MITextAlignmentRightTop")
			alignment=MITextAlignmentRightTop;
		else if(_text_alignment == "MITextAlignmentRightMiddle")
			alignment=MITextAlignmentRightMiddle;
		else if(_text_alignment == "MITextAlignmentRightBottom")
			alignment=MITextAlignmentRightBottom;

		if(m_type == "MITextView")
		{
			MITextView* _view = static_cast<MITextView*>(view);
			_view->setTextAlignment(alignment);
		}
		else if(m_type == "MITextField")
		{
			MITextField* _view = static_cast<MITextField*>(view);
			_view->setTextAlignment(alignment);
		}
		else if(m_type == "MILabel")
		{
			MILabel* _view = static_cast<MILabel*>(view);
			_view->setTextAlignment(alignment);
		}
		//view->setTextAlignment(alignment);
	}


	MIView * MITextViewLayoutNode::newNode()
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
			return ret;
		}
		return NULL;
	}

	void MITextViewLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MIBaseTextViewLayoutNode::load(node);
	}

	rapidxml::xml_node<>* MITextViewLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		MIBaseTextViewLayoutNode::save(doc,node);
		return node;
	}

	bool MITextViewLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		if (_attribute=="text")
		{
			m_text=_value;
		}
		else
		{
			MIBaseTextViewLayoutNode::addAttribute(_attribute,_value);
		}
		return true;
	}


	MIView * MITextFieldLayoutNode::newNode()
	{
		MITextField *ret=new MITextField();
		ret->setPlaceholder(m_place_holder);
		initBaseTextWithView(ret);
		return ret;
	}

	void MITextFieldLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MIBaseTextViewLayoutNode::load(node);
		xml_attribute<> *attribute=NULL;
		if ((attribute=node->first_attribute("placeholder")) != NULL)
		{
			m_place_holder=attribute->value();
		}
	}



	rapidxml::xml_node<>* MITextFieldLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		MIBaseTextViewLayoutNode::save(doc,node);
		if (m_place_holder!="")
		{
			node->append_attribute(doc->allocate_attribute("placeholder",doc->allocate_string(m_place_holder.c_str())));
		}
		return node;
	}

	bool MITextFieldLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		if (_attribute=="placeholder")
		{
			m_place_holder=_value;
		}
		else
		{
			MIBaseTextViewLayoutNode::addAttribute(_attribute,_value);
		}
		return true;
	}


	MIView * MILabelLayoutNode::newNode()
	{
		MILabel *ret=new MILabel();
		initBaseTextWithView(ret);
		return ret;
	}

	void MILabelLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MIBaseTextViewLayoutNode::load(node);
	}

	rapidxml::xml_node<>* MILabelLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		MIBaseTextViewLayoutNode::save(doc,node);
		return node;
	}

	bool MILabelLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		MIBaseTextViewLayoutNode::addAttribute(_attribute,_value);
		return true;
	}
	void MILabelLayoutNode::setTextAlignment(MILabel* view, string _text_alignment)
	{
		MITextAlignment alignment = MITextAlignmentCenterMiddle;
		if (_text_alignment == "MITextAlignmentLeftTop")
			alignment=MITextAlignmentLeftTop;
		else if(_text_alignment == "MITextAlignmentLeftMiddle")
			alignment=MITextAlignmentLeftMiddle;
		else if(_text_alignment == "MITextAlignmentLeftBottom")
			alignment=MITextAlignmentLeftBottom;
		else if(_text_alignment == "MITextAlignmentCenterTop")
			alignment=MITextAlignmentCenterTop;
		else if(_text_alignment == "MITextAlignmentCenterMiddle")
			alignment=MITextAlignmentCenterMiddle;
		else if(_text_alignment == "MITextAlignmentCenterBottom")
			alignment=MITextAlignmentCenterBottom;
		else if(_text_alignment == "MITextAlignmentRightTop")
			alignment=MITextAlignmentRightTop;
		else if(_text_alignment == "MITextAlignmentRightMiddle")
			alignment=MITextAlignmentRightMiddle;
		else if(_text_alignment == "MITextAlignmentRightBottom")
			alignment=MITextAlignmentRightBottom;
		view->setTextAlignment(alignment);
	}


	void MISegmentedControlLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MILayoutNode::load(node);
		if (xml_attribute<> *attribute=node->first_attribute("items"))
		{
			m_items=attribute->value();
			//string items=attribute->value();
			//string::size_type idx=0;
			//do
			//{
			//	idx=items.find_first_of(',');
			//	m_items.push_back(items.substr(0,idx));
			//	if (idx!=string::npos)
			//		items=items.substr(idx+1);
			//} while (idx!=string::npos);
		}
	}

	MIView * MISegmentedControlLayoutNode::newNode()
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

	rapidxml::xml_node<>* MISegmentedControlLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		node =  doc->allocate_node(node_element,m_type.c_str(),"");
		MILayoutNode::save(doc,node);
		if (m_items!="")
		{
			node->append_attribute(doc->allocate_attribute("items",doc->allocate_string(m_items.c_str())));
		}
		return node;
	}

	bool MISegmentedControlLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		if (_attribute=="items")
		{
			m_items=_value;
		}
		else
			MILayoutNode::addAttribute(_attribute,_value);
		return true;
	}


	void MIButtonLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MILayoutNode::load(node);
		xml_attribute<> *attribute=NULL;
		if ((attribute=node->first_attribute("type")) != NULL)
		{
			m_button_type=attribute->value();
		}
		if (xml_node<> *label_node=node->first_node("MILabel"))
		{
			if((attribute=label_node->first_attribute("text")) != NULL)
			{
				m_label_text=attribute->value();
			}
			if ((attribute=label_node->first_attribute("font_size")) != NULL)
			{
				sscanf(attribute->value(),"%f",&m_label_font_size);
				resize_font(m_label_font_size);
			}
			if ((attribute=label_node->first_attribute("font_color")) != NULL)
			{
				m_label_font_color=attribute->value();
			}
			if ((attribute=label_node->first_attribute("text_alignment")) != NULL)
			{
				m_label_text_alignment=attribute->value();
			}
		}
	}

	MIView * MIButtonLayoutNode::newNode()
	{
		MIButton *ret=NULL;
		if (m_button_type=="MIButtonTypeRoundedRect")
		{
			ret=MIButton::buttonWithType(MIButtonTypeRoundedRect);
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
			}
			if (m_label_text_alignment != "")
			{
				MILabelLayoutNode::setTextAlignment(ret->titleLabel(),m_label_text_alignment);
			}
		}
		else
		{
			ret=new MIButton();
		}
		return ret;
	}

	rapidxml::xml_node<>* MIButtonLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		node =  doc->allocate_node(node_element,m_type.c_str(),"");
		MILayoutNode::save(doc,node);
		if (m_button_type!="")
		{
			node->append_attribute(doc->allocate_attribute("type",doc->allocate_string(m_button_type.c_str())));
			if (m_button_type=="MIButtonTypeRoundedRect")
			{
				if (m_label_text!="")
				{
					rapidxml::xml_node<>*label_node=doc->allocate_node(node_element,"MILabel","");
					label_node->append_attribute(doc->allocate_attribute("text",doc->allocate_string(m_label_text.c_str())));
					node->append_node(label_node);
				}
			}
		}
		return node;
	}

	bool MIButtonLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		if (_attribute=="type")
		{
			m_button_type=_value;
		}
		else
			MILayoutNode::addAttribute(_attribute,_value);
		return true;
	}


	void MIViewLayouNode::load( MIView *view,rapidxml::xml_node<>* node )
	{
		if (xml_attribute<> *attribute=node->first_attribute("background"))
		{
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(attribute->value());
			if (color!=NULL)
			{
				view->setBackgroundColor(color);
			}
		}
	}

    MIViewLayouNode* MIViewLayouNode::loadXML(rapidxml::xml_node<>* root,MILayoutDocument *doc)
    {
        MIViewLayouNode *ret=new MIViewLayouNode();
        if (xml_attribute<> *attribute=root->first_attribute("background"))
		{
		    ret->m_background=attribute->value();
		}

		ret->m_layout_docment=doc;

		// 查找下层节点   //
        xml_node<>* node = root->first_node();
		for (;node!=NULL;node=node->next_sibling())
		{
			//add to document's node
			MILayoutNode *layout_node=MILayoutNode::loadXML(node);
			doc->addNodeById(layout_node->m_id,layout_node);
			//add to sub node
			ret->subLayoutNode.push_back(layout_node);
		}
		return ret;
    }

	MIView * MIViewLayouNode::create( MIView *_view, bool according_view )
	{
		if (_view==NULL)
		{
			return NULL;
		}

		if (m_background!="")
		{
			MGColor *color=MILayoutEngine::sharedEngine().findColorById(m_background);
			if (color!=NULL)
			{
				_view->setBackgroundColor(color);
			}
		}

		for (LayoutNodesIt it=subLayoutNode.begin();it!=subLayoutNode.end();it++)
		{
			MIView *view=(*it)->create(_view, according_view);
			if (view!=NULL)
			{
				m_layout_docment->addViewById((*it)->m_id,view);
				_view->addSubview(view);
			}
		}

		return _view;
	}

	MIViewLayouNode::MIViewLayouNode()
	{
		m_layout_docment=NULL;
	}

	xml_node<>* MIViewLayouNode::save( xml_document<> *doc )
	{
		xml_node<>* node =  doc->allocate_node(node_element,"MIView","");
		MILayoutNode::save(doc,node);
		for (LayoutNodesIt it=subLayoutNode.begin();it!=subLayoutNode.end();it++)
		{
			node->append_node((*it)->save(doc,NULL));
		}
		return node;
	}


	void MITableViewLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MILayoutNode::load(node);
		xml_node<> *_node=NULL;
		//load data
		if ((_node=node->first_node("data")) != NULL)
		{
			xml_attribute<> *attribute=_node->first_attribute("size");
			if (attribute!=NULL)
			{
				sscanf(attribute->value(),"%ld",&data_size);
			}
			for(xml_node<> *data_node=_node->first_node();data_node!=NULL;data_node=data_node->next_sibling())
			{
				string string_name=data_node->value();
				string type("@strings/");
				string::size_type pos=string_name.find(type);
				if (pos!=string::npos)
				{
					const std::vector<std::string> * vt=MILayoutEngine::sharedEngine().findStringsById(string_name.substr(type.size()));
					if (vt!=NULL)
					{
						data[data_node->first_attribute("name")->value()]=*vt;
					}
				}
				//data[data_node->first_attribute("name")->value()]=data_node->value();
			}

		}
		//load cell structure;
		if ((_node=node->first_node("MITableViewCell")) != NULL)
		{
			MITableViewCellLayoutNode *cell_node=new MITableViewCellLayoutNode();
			cell_node->load(_node);
			addSubLayoutNode(cell_node);
			//subLayoutNode.push_back(cell_node);
		}


	}


	MIView * MITableViewLayoutNode::newNode()
	{
		return NULL;
	}

	bool MITableViewLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		return false;
	}

	rapidxml::xml_node<>* MITableViewLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		return NULL;
	}

	MIView * MITableViewLayoutNode::create(MIView* root, bool according_view)
	{
		MITableView *ret = new MITableView();
		MITemplateDataSource *data_source=new MITemplateDataSource();
		//data_source->release();
		for (std::map<std::string,vector<std::string> >::iterator  it=data.begin();it!=data.end();++it)
		{
			data_source->data[it->first]=(it->second);
		}
		ret->setTableViewDataSource(data_source);
		data_source->layout_node=this;
		if (ret!=NULL)
		{
			setBackgroundColor(ret);
            if (according_view) {
                scale_rect(root, m_rect);
            } else {
                scale_rect(NULL, m_rect);
            }
			ret->initWithFrame(m_rect);
		}
		return ret;
	}

	void MITableViewCellLayoutNode::load( rapidxml::xml_node<>* root )
	{
		MILayoutNode::load(root);
		for (rapidxml::xml_node<>* node=root->first_node();node!=NULL;node=node->next_sibling())
		{
			addSubLayoutNode(MILayoutNode::loadXML(node));
		}
	}
	MIView * MITableViewCellLayoutNode::newNode()
	{
		return NULL;
	}

	bool MITableViewCellLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		return false;
	}

	rapidxml::xml_node<>* MITableViewCellLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		return NULL;
	}

	MIView * MITableViewCellLayoutNode::create(MIView* root, bool according_view, int row)
	{
		MITemplateTableCell *ret=new MITemplateTableCell();
		if (ret!=NULL)
		{
			setBackgroundColor(ret);
            if (according_view) {
                scale_rect(root, m_rect);
            } else {
                scale_rect(NULL, m_rect);
            }
			
			ret->initWithFrame(m_rect);
		}
		((MITableViewLayoutNode*)parent())->current_cell=row;
		for (size_t i=0;i<subLayoutNode.size();++i)
		{
			ret->addSubview(subLayoutNode[i]->create(root, according_view));
		}
		return ret;

	}



	void MIPickViewLayoutNode::load( rapidxml::xml_node<>* node )
	{
		MILayoutNode::load(node);
		xml_node<> *_node=NULL;
		data=NULL;
		//load data
		if ((_node=node->first_node("data")) != NULL)
		{
			xml_node<> *data_node=NULL;
			if ((data_node=_node->first_node())!=NULL)
			{
				data=loadTitles(data_node);
			}
		}
		if ((_node=node->first_node("layout")) != NULL)
		{
			for(xml_node<> *data_node=_node->first_node();data_node!=NULL;data_node=data_node->next_sibling())
			{
				MGSize _size;
				sscanf( data_node->first_attribute("width")->value(),"%f",&_size.width);
				sscanf( data_node->first_attribute("height")->value(),"%f",&_size.height);
				component_size.push_back(_size);
			}
		}

	}

	bool MIPickViewLayoutNode::addAttribute( const std::string& _attribute,const std::string& _value )
	{
		return false;
	}

	rapidxml::xml_node<>* MIPickViewLayoutNode::save( rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node )
	{
		return NULL;
	}

	MIView * MIPickViewLayoutNode::create(MIView* root, bool according_view)
	{
		MIPickerView *ret = new MIPickerView();
		MITemplatePickerDataSource *data_source=new MITemplatePickerDataSource();
		data_source->data=data;
		ret->setDataSource(data_source);
		MITemplatePickerDelegate *data_delegate=new MITemplatePickerDelegate();
		data_delegate->component_size=component_size;
		ret->setDelegate(data_delegate);
		if (ret!=NULL)
		{
			setBackgroundColor(ret);
            if (according_view) {
                scale_rect(root, m_rect);
            } else {
                scale_rect(NULL, m_rect);
            }
			ret->initWithFrame(m_rect);
		}
		return ret;
	}

	MIView * MIPickViewLayoutNode::newNode()
	{
		return NULL;
	}

	MGMultipleIndexDictionary * MIPickViewLayoutNode::loadTitles( rapidxml::xml_node<>* _node )
	{
		MGMultipleIndexDictionary *ret=new MGMultipleIndexDictionary();
		for(xml_node<> *data_node=_node->first_node();data_node!=NULL;data_node=data_node->next_sibling())
		{
			ret->addSubDirectory(loadTitles(data_node));
		}
		string string_name=_node->first_attribute("titles")->value();
		string type("@strings/");
		string::size_type pos=string_name.find(type);
		if (pos!=string::npos)
		{
			const std::vector<std::string> * vt=MILayoutEngine::sharedEngine().findStringsById(string_name.substr(type.size()));
			if (vt!=NULL)
			{
				ret->m_strings=(*vt);
			}
		}
		return ret;
	}
}
