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
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <QFile>
#include <QTextStream>

#include "MiroConfig.h"

#include "miro/JsonCpp.h"

#include "SimpleSurveyParameters.h"

using namespace std;

bool endsWith(const string& s, const string& suffix)
{
  string::size_type p = s.rfind(suffix);
  return ((p != string::npos)
          && (p == s.size() - suffix.size()));
}

SimpleSurveyParameters readJson(const string& jsonName)
{
  ifstream jsonStream(jsonName.c_str());
  if (!jsonStream) {
    cerr << "couldn't open " << jsonName << " for reading: " << strerror(errno) << endl;
    exit(1);
  }

  Json::Value root;
  Json::Reader reader;
  bool success = reader.parse(jsonStream, root);
  if (!success) {
    cerr << reader.getFormatedErrorMessages() << endl;
    exit(1);
  }
  jsonStream.close();

  SimpleSurveyParameters params;
  params <<= root;
  return params;
}

void writeJson(const string& jsonName, const SimpleSurveyParameters& params)
{
  Json::Value root;
  params >>= root;

  ofstream jsonStream(jsonName.c_str());
  if (!jsonStream) {
    cerr << "couldn't open " << jsonName << " for writing: " << strerror(errno) << endl;
    exit(1);
  }

  Json::StyledWriter writer;
  jsonStream << writer.write(root);
  jsonStream.close();

  cout << "wrote output to " << jsonName << endl;
}

SimpleSurveyParameters readXml(const string& xmlName)
{
  QFile xmlFile(xmlName.c_str());
  if (!xmlFile.open(QIODevice::ReadOnly)) {
    cerr << "couldn't open " << xmlName << " for reading: "
         << ((const char*) xmlFile.errorString().toAscii()) << endl;
    exit(1);
  }

  QDomDocument doc("SurveyPlan");
  if (!doc.setContent(&xmlFile)) {
    xmlFile.close();
    cerr << "couldn't parse " << xmlName << " as xml" << endl;
    exit(1);
  }
  QDomElement e = doc.documentElement();
  QDomNode n = e.firstChild();

  SimpleSurveyParameters params;
  params <<= n;
  return params;
}

void writeXml(const string& xmlName, const SimpleSurveyParameters& params)
{
  QDomDocument doc("SurveyPlan");
  QDomElement plan = doc.createElement("plan");
  doc.appendChild(plan);
  params >>= plan;

  QFile xmlFile(xmlName.c_str());
  if (!xmlFile.open(QIODevice::WriteOnly)) {
    cerr << "couldn't open " << xmlName << " for writing: "
         << ((const char*) xmlFile.errorString().toAscii()) << endl;
    exit(1);
  }
  QTextStream ts(&xmlFile);
  doc.save(ts, 2);
  ts.flush();
  xmlFile.close();

  cout << "wrote output to " << xmlName << endl;
}

void usage(void)
{
  cerr << "usage: convertParams <in> <out>" << endl;
  exit(1);
}

int main(int argc, const char *argv[])
{
  if (argc != 3) {
    usage();
  }
  string input = argv[1];
  string output = argv[2];

  SimpleSurveyParameters params;
  if (endsWith(input, ".json")) {
    params = readJson(input);
  }
  else {
    params = readXml(input);
  }
  if (endsWith(output, ".json")) {
    writeJson(output, params);
  }
  else {
    writeXml(output, params);
  }

  return 0;
}
