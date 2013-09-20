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
#include "Parser.h"
#include "Generator.h"
#include "Parameter.h"

#include <qstring.h>

namespace 
{
  QString currentParameterName;
}

namespace Miro
{
  namespace CFG {
    Parser::Parser(Generator& _generator) :
        type_(),
        generator_(_generator),
        parsing_(false),
        parameterParsing_(false),
        docuParsing_(false),
        ctorParsing_(false),

        staticConst_(false),
        instance_(false),
        unmanaged_(false),
        userSingleton_(false),
        userSingletonName_(),
        string_(false),
        enumeration_(false),
        enumerationMultiple_(false),
        vector_(false),
        set_(false),
        angle_(false),
        timeValue_(false),
        inetAddr_(false),
        serialParams_(false),
        scanDescription_(false),
        schedParams_(false),
        text_(false)
#if JSONCPP_FOUND
	, useJson_(false)
#endif
    {
    }

    void
    Parser::reset()
    {
      parsing_ = false;
      parameterParsing_ = false;
      docuParsing_ = false;
      ctorParsing_ = false;

      staticConst_ = false;
      instance_ = false;
      unmanaged_ = false;
      userSingleton_ = false;
      userSingletonName_ = "";
      string_ = false;
      enumeration_ = false;
      enumerationMultiple_ = false;
      vector_ = false;
      set_ = false;
      angle_ = false;
      timeValue_ = false;
      inetAddr_ = false;
      serialParams_ = false;
      scanDescription_ = false;
      schedParams_ = false;
      text_ = false;

      docu_ = "";
      ctor_ = "";
      error_ = "";
    }

    bool Parser::startDocument()
    {
#if JSONCPP_FOUND
      generator_.setUseJson(useJson_);
#endif
      return true;
    }

    bool Parser::startElement(QString const&,
                              QString const&,
                              QString const& qName,
                              QXmlAttributes const& attributes)
    {
      bool rc = false;
      QString str;

      while (true) {
        if (qName == "config") {
        }
        else if (qName == "config_global") {
          str = attributes.value("name");
          if (str.isEmpty()) {
            error_ = "No item name specified.";
            break;
          }
          if (str == "namespace") {
            str = attributes.value("value");
            if (!str.isEmpty())
              generator_.addNamespace(str);
            else {
              error_ = "Anonymuous namespace specified.";
              break;
            }
          }
          if (str == "include") {
            str = attributes.value("value");
            if (!str.isEmpty())
              generator_.addinclude(str);
            else {
              error_ = "Include statement without file name.";
              break;
            }
          }
          if (str == "Include") {
            str = attributes.value("value");
            if (!str.isEmpty())
              generator_.addInclude(str);
            else {
              error_ = "Include statement without file name.";
              break;
            }
          }
          if (str == "src_Include") {
            str = attributes.value("value");
            if (!str.isEmpty())
              generator_.addSrcInclude(str);
            else {
              error_ = "Source Include statement without file name.";
              break;
            }
          }
          if (str == "forward") {
            str = attributes.value("value");
            if (!str.isEmpty())
              generator_.addForwardDeclaration(str);
            else {
              error_ = "Forward declaration statement without name.";
              break;
            }
          }
          if (str == "local_forward") {
            str = attributes.value("value");
            if (!str.isEmpty())
              generator_.addLocalForwardDeclaration(str);
            else {
              error_ = "Local forward declaration statement without name.";
              break;
            }
          }
        }
        else if (qName == "config_group") {
          str = attributes.value("name");
          if (str.isEmpty()) {
            error_ = "No group name specified.";
            break;
          }
#if QT_VERSION >= 0x040000
          str[0] = str[0].toUpper();
#else
          str[0] = str[0].upper();
#endif
          group_ = str;
        }
        else if (qName == "config_item") {

          type_ = Type();
#if JSONCPP_FOUND
	  type_.setUseJson(useJson_);
#endif

          // get class name
          str = attributes.value("name");
          if (str.isNull())
            str = "";

          parsing_ = true;
          type_.setName(str);

          // get parent
          str = attributes.value("parent");
          if (!str.isEmpty()) {
            type_.setParent(str + "Parameters");
          }

          // final attribute
          str = attributes.value("final");
          if (!str.isEmpty())
            type_.setFinal(str != "false");

          // dummy attribute
          str = attributes.value("dummy");
          if (!str.isEmpty())
            type_.setDummy(str == "true");

          // dummy attribute
          str = attributes.value("extern");
          if (!str.isEmpty())
            type_.setExtern(str == "true");

          // get instance tag
          QString instance = attributes.value(QString("instance"));
          if (instance == QString("true")) {
            instance_ = true;
          }
          else if (instance == QString("false")) {
            instance_ = false;
          }
          else if (instance == QString("unmanaged")) {
            instance_ = true;
            unmanaged_ = true;
          } 
          else if (!instance.isEmpty()) {
            instance_ = true;
            userSingleton_ = true;
            userSingletonName_ = instance;
          } 
        }
        else if (parsing_) {
          if (qName == "config_parameter") {
            parameterParsing_ = true;

            QString name = attributes.value("name");
            if (name.isEmpty()) {
              error_ = "No parameter name specified.";
              break;
            }
	    currentParameterName = name;

#if QT_VERSION >= 0x040000
            name[0] = name[0].toLower();
#else
            name[0] = name[0].lower();
#endif
            QString type = attributes.value("type");
            if (type.isEmpty()) {
              error_ = "No parameter type specified.";
              break;
            }
            QString values;

            if (type == "string") {
              type = "std::" + type;
              string_ = true;
            }
            else if (type == "std::string") {
              string_ = true;
            }
            else if (type == "Text") {
              type = "Miro::" + type;
              text_ = true;
            }
            else if (type == "Miro::Text") {
              text_ = true;
            }
            else if (type == "Enumeration" || type == "Miro::Enumeration") {
              if (type == "Enumeration")
                type = "Miro::" + type;
              enumeration_ = true;

              values = attributes.value("values");
              if (values.isEmpty()) {
                error_ = "No enumeration values specified.";
                break;
              }
            }
            else if (type == "EnumerationMultiple" || type == "Miro::EnumerationMultiple") {
              if (type == "EnumerationMultiple")
                type = "Miro::" + type;
              enumerationMultiple_ = true;

              values = attributes.value("values");
              if (values.isEmpty()) {
                error_ = "No enumeration values specified.";
                break;
              }
            }
            else if (type.startsWith("vector<")) {
              type = "std::" + type;
              vector_ = true;
            }
            else if (type.startsWith("std::vector<")) {
              vector_ = true;
            }
            else if (type.startsWith("set<")) {
              type = "std::" + type;
              set_ = true;
            }
            else if (type.startsWith("std::set<")) {
              set_ = true;
            }
            else if (type == "Angle") {
              type = "Miro::" + type;
            }
            else if (type == "angle" || type == "Miro::Angle") {
              angle_ = true;
            }
            else if (type == "ACE_Time_Value") {
              timeValue_ = true;
            }
            else if (type == "ACE_INET_Addr") {
              inetAddr_ = true;
            }
            else if (type == "ACE_TTY_IO::Serial_Params") {
              serialParams_ = true;
            }
            else if (type == "ACE_Sched_Params") {
              schedParams_ = true;
            }
            else if (type == "ScanDescriptionIDL") {
              type = "Miro::" + type;
              scanDescription_ = true;
            }
            else if (type == "Miro::ScanDescriptionIDL") {
              scanDescription_ = true;
            }

            QString def = attributes.value("default");
            if (def.isNull())
              def = "";

            // parse for common ACE_Time_Value default error
            QString fullDef;
#if QT_VERSION >= 0x040000
            if (type == "ACE_Time_Value" && def.contains('.'))
#else
            if (type == "ACE_Time_Value" && def.find('.') != -1)
#endif
	    {
              error_ = "ACE_Time_Value format is (x, y), no dot.";
              break;
            }
            else if (type == "std::string" ||
                     type == "Miro::Text" ||
                     type == "Miro::BAP::TransitionMessage")
              fullDef = "\"" + def + "\"";
            else if (type == "Miro::Enumeration") {
              if (def == "") {
                error_ = "Enumeration definition needs default value.";
                break;
              }
              fullDef = "\"" + def + "\", \"" + values + "\"";
            }
            else if (type == "Miro::EnumerationMultiple") {
              if (def == "") {
                error_ = "EnumerationMultiple definition needs default value.";
                break;
              }
              fullDef = "\"" + def + "\", \"" + values + "\"";
            }
            else if (type == "Miro::Angle")
              fullDef = "Miro::deg2Rad(" + def + ")";
            else if (type == "angle")
              fullDef = "Miro::deg2Rad(" + def + ")";
            else if (type == "char")
              fullDef = "'" + def + "'";
            else if (type == "ACE_INET_Addr")
              fullDef = "\"" + def + "\"";
            else
              fullDef = def;

            // static const parameter parsing
            QString staticConst = attributes.value("static_const");
            staticConst_ = (staticConst == "true");

            if (staticConst_) {
              if (def.isEmpty()) {
                error_ = "Static const declaration without default value for " + type_.name();
                break;
              }
              // XXX mallan: qstring generating ambiguous overload errors on windows
              //for (unsigned int i = 0; i < name.length(); ++i) {
              //  name[i] = name[i].upper();
              //}
#if QT_VERSION >= 0x040000
              name = name.toUpper();
#else
              name = name.upper();
#endif
            }

            // inherited parameter handling
            str = attributes.value("inherited");
            if (!str.isEmpty() && str == QString("true")) {
              if (def.isEmpty())  {
                error_ = "Parameter taged inherited without default value.";
                break;
              }
              if (staticConst_)  {
                error_ = "Static const parameter can't be taged inherited.";
                break;
              }

              type_.addToConstructor(name + QString(" = ") + fullDef + QString(";"));
            }
            else {
              QString unit = attributes.value("measure");
              if (unit == "")
                unit = attributes.value("unit");

              if (!staticConst_) {
                type_.addParameter(Parameter(type, name, def, fullDef, unit, docu_));
              }
              else {
                type_.addStaticConstParameter(Parameter(type, name, def, fullDef, unit, docu_));
              }
            }
          }
          else if (qName == "constructor") {
            ctorParsing_ = true;
            ctor_ = "";
          }
          else if (qName == "documentation") {
            docuParsing_ = true;
            docu_ = "";
          }
          else {
            error_ = "Unknown tag name: " + qName;
            break;
          }
        }
        else {
          error_ = "Unknown tag name: " + qName;
          break;
        }

        rc = true;
        break;
      }
      return rc;
    }

    bool
    Parser::endElement(QString const&, QString const&, QString const& qName)
    {
      bool rc = false;
      while (true) {
        if (parsing_ && qName == "config_item") {
          if (type_.isExtern() && !type_.parent().isEmpty()) {
            error_ = "Type " + type_.name() + " defined extern but also has a parent.";
            break;
          }
          if (instance_ && !unmanaged_ && !userSingleton_) {
            generator_.addInclude("miro/Singleton.h");
            type_.setInstance();
          }
          if (instance_ && unmanaged_) {
            generator_.addInclude("miro/Singleton.h");
            type_.setUnmanagedInstance();
          }
          if (instance_ && userSingleton_) {
            type_.setUserInstance(userSingletonName_);
          }
          if (string_)
            generator_.addInclude("string");
          if (text_)
            generator_.addInclude("miro/Text.h");
          if (enumeration_)
            generator_.addInclude("miro/Enumeration.h");
          if (enumerationMultiple_)
            generator_.addInclude("miro/EnumerationMultiple.h");
          if (vector_) {
            generator_.addInclude("vector");
            generator_.addSrcInclude("miro/MiroIO.h");
          }
          if (set_) {
            generator_.addInclude("set");
            generator_.addSrcInclude("miro/MiroIO.h");
          }
          if (angle_)
            generator_.addInclude("miro/Angle.h");
          if (timeValue_) {
            generator_.addInclude("ace/Time_Value.h");
            generator_.addSrcInclude("miro/TimeHelper.h");
            generator_.addSrcInclude("miro/XmlParseAce.h");
#if JSONCPP_FOUND
	    if (useJson_) {
		generator_.addSrcInclude("miro/JsonParseAce.h");
	    }
#endif
          }
          if (serialParams_) {
            generator_.addInclude("ace/TTY_IO.h");
            generator_.addSrcInclude("miro/MiroIO.h");
            generator_.addSrcInclude("miro/XmlParseAce.h");
#if JSONCPP_FOUND
	    if (useJson_) {
		generator_.addSrcInclude("miro/JsonParseAce.h");
	    }
#endif
          }
          if (schedParams_) {
            generator_.addInclude("ace/Sched_Params.h");
            generator_.addSrcInclude("miro/MiroIO.h");
            generator_.addSrcInclude("miro/XmlParseAce.h");
#if JSONCPP_FOUND
	    if (useJson_) {
		generator_.addSrcInclude("miro/JsonParseAce.h");
	    }
#endif
          }
          if (inetAddr_) {
            generator_.addInclude("ace/INET_Addr.h");
            generator_.addSrcInclude("miro/MiroIO.h");
            generator_.addSrcInclude("miro/XmlParseAce.h");
#if JSONCPP_FOUND
	    if (useJson_) {
		generator_.addSrcInclude("miro/JsonParseAce.h");
	    }
#endif
          }
          if (scanDescription_) {
            generator_.addInclude("idl/ScanDescriptionC.h");
            generator_.addSrcInclude("idl/RangeSensorC.h");
            generator_.addSrcInclude("miro/ScanDescriptionHelper.h");
          }
#if JSONCPP_FOUND
	  if (useJson_) {
	    generator_.addSrcInclude("miro/JsonCpp.h");
	  }
#endif
          generator_.addType(group_, type_);

          reset();
        }
        else if (parameterParsing_ && qName == "config_parameter") {
	  currentParameterName = "";
          parameterParsing_ = false;
        }
        else if (ctorParsing_ && qName == "constructor") {
          ctorParsing_ = false;
          type_.addToConstructor(ctor_);
          ctor_ = "";
        }
        else if (docuParsing_ && qName == "documentation") {
          docuParsing_ = false;
	  if (parameterParsing_) {
	    type_.addToParameterDocumentation(currentParameterName, docu_);
	  }
	  else {
	    type_.addToDocumentation(docu_);
	  }
          docu_ = "";
        }

        rc = true;
        break;
      }
      return rc;
    }

    QString
    Parser::errorString() const {
      return error_;
    }

    bool
    Parser::characters(QString const& ch)
    {
      if (ctorParsing_) {
        ctor_ += ch;
      }
      if (docuParsing_) {
        docu_ += ch;
      }
      return true;
    }
  }
}
