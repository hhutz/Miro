// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2013
// Department of Neural Information Processing, University of Ulm
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#include "SimpleParameter.h"

#include "ConfigFile.h"
#include "SimpleParameterEdit.h"
#include "ParameterDialog.h"
#include "ParameterListDialog.h"
#include "SingleParameterDialog.h"

#include "params/Generator.h"

#include <q3popupmenu.h>
#include <q3listview.h>

namespace 
{
  struct Pair {
    char const * name;
    SimpleParameter::Type type;
  };

  Pair typeMap [] = {
    { "char",              SimpleParameter::CHAR },
    { "bool",              SimpleParameter::BOOL },
    { "short",             SimpleParameter::SHORT },
    { "unsigned short",    SimpleParameter::USHORT },
    { "int",               SimpleParameter::INT },
    { "unsigned int",      SimpleParameter::UINT },
    { "long",              SimpleParameter::LONG },
    { "unsigned long",     SimpleParameter::ULONG },
    { "float",             SimpleParameter::FLOAT },
    { "double",            SimpleParameter::DOUBLE },
    { "angle",             SimpleParameter::ANGLE },
    { "Miro::Angle",       SimpleParameter::MIRO_ANGLE },
    { "Angle",             SimpleParameter::MIRO_ANGLE },
    { "std::string",       SimpleParameter::STRING },
    { "string",            SimpleParameter::STRING },
    { "Text",              SimpleParameter::TEXT },
    { "Miro::Text",        SimpleParameter::TEXT },
    { "ACE_Time_Value",    SimpleParameter::ACE_TIME_VALUE },
    { "ACE_INET_Addr",     SimpleParameter::ACE_INET_ADDR },
    { "Enumeration",       SimpleParameter::ENUMERATION },
    { "Miro::Enumeration", SimpleParameter::ENUMERATION },
    { "EnumerationMultiple",       SimpleParameter::ENUMERATIONMULTIPLE },
    { "Miro::EnumerationMultiple", SimpleParameter::ENUMERATIONMULTIPLE }
  };
}

QString const SimpleParameter::XML_ATTRIBUTE_VALUE ="value";

SimpleParameter::Type 
SimpleParameter::typeFromName(QString const& _type)
{
  for (unsigned int i = 0; i < sizeof(typeMap)/sizeof(Pair); ++i) {
    if (_type == typeMap[i].name)
      return typeMap[i].type;
  }
  return SimpleParameter::NONE;
}

SimpleParameter::SimpleParameter(Miro::CFG::Parameter const& _param,
				 QDomNode const& _node,
				 Q3ListViewItem * _parentItem,
				 Q3ListViewItem * _pre,
				 QObject * _parent, const char * _name) :
  Super(_node, _parentItem, _pre, _parent, _name),
  param_(_param)
{
  QDomElement e = node().toElement();
  if (listViewItem()->listView()->columns() >= 2)
    listViewItem()->setText(1, e.attribute(XML_ATTRIBUTE_VALUE));
  if (listViewItem()->listView()->columns() >= 3)
    listViewItem()->setText(2, param_.type_);
}

SimpleParameter::SimpleParameter(Miro::CFG::Parameter const& _param,
				 QDomNode const& _node,
				 Q3ListView * _list,
				 Q3ListViewItem * _pre,
				 QObject * _parent, const char * _name) :
  Super(_node, _list, _pre, _parent, _name),
  param_(_param)
{
  QDomElement e = node().toElement();
  if (listViewItem()->listView()->columns() >= 2)
    listViewItem()->setText(1, e.attribute(XML_ATTRIBUTE_VALUE));
  if (listViewItem()->listView()->columns() >= 3)
    listViewItem()->setText(2, param_.type_);
}

void
SimpleParameter::setParameters()
{
  SingleParameterDialog dialog(param_,
			       node_,
			       NULL, this, NULL, name());
  int rc = dialog.exec();
  if (rc == QDialog::Accepted) {
    dialog.setXML();
    if (dialog.modified())
      setModified(true);
  }
}
