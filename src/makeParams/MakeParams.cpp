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
#define QT_ALTERNATE_QTSMANIP

#include <ace/Get_Opt.h>

#include "params/Parser.h"
#include "params/Generator.h"
#include "params/TextErrorHandler.h"

#include <qstring.h>
#include <qfile.h>
#include <qxml.h>

#include <iostream>
#include <fstream>

#include "qt_compatibility.h"

using std::cout;
using std::cerr;
using std::ofstream;

using namespace Miro::CFG;

bool verbose = false;
#if JSONCPP_FOUND
bool useJson = false;
#endif

QString baseName = "Parameters";
QString headerExtension = "h";
QString sourceExtension = "cpp";
QString fileName = "Parameters.xml";
QString exportDirective;

int
parseArgs(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts(argc, argv, "f:h:n:s:x:vj?");

  while ((c = get_opts()) != -1) {
    switch (c) {
      case 'f':
        fileName = get_opts.optarg;
        break;
      case 'h':
        headerExtension = get_opts.optarg;
        break;
      case 'n':
        baseName = get_opts.optarg;
        break;
      case 's':
        sourceExtension = get_opts.optarg;
        break;
      case 'v':
        verbose = true;
        break;
      case 'x':
        exportDirective = get_opts.optarg;
        break;
#if JSONCPP_FOUND
      case 'j':
	useJson = true;
        break;
#endif
      case '?':
      default:
        cerr << "usage: " << argv[0] << "[-f file] [-i item] [-s=source] [-h=header] [-v?]" << std::endl
        << "  -f <file> name of the input file (Parameters.xml)" << std::endl
        << "  -n <name> base name of the output file (Parameters)" << std::endl
        << "  -s <extension> extension of the generated source file (cpp)" << std::endl
        << "  -h <extension> extension of the generated header file (h)" << std::endl
        << "  -x <directive> add export directive to generated header" << std::endl
#if JSONCPP_FOUND
	<< "  -j Include JSON support in generated files" << std::endl
#endif
        << "  -v verbose mode" << std::endl
        << "  -? help: emit this text and stop" << std::endl;
        rc = 1;
    }

    if (verbose) {
      cout << "file name: " << fileName << std::endl
      << "base name: " << baseName << std::endl
      << "source extension: " << sourceExtension << std::endl
      << "header extension: " << headerExtension << std::endl;
    }
  }
  return rc;
}

int
main(int argc, char * argv[])
{
  int rc;
  if ((rc = parseArgs(argc, argv)) == 0) {
    try {
      Generator generator;
      generator.setBaseName(baseName);
      generator.setHeaderExtension(headerExtension);

      Parser handler(generator);
#if JSONCPP_FOUND
      handler.setUseJson(useJson);
#endif

      QFile xmlFile(fileName);
      QXmlInputSource source(&xmlFile);
      QXmlSimpleReader reader;
      TextErrorHandler errorHandler;

      reader.setContentHandler(&handler);
      reader.setErrorHandler(&errorHandler);
      if (verbose)
        cout << "parsing " << fileName << std::endl;
      bool success = reader.parse(source);

      if (success) {

        if (verbose)
          cout << "generating  " << (baseName + "." + sourceExtension) << std::endl;
        QString sourceFilename(baseName + "." + sourceExtension);
        ofstream sourceFile(qPrintable(sourceFilename));
        if (verbose) {
          cout << "generating  " << (baseName + "." + headerExtension) << std::endl;
        }
        QString headerFilename(baseName + "." + headerExtension);
        ofstream headerFile(qPrintable(headerFilename));

        generator.generateSource(sourceFile);
        generator.generateHeader(headerFile, exportDirective);
      }
      else {
        cerr << "Error parsing " << fileName << std::endl
        << errorHandler.errorString();
        rc = 1;
      }
    }
    catch (const QString& e) {
      cerr << "Exception: " << e << std::endl;
      rc = 1;
    }
  }
  return rc;
}
