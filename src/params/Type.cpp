// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2005
// Department of Neuroinformatics, University of Ulm, Germany
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
// $Id$
//
#include "Type.h"

#include <iostream>
#include <algorithm>

#include "qt_compatibility.h"

namespace
{
  QString spaces("                                                        ");
}

namespace Miro
{
  namespace CFG {
    Type::Type() :
        final_(true),
        dummy_(false),
        extern_(false),
        instance_(INSTANCE_NONE)
#if JSONCPP_FOUND
	, useJson_(false)
#endif
    {}

    void
    Type::addStatic(QString const& _type, QString const& _name)
    {
      staticData_.push_back(std::make_pair(_type, _name));
    }

    void
    Type::addToConstructor(QString const& _ctor)
    {
      ctor_.push_back(_ctor);
    }

    void
    Type::addToDocumentation(QString const& _doc)
    {
      doc_.push_back(_doc);
    }

    void
    Type::addToParameterDocumentation(QString const& _name, QString const& _doc)
    {
      if (_name != "") {
	parameterDocumentation_[_name].push_back(_doc);
      }
    }

    void
    Type::addParameter(Parameter const& _parameter)
    {
      ParameterVector::const_iterator i =
        std::find(parameter_.begin(), parameter_.end(), _parameter);
      if (i == parameter_.end())
        parameter_.push_back(_parameter);
      else {
        throw QString("Parameter multiply defined for type " +
                      fullName() +
                      ": " + _parameter.name_);

      }
    }

    void
    Type::addStaticConstParameter(Parameter const& _parameter)
    {
      ParameterVector::const_iterator i =
        std::find(parameter_.begin(), parameter_.end(), _parameter);
      if (i == parameter_.end())
        staticConstParameter_.push_back(_parameter);
      else {
        throw QString("Parameter multiply defined for type " +
                      fullName() +
                      ": " + _parameter.name_);

      }
    }

    void
    Type::generateHeader(std::ostream& ostr, unsigned int _indent, const QString& exportDirective) const
    {
      //   if(name_.isEmpty())
      //     throw QString("No class name specified.");

      unsigned int indent = _indent;

      std::string expStr = "";
      if (!exportDirective.isEmpty()) {
        expStr = QSTRCAST(QString(exportDirective + " "));
      }

      if (!isExtern()) {
        // debug ostream operator
        if (parent_.isEmpty()) {
          ostr << spaces.left(indent) << "class " << name_ << "Parameters;" << std::endl
          << spaces.left(indent) << expStr.c_str() << "std::ostream&" << std::endl
          << spaces.left(indent) << "operator << (std::ostream& ostr, const " << name_ << "Parameters& rhs);" << std::endl
          << std::endl;
        }

        ostr << spaces.left(indent) << "class " << expStr.c_str() << name_ << "Parameters";

        if (!parent_.isEmpty())
          ostr << " : public " << parent_;

        ostr << std::endl
        << spaces.left(indent) << "{" << std::endl;
        indent += STEP;

        if (!parent_.isEmpty())
          ostr << spaces.left(indent) << "typedef " << parent_ << " Super;" << std::endl
          << std::endl;

        ostr << spaces.left(indent - STEP) << "public: " << std::endl;

        // data members
        ParameterVector::const_iterator j;
        for (j = parameter_.begin(); j != parameter_.end(); ++j)
          ostr << spaces.left(indent)
          << ((j->type_ != "angle") ? j->type_ : QString("double"))
          << " " << j->name_ << ";" << std::endl;
        if (parameter_.size() != 0)
          ostr << std::endl;

        // const static data members
        for (j = staticConstParameter_.begin(); j != staticConstParameter_.end(); ++j) {
          ostr << spaces.left(indent)
          << "static const "
          << ((j->type_ != "angle") ? j->type_ : QString("double"))
          << " " << j->name_;

          if (isIntegralType(j->type_)) {
            ostr << " = " << j->fullDefault_;
          }

          ostr << ";" << std::endl;
        }
        if (staticConstParameter_.size() != 0)
          ostr << std::endl;


        // static data members
        QStringPairVector::const_iterator k;
        for (k = staticData_.begin(); k != staticData_.end(); ++k)
          ostr << spaces.left(indent) << "static " << k->first << " " << k->second << ";" << std::endl;
        if (staticData_.size() != 0)
          ostr << std::endl;

        // constructor
        ostr << spaces.left(indent) << name_ << "Parameters();" << std::endl;
        // destructor
        if (parent_.isEmpty())
          ostr << spaces.left(indent) << "virtual ~" << name_ << "Parameters();" << std::endl;
        // type information
        ostr << spaces.left(indent) << "virtual char const * fullTypeName() const throw();" << std::endl;
        // parsing operator
        ostr << std::endl;
        if (parameter_.size() > 0 || parent_.isEmpty()) {
          ostr << spaces.left(indent) << "virtual void operator <<= (const QDomNode&);" << std::endl;
          ostr << spaces.left(indent) << "virtual QDomElement operator >>= (QDomNode&) const;" << std::endl;
#if JSONCPP_FOUND
	  if (useJson_) {
	      ostr << spaces.left(indent) << "virtual void operator <<= (const Json::Value&);" << std::endl;
	      ostr << spaces.left(indent) << "virtual Json::Value operator >>= (Json::Value&) const;" << std::endl;
	  }
#endif
        }
        // debug output operator
        if (parameter_.size() > 0 || parent_.isEmpty()) {
          ostr << spaces.left(indent - STEP) << "protected:" << std::endl
          << spaces.left(indent) << "virtual void printToStream(std::ostream&) const;" << std::endl;
        }

        if (parent_.isEmpty())
          ostr << spaces.left(indent) << "friend" << std::endl
          << spaces.left(indent) << expStr.c_str() << "std::ostream&" << std::endl
          << spaces.left(indent) << "operator << (std::ostream& ostr, const " << name_ << "Parameters& rhs);" << std::endl;
      }
      else {
        // if this is a externally defined class,
        // use global operators for parsing
        ostr << spaces.left(indent) << QSTRCAST(exportDirective) << " void operator <<= (" << name_ << "&, const QDomNode&);" << std::endl;
        ostr << spaces.left(indent) << QSTRCAST(exportDirective) << " QDomElement operator >>= (const " << name_ << "&, QDomNode&);" << std::endl;

      }

      while (indent > _indent) {
        indent -= STEP;
        ostr << spaces.left(indent) << "};" << std::endl;
      }

      switch (instance_) {
      case INSTANCE_MANAGED:
	ostr << std::endl
             << spaces.left(indent) << "typedef ACE_Singleton<"    
             << name_  << "Parameters, ACE_SYNCH_RECURSIVE_MUTEX> "
	     << name_ << "ParametersInstance;" << std::endl;
	break;
      case INSTANCE_UNMANAGED:
	ostr << std::endl
	     << spaces.left(indent) << "typedef ACE_Unmanaged_Singleton<"
	     << name_  << "Parameters, ACE_SYNCH_RECURSIVE_MUTEX> "
	     << name_ << "ParametersInstance;" << std::endl;
	break;
      case INSTANCE_USER:
	ostr << std::endl
	     << spaces.left(indent) << "typedef " << userSingleton_ << "<"
	     << name_  << "Parameters> "
	     << name_ << "ParametersInstance;" << std::endl;
	break;
      case INSTANCE_DLL:
      case INSTANCE_NONE:
	break;
      }
    }

    void
    Type::generateSingleton(std::ostream& ostr, unsigned int indent,
			    QString const& namespaceQualifier,
			    QString const& exportDirective) const
    {
      // add instance declarations
      QString exportUpper = exportDirective.toUpper();
      exportUpper.chop(7); // get rid of _Export

      switch (instance_) {
      case INSTANCE_MANAGED:
	ostr << spaces.left(indent) << exportUpper
             << "_SINGLETON_DECLARE(ACE_Singleton, "               
	     << namespaceQualifier << name_
             << "Parameters, ACE_SYNCH_RECURSIVE_MUTEX);" << std::endl;
	break;
      case INSTANCE_UNMANAGED:
	ostr << spaces.left(indent) << exportUpper
	     << "_SINGLETON_DECLARE(ACE_Unmanaged_Singleton, "
	     << namespaceQualifier << name_
	     << "Parameters, ACE_SYNCH_RECURSIVE_MUTEX);" << std::endl;
	break;
      case INSTANCE_USER:
	ostr << spaces.left(indent) << exportUpper
	     << "_SINGLETON_DECLARATION(" << userSingleton_ << "< "
	     << namespaceQualifier << name_
	     << "Parameters >);" << std::endl;
	break;
      case INSTANCE_DLL:
      case INSTANCE_NONE:
	break;
      }

    }

    void
    Type::generateSource(std::ostream& ostr, unsigned int _indent) const
    {
      //   if(name_.isEmpty())
      //     throw QString("No class name specified.");

      unsigned int indent = _indent;

      if (!isExtern()) {
        QStringVector::const_iterator i;
        ParameterVector::const_iterator j;

        // debug ostream operator
        if (parent_.isEmpty()) {
          ostr << spaces.left(indent) << "std::ostream&" << std::endl
          << spaces.left(indent) << "operator << (std::ostream& ostr, const " << name_ << "Parameters& rhs)" << std::endl
          << spaces.left(indent) << "{" << std::endl;
          indent += STEP;
          ostr << spaces.left(indent) << "rhs.printToStream(ostr);" << std::endl
          << spaces.left(indent) << "return ostr;" << std::endl;
          indent -= STEP;
          ostr << spaces.left(indent) << "}" << std::endl
          << std::endl;
        }

        // static data members
        QStringPairVector::const_iterator k;
        for (k = staticData_.begin(); k != staticData_.end(); ++k)
          ostr << spaces.left(indent) << k->first << " " << name_ << "Parameters::" << k->second << ";" << std::endl;
        if (staticData_.size() != 0)
          ostr << std::endl;

        // const static data members
        for (j = staticConstParameter_.begin(); j != staticConstParameter_.end(); ++j) {
          ostr << spaces.left(indent) << "const "
          << ((j->type_ != "angle") ? j->type_ : QString("double"))
          << " " << name_ << "Parameters::" << j->name_;

          if (!isIntegralType(j->type_)) {
            ostr << "(" << j->fullDefault_ << ")";
          }

          ostr << ";" << std::endl;
        }
        if (staticConstParameter_.size() != 0)
          ostr << std::endl;

        // constructor
        ostr << spaces.left(indent) << name_ << "Parameters::" << name_ << "Parameters()";
        if (parameter_.size() > 0) {
          ostr << " :";
          indent += 2;

          for (j = parameter_.begin(); j != parameter_.end(); ++j) {
            if (j != parameter_.begin())
              ostr << ",";
            ostr << std::endl
            << spaces.left(indent) << j->name_ << "(" << j->fullDefault_ << ")";
          }
          indent -= 2;
        }
        ostr << std::endl
        << spaces.left(indent) << "{" << std::endl;

        indent += STEP;
        for (i = ctor_.begin(); i != ctor_.end(); ++i)
          ostr << spaces.left(indent) << *i << std::endl;
        indent -= STEP;

        ostr << spaces.left(indent) << "}" << std::endl
        << std::endl;

        // destructor
        if (parent_.isEmpty())
          ostr << spaces.left(indent) << name_ << "Parameters::~" << name_ << "Parameters()" << std::endl
          << spaces.left(indent) << "{}" << std::endl
          << std::endl;

        // type information
        ostr << spaces.left(indent) << "char const *" << std::endl
        << spaces.left(indent) << name_ << "Parameters::fullTypeName() const throw()" << std::endl
        << spaces.left(indent) << "{" << std::endl
        << spaces.left(indent + 2) << "return \"" << fullName() << "\";" << std::endl
        << spaces.left(indent) << "}" << std::endl
        << std::endl;

        if (parameter_.size() > 0 || parent_.isEmpty()) {

          // operator <<=
          ostr << spaces.left(indent) << "void" << std::endl
          << spaces.left(indent) << name_ << "Parameters::operator <<= (const QDomNode&";
          if (parameter_.size() > 0 || !parent_.isEmpty())
            ostr << " _node";
          ostr << ")" << std::endl;

          generateQDomOutOperator(ostr, "this->", indent);

          // operator >>=
          ostr << spaces.left(indent) << "QDomElement" << std::endl
          << spaces.left(indent) << name_ << "Parameters::operator >>= (QDomNode& _node) const" << std::endl;
          generateQDomInOperator(ostr, "this->", indent);

#if JSONCPP_FOUND
	  if (useJson_) {
	      // operator <<=
	    ostr << spaces.left(indent) << "void" << std::endl
		 << spaces.left(indent) << name_ << "Parameters::operator <<= (const Json::Value&";
	    if (parameter_.size() > 0 || !parent_.isEmpty())
		ostr << " _node";
	    ostr << ")" << std::endl;
	    generateJsonOutOperator(ostr, "this->", indent);

	    // operator >>=
	    ostr << spaces.left(indent) << "Json::Value" << std::endl
		 << spaces.left(indent) << name_ << "Parameters::operator >>= (Json::Value& _node) const" << std::endl;
	    generateJsonInOperator(ostr, "this->", indent);
	  }
#endif // JSONCPP_FOUND

          // stream output
          ostr << std::endl
          << spaces.left(indent) << "void" << std::endl
          << spaces.left(indent) << name_ << "Parameters::printToStream(std::ostream&";
          if (parameter_.size() > 0 || !parent_.isEmpty())
            ostr << " ostr";
          ostr << ") const" << std::endl
          << spaces.left(indent) << "{" << std::endl;
          indent += STEP;

          if (!parent_.isEmpty())
            ostr << spaces.left(indent) << "Super::printToStream(ostr);" << std::endl
            << std::endl;

          if (parameter_.size() > 0) {
            ostr << spaces.left(indent) << "ostr << ";
            indent += 5;
            for (j = parameter_.begin(); j != parameter_.end(); ++j) {
              if (j != parameter_.begin())
                ostr << std::endl
                << spaces.left(indent) << "<< ";
              if (!j->description_.isEmpty())
                ostr << "\"" << j->description_;
              else {
                QString name(j->name_);
#if QT_VERSION >= 0x040000
                name[0] = name[0].toUpper();
#else
                name[0] = name[0].upper();
#endif

                ostr << "\"" << name;
              }
              ostr << ": \" << ";

              if (j->type_ != "angle")
                ostr << j->name_;
              else
                ostr << "Miro::rad2Deg(" << j->name_ << ")" << std::endl;

              if (!j->measure_.isEmpty())
                ostr << " << \"" << j->measure_ << "\"";
              ostr << " << std::endl";
            }
            ostr << ";" << std::endl;
            indent -= 5;
          }
          indent -= STEP;
          ostr << spaces.left(indent) << "}" << std::endl;
        }
      }
      else {
        // operator <<=
        ostr << spaces.left(indent) << "void" << std::endl
        << spaces.left(indent) << "operator<<= (" << name_ << "& _lhs, const QDomNode& _node)" << std::endl;

        generateQDomOutOperator(ostr, "_lhs.", indent);

        // operator >>=
        ostr << spaces.left(indent) << "QDomElement" << std::endl
        << spaces.left(indent) << "operator>>= (const " << name_ << "& _lhs, QDomNode& _node)" << std::endl;
        generateQDomInOperator(ostr, "_lhs.", indent);

#if JSONCPP_FOUND
	if (useJson_) {
          // operator <<=
          ostr << spaces.left(indent) << "void" << std::endl
	       << spaces.left(indent) << "operator<<= (" << name_ << "& _lhs, const Json::Value& _node)" << std::endl;

	  generateJsonOutOperator(ostr, "_lhs.", indent);

	  // operator >>=
	  ostr << spaces.left(indent) << "Json::Value" << std::endl
	       << spaces.left(indent) << "operator>>= (const " << name_ << "& _lhs, Json::Value& _node)" << std::endl;
	  generateJsonInOperator(ostr, "_lhs.", indent);
	}
#endif // JSONCPP_FOUND
      }
    }

    // operator <<=
    void
    Type::generateQDomOutOperator(std::ostream& ostr,
                                  QString const& classPrefix, unsigned int indent) const
    {
      unsigned int preIndent = indent;

      ostr << spaces.left(indent) << "{" << std::endl;
      indent += STEP;

      if (!parent_.isEmpty())
        ostr << spaces.left(indent) << classPrefix << "Super::operator <<= (_node);" << std::endl
        << std::endl;
      if (parameter_.size() > 0) {
        ostr << spaces.left(indent) << "if (!_node.isNull()) {" << std::endl;
        indent += STEP;
        ostr << spaces.left(indent) << "QDomNode n = _node.firstChild();" << std::endl
        << spaces.left(indent) << "while(!n.isNull() ) {" << std::endl;
        indent += STEP;

        ostr << spaces.left(indent) << "QDomElement e = n.toElement();" << std::endl
        << spaces.left(indent) << "if( !e.isNull() ) {" << std::endl;
        indent += STEP;

        ostr << spaces.left(indent) << "QDomAttr a = e.attributeNode(\"name\");" << std::endl
        << spaces.left(indent) << "if (!a.isNull()) {" << std::endl;
        indent += STEP;

        ostr << spaces.left(indent) << "QString i = a.value();" << std::endl;

        ParameterVector::const_iterator j;
        for (j = parameter_.begin(); j != parameter_.end(); ++j) {
          QString name(j->name_);
#if QT_VERSION >= 0x040000
          name[0] = name[0].toUpper();
#else
          name[0] = name[0].upper();
#endif
          ostr << spaces.left(indent);
          if (j != parameter_.begin())
            ostr << "else ";
          ostr << "if (i == \"" << name << "\") {" << std::endl;
          indent += STEP;
          ostr << spaces.left(indent) << classPrefix << j->name_ << " <<= n;" << std::endl;
          if (j->type_ == "angle")
            ostr << spaces.left(indent) << classPrefix << j->name_
            << " = Miro::deg2Rad(" << classPrefix << j->name_ << ");" << std::endl;

          indent -= STEP;
          ostr << spaces.left(indent) << "}" << std::endl;
        }

        indent -= STEP;
        ostr << spaces.left(indent) << "}" << std::endl;
        indent -= STEP;
        ostr << spaces.left(indent) << "}" << std::endl
        << spaces.left(indent) << "n = n.nextSibling();" << std::endl;
        indent -= STEP;
        ostr << spaces.left(indent) << "}" << std::endl;
      }
      while (indent > preIndent) {
        indent -= STEP;
        ostr << spaces.left(indent) << "}" << std::endl;
      }
    }

    // operator >>=
    void
    Type::generateQDomInOperator(std::ostream& ostr,
                                 QString const& classPrefix, unsigned int indent) const
    {
      ostr << spaces.left(indent) << "{" << std::endl;
      indent += STEP;

      if (parent_.isEmpty()) {
        ostr << spaces.left(indent) << "QDomDocument d = _node.ownerDocument();" << std::endl;
        ostr << spaces.left(indent) << "QDomElement e = d.createElement(\"parameter\");" << std::endl;
        ostr << spaces.left(indent) << "_node.appendChild(e);" << std::endl << std::endl;
      }
      else {
        ostr << spaces.left(indent) << "QDomElement e = Super::operator >>= (_node);" << std::endl;
      }

      ostr << spaces.left(indent) << "QDomElement g;" << std::endl << std::endl;

      ParameterVector::const_iterator j;
      for (j = parameter_.begin(); j != parameter_.end(); ++j) {
        QString name(j->name_);
#if QT_VERSION >= 0x040000
        name[0] = name[0].toUpper();
#else
        name[0] = name[0].upper();
#endif

        if (j->type_ != "angle")
          ostr << spaces.left(indent) << "g = (" << classPrefix << j->name_ << " >>= e);" << std::endl;
        else
          ostr << spaces.left(indent) << "g = (Miro::rad2Deg(" << classPrefix << j->name_ << ") >>= e);" << std::endl;

        ostr << spaces.left(indent) << "g.setAttribute(\"name\", \"" << name << "\");" << std::endl;
      }

      ostr << std::endl
      << spaces.left(indent) << "return e;" << std::endl;
      indent -= STEP;
      ostr << spaces.left(indent) << "}" << std::endl;
    }

#if JSONCPP_FOUND
    // operator <<=
    void
    Type::generateJsonOutOperator(std::ostream& ostr,
                                  QString const& classPrefix, unsigned int indent) const
    {
      unsigned int preIndent = indent;

      ostr << spaces.left(indent) << "{" << std::endl;
      indent += STEP;

      if (!parent_.isEmpty())
        ostr << spaces.left(indent) << classPrefix << "Super::operator <<= (_node);" << std::endl
	     << std::endl;
      if (parameter_.size() > 0) {
        ostr << spaces.left(indent) << "if (!_node.isNull()) {" << std::endl;
        indent += STEP;

        ParameterVector::const_iterator j;
        for (j = parameter_.begin(); j != parameter_.end(); ++j) {
          ostr << spaces.left(indent) << classPrefix << j->name_ << " <<= _node[\"" << j->name_ << "\"];" << std::endl;
          if (j->type_ == "angle") {
            ostr << spaces.left(indent) << classPrefix << j->name_
		 << " = Miro::deg2Rad(" << classPrefix << j->name_ << ");" << std::endl;
	  }
        }

        indent -= STEP;
        ostr << spaces.left(indent) << "}" << std::endl;
      }
      while (indent > preIndent) {
        indent -= STEP;
        ostr << spaces.left(indent) << "}" << std::endl;
      }
    }

    // operator >>=
    void
    Type::generateJsonInOperator(std::ostream& ostr,
                                 QString const& classPrefix, unsigned int indent) const
    {
      ostr << spaces.left(indent) << "{" << std::endl;
      indent += STEP;

      if (!parent_.isEmpty()) {
        ostr << spaces.left(indent) << "Super::operator >>= (_node);" << std::endl;
      }

      ParameterVector::const_iterator j;
      for (j = parameter_.begin(); j != parameter_.end(); ++j) {
	if (j->type_ != "angle") {
	  ostr << spaces.left(indent) << classPrefix << j->name_ << " >>= _node[\"" << j->name_ << "\"];" << std::endl;
        } else {
	  ostr << spaces.left(indent) << "Miro::rad2Deg(" << classPrefix << j->name_ << ") >>= _node[\"" << j->name_ << "\"];" << std::endl;
	}
      }

      ostr << std::endl
	   << spaces.left(indent) << "return _node;" << std::endl;
      indent -= STEP;
      ostr << spaces.left(indent) << "}" << std::endl;
    }
#endif // JSONCPP_FOUND

    bool
    Type::isIntegralType(QString const& _type)
    {
      static char const * integralType[] = { "char", "unsigned char", "signed char",
                                             "short", "unsigned short", "signed short",
                                             "short int", "unsigned short int", "signed short int",
                                             "int", "unsigned int", "signed short int",
                                             "long", "unsigned long", "signed long",
                                             "long int", "unsigned long int", "signed long int",
                                             NULL
                                           };

      char const ** type = integralType;
      while (*type != NULL) {
        if (_type == *type)
          return true;
        ++type;
      }
      return false;
    }
  }
}
