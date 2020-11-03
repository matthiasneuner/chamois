#include "ChamoisApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
ChamoisApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy DirichletBC, that is, set DirichletBC default for preset = true
  params.set<bool>("use_legacy_dirichlet_bc") = false;

  return params;
}

ChamoisApp::ChamoisApp(InputParameters parameters) : MooseApp(parameters)
{
  ChamoisApp::registerAll(_factory, _action_factory, _syntax);
}

ChamoisApp::~ChamoisApp() {}

void
ChamoisApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"ChamoisApp"});
  Registry::registerActionsTo(af, {"ChamoisApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
ChamoisApp::registerApps()
{
  registerApp(ChamoisApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
ChamoisApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ChamoisApp::registerAll(f, af, s);
}
extern "C" void
ChamoisApp__registerApps()
{
  ChamoisApp::registerApps();
}
