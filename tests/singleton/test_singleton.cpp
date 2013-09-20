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
#include "libA/libA.h"
#include "libB/libB.h"
#include "libC/libC.h"

#include <ace/Singleton.h>

#include <iostream>

using namespace std;

int main(int, char**)
{

    cout << "lib A singleton <int>" << libA() << endl;
    cout << "lib B singleton <int>" << libB() << endl;
    cout << " main singleton <int>" <<
         (void *)TestIntSingleton::instance () << endl;

    void* a = libA();
    void* b = libB();
    void* c = (void *)TestIntSingleton::instance();

    if ( a == b && b == c ) {
        cout << "PASS" << endl;
        return 0;
    }
    else {
        cout << "FAIL" << endl;
        return -1;
    }
}
