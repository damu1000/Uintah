/*
  The contents of this file are subject to the University of Utah Public
  License (the "License"); you may not use this file except in compliance
  with the License.
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.
  
  The Original Source Code is Vtk, released March 12, 2001.
  
  The Original Source Code was developed by the University of Utah.
  Portions created by UNIVERSITY are Copyright (C) 2001, 1994 
  University of Utah. All Rights Reserved.
*/

/*
 *  VtkUIPort.cc: CCA-style Interface to old TCL interfaces
 *
  *  Written by:
 *   Keming Zhang
 *   Department of Computer Science
 *   University of Utah
 *   January 2004
 *
 */

#include <SCIRun/Vtk/VtkUIPort.h>
#include <SCIRun/Vtk/VtkComponentInstance.h>
#include <iostream>
using namespace SCIRun;

VtkUIPort::VtkUIPort(VtkComponentInstance* ci)
  : ci(ci)
{
}

VtkUIPort::~VtkUIPort()
{
}

int 
VtkUIPort::ui()
{
  //  vtk::Component* component=ci->getComponent();
  
  //TODO: add popupUI()
  ci->getComponent()->popupUI();

  //TODO: need return correct value: 0 success, -1 fatal error, 
  //other values for other errors.
  return 0;
}


