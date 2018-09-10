//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MILayoutNode.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-10-18)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MILAYOUTNODE_H_
#define _MILAYOUTNODE_H_
#include <string>
#include <map>
#include "../../CommonBase/MGStructure.h"
#include "../../GraphicCore/GraphicStructure.h"
#include "../../GraphicCore/MGColor.h"
#include "../../ExternalLib/xml/rapidxml.hpp"
#include "../../ExternalLib/xml/rapidxml_utils.hpp"
#include "../../ExternalLib/xml/rapidxml_print.hpp"

namespace z11
{
	class MILayoutNode;
	class MILayoutDocument ;
	class MIView ;
	class MILabel;
	class MIBaseTextView;
	typedef std::map<std::string,MILayoutNode*> NodesType;
	typedef NodesType::iterator NodesIt;
	typedef std::map<std::string,RGBAColorRef> ColorsType;
	typedef ColorsType::iterator ColorsIt;
	typedef std::map<std::string,MIView *>ViewNodesType;
	typedef ViewNodesType::iterator ViewNodesIt;
	typedef vector<MILayoutNode *>::iterator LayoutNodesIt;

	class MILayoutNode
	{
	public:
		std::string m_type;
		std::string m_id;
		MGRect m_rect;
		std::string m_background;

		MILayoutNode(const std::string& type,const std::string& id,const MGRect& rect,const std::string& background):m_type(type),m_id(id),m_rect(rect),m_background(background){}
		MILayoutNode(){m_parent=NULL;}

		static MILayoutNode *loadXML (rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);

		virtual MIView *create(MIView* root, bool according_view);
		virtual MILayoutNode *parent();
		void addSubLayoutNode(MILayoutNode *_node);
		vector<MILayoutNode *> subLayoutNode;
		MILayoutNode *m_parent;
        virtual ~MILayoutNode()
        {}
	protected:
		virtual void load(rapidxml::xml_node<>* node);

		virtual MIView *newNode();

		void setBackgroundColor(MIView *ret);
	};

	//add by HuDanYuan 2012-05-03  
	class MIBaseTextViewLayoutNode : public MILayoutNode
	{
	public :
        virtual ~MIBaseTextViewLayoutNode()
        {}
		std::string		m_text;
		float			m_font_size;
		std::string		m_font_color;
		std::string		m_text_alignment;
		MIBaseTextViewLayoutNode():m_font_size(-1){}
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);

		void setTextAlignment(/*MIBaseTextView*/MIView* view, string _text_alignment);
	protected:
		virtual void initBaseTextWithView(/*MIBaseTextView*/MIView* view);
	};

	class MITextViewLayoutNode:public MIBaseTextViewLayoutNode
	{
	public :
		MITextViewLayoutNode(){}
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();

	};

	class MITextFieldLayoutNode:public MIBaseTextViewLayoutNode
	{
	public :
		std::string		m_place_holder;
		MITextFieldLayoutNode(){}
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();

	};

	typedef uint_32 MITextAlignment;
	class MILabelLayoutNode:public MIBaseTextViewLayoutNode
	{
	public:
		MILabelLayoutNode(){}
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);

		static void setTextAlignment(MILabel* view, string _text_alignment);
	protected:
		virtual MIView *newNode();
	};

	class MISegmentedControlLayoutNode:public MILayoutNode
	{
	public :
		std::string m_items;
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();
	};

	class MIButtonLayoutNode:public MILayoutNode
	{
	public:
		string m_button_type;
		string m_label_text;
		float		m_label_font_size;
		std::string	m_label_font_color;
		std::string m_label_text_alignment;
		MIButtonLayoutNode():m_label_font_size(-1){}
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
	protected:
		virtual MIView *newNode();
	};

	class MIViewLayouNode:public MILayoutNode
	{
	public :
		static MIViewLayouNode* loadXML(rapidxml::xml_node<>* node,MILayoutDocument *doc);
		virtual MIView *create(MIView *_view, bool according_view);
		rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc);
	private:
		static void load(MIView *view,rapidxml::xml_node<>* node);
		MIViewLayouNode();
		MILayoutDocument *m_layout_docment;
	};

	class MITableViewLayoutNode:public MILayoutNode
	{
	public:
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
		virtual MIView *create(MIView* root, bool according_view);
		size_t data_size;
		int current_cell;
		std::map<std::string,std::vector<std::string> > data;
	protected:
		virtual MIView *newNode();
	};

	class MITableViewCellLayoutNode:public MILayoutNode
	{
	public:
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
		virtual MIView *create(MIView* root, bool according_view, int row);
	protected:
		virtual MIView *newNode();
	};
	class MGMultipleIndexDictionary
	{
	public:
		~MGMultipleIndexDictionary()
		{
			for (size_t i=0;i<m_sub_directory.size();i++)
			{
				delete_and_set_null(m_sub_directory[i]);
			}
			m_sub_directory.clear();
		}
		void addSubDirectory(MGMultipleIndexDictionary *subnode)
		{
			subnode->parent=this;
			m_sub_directory.push_back(subnode);
		}
		MGMultipleIndexDictionary *objectForIndex(size_t index)
		{
			if (index<m_sub_directory.size())
			{
				return m_sub_directory[index];
			}
			return NULL;
		}
		vector<MGMultipleIndexDictionary *> m_sub_directory;
		MGMultipleIndexDictionary *parent;
		vector<string> m_strings;
	};
	class MIPickViewLayoutNode:public MILayoutNode
	{
	public:
		virtual void load(rapidxml::xml_node<>* node);
		virtual bool addAttribute(const std::string& _attribute,const std::string& _value);
		virtual rapidxml::xml_node<>* save(rapidxml::xml_document<> *doc ,rapidxml::xml_node<>* node);
		virtual MIView *create(MIView* root, bool according_view);
		MGMultipleIndexDictionary *data;
		vector<MGSize> component_size;
	protected:
		MGMultipleIndexDictionary *loadTitles(rapidxml::xml_node<>* _node);
	protected:
		virtual MIView *newNode();
	};
}


#endif //_MILAYOUTNODE_H_
