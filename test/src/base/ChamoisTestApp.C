//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "ChamoisTestApp.h"
#include "ChamoisApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
ChamoisTestApp::validParams()
{
  InputParameters params = ChamoisApp::validParams();
  return params;
}

ChamoisTestApp::ChamoisTestApp(InputParameters parameters) : MooseApp(parameters)
{
  ChamoisTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

ChamoisTestApp::~ChamoisTestApp() {}

void
ChamoisTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  ChamoisApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"ChamoisTestApp"});
    Registry::registerActionsTo(af, {"ChamoisTestApp"});
  }
}

void
ChamoisTestApp::registerApps()
{
  registerApp(ChamoisApp);
  registerApp(ChamoisTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
ChamoisTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ChamoisTestApp::registerAll(f, af, s);
}
extern "C" void
ChamoisTestApp__registerApps()
{
  ChamoisTestApp::registerApps();
}
