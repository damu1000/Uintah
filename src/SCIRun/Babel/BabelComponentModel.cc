/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2004 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


/*
 *  BabelComponentModel.cc:
 *
 *  Written by:
 *   Keming Zhang
 *   Department of Computer Science
 *   University of Utah
 *   July 2002
 *
 */

#include <SCIRun/Babel/BabelComponentModel.h>
#include <SCIRun/Babel/BabelComponentDescription.h>
#include <SCIRun/Babel/BabelComponentInstance.h>
#include <SCIRun/SCIRunFramework.h>
#include <SCIRun/SCIRunErrorHandler.h>
#include <Core/Containers/StringUtil.h>
#include <Core/OS/Dir.h>
#include <Dataflow/XMLUtil/StrX.h>
#include <Dataflow/XMLUtil/XMLUtil.h>
#include <Core/Util/soloader.h>
#include <Core/Util/Environment.h>
#include <Core/CCA/PIDL/PIDL.h>
#include <string>
#include "framework.hh"
#include "sidl.hh"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

#ifdef __sgi
#define IRIX
#pragma set woff 1375
#endif
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#ifdef __sgi
#pragma reset woff 1375
#endif

#include <iostream>

namespace SCIRun {

BabelComponentModel::BabelComponentModel(SCIRunFramework* framework)
  : ComponentModel("babel"), framework(framework)
{
  // Record the path to XML descriptions of components.  The environment
  // variable SIDL_XML_PATH should be set, otherwise use a default.
  const char *component_path = getenv("SIDL_XML_PATH");
  if (component_path != 0) {
    this->setSidlXMLPath( std::string(component_path) );
  } else {
    this->setSidlXMLPath(sci_getenv("SCIRUN_SRCDIR") + std::string("/CCA/Components/BabelTest/xml"));
  }
  
  // Now append the SIDL_XML_PATH to the sidl::Loader search path.  This
  // is the path that sidl::Loader will search for .scl files.  Babel .scl
  // files are necessary for mapping component names with their DLLs.
  std::string search_path = sidl::Loader::getSearchPath();
  if (search_path.find(this->getSidlXMLPath()) == std::string::npos)
    {// SIDL_XML_PATH is not already in the sidl runtime search path
    sidl::Loader::addSearchPath( this->getSidlXMLPath() );     
    }
  
  buildComponentList();
}

BabelComponentModel::~BabelComponentModel()
{
  destroyComponentList();
}

void BabelComponentModel::destroyComponentList()
{
  for(componentDB_type::iterator iter=components.begin();
      iter != components.end(); iter++)
    {
    delete iter->second;
    }
  components.clear();
}

void BabelComponentModel::buildComponentList()
{
  // Initialize the XML4C system
  try
    {
    XMLPlatformUtils::Initialize();
    }
  catch (const XMLException& toCatch)
    {
    std::cerr << "Error during initialization! :\n"
              << StrX(toCatch.getMessage()) << std::endl;
    return;
    }

  destroyComponentList();

  std::string component_path(this->getSidlXMLPath());

  while(component_path != "")
    {
    unsigned int firstColon = component_path.find(';');
    std::string dir;
    if(firstColon < component_path.size())
      {
      dir=component_path.substr(0, firstColon);
      component_path = component_path.substr(firstColon+1);
      }
    else
      {
      dir = component_path;
      component_path="";
      }
    Dir d(dir);

    std::cerr << "BabelComponentModel: Looking at directory: " << dir << std::endl;

    std::vector<std::string> files;
    d.getFilenamesBySuffix(".cca", files);
    for(std::vector<std::string>::iterator iter = files.begin();
        iter != files.end(); iter++)
      {
      std::string& file = *iter;
      std::cerr << "BabelComponentModel: Parsing file " << file << std::endl;
      readComponentDescription(dir+"/"+file);
      }
    }
}

void BabelComponentModel::readComponentDescription(const std::string& file)
{
  // Instantiate the DOM parser.
  XercesDOMParser parser;
  parser.setDoValidation(false);
  
  SCIRunErrorHandler handler;
  parser.setErrorHandler(&handler);
  
  try
    {
    parser.parse(file.c_str());
    }
  catch (const XMLException& toCatch)
    {
    std::cerr << "Error during parsing: '" <<
      file << "'\nException message is:  " <<
      xmlto_string(toCatch.getMessage()) << std::endl;
    handler.foundError=true;
    return;
    }
  
  DOMDocument* doc = parser.getDocument();
  DOMNodeList* list = doc->getElementsByTagName(to_xml_ch_ptr("component"));
  int nlist = list->getLength();
  if(nlist == 0)
    {
    std::cerr << "WARNING: file " << file << " has no components!" << std::endl;
    }
  for (int i=0;i<nlist;i++)
    {    
    DOMNode* d = list->item(i);
    BabelComponentDescription* cd = new BabelComponentDescription(this);

    // Is this component a Babel component?
    DOMNode* model= d->getAttributes()->getNamedItem(to_xml_ch_ptr("model"));
    if (model != 0)
      { 
      if ( strcmp(to_char_ptr(model->getNodeValue()), this->prefixName.c_str()) != 0 )
        {// not a babel component, ignore it
        continue;
        }
      }
    else
      { // no model, ignore this component
      std::cerr << "ERROR: Component has no model in file" << file << std::endl;
      continue;
      }

    DOMNode* name = d->getAttributes()->getNamedItem(to_xml_ch_ptr("name"));
    if (name==0)
      {
      std::cout << "ERROR: Component has no name." << std::endl;
      cd->type = "unknown type";
      }
    else
      {
      cd->type = to_char_ptr(name->getNodeValue());
      }
    
    componentDB_type::iterator iter = components.find(cd->type);
    if(iter != components.end())
      {
      std::cerr << "WARNING: Component multiply defined: " << cd->type << std::endl;
      }
    else
      {
        std::cerr << "Added Babel component of type: " << cd->type << std::endl;
        components[cd->type]=cd;
      }
    }
}

gov::cca::Services
BabelComponentModel::createServices(const std::string& instanceName,
				  const std::string& className,
				  const gov::cca::TypeMap& properties)
{
  /*
  gov::cca::Component nullCom;
  gov::cca::Services svc;
  cerr<<"need associate svc with ci in createServices!"<<endl;
  BabelComponentInstance* ci = new BabelComponentInstance(framework,
							  instanceName, className,
							  properties,
							  nullCom,
							  svc);
  framework->registerComponent(ci, instanceName);

  //ci->addReference();

  */
  gov::cca::Services svc;
  std::cerr << "BabelComponentModel::createServices() is not implemented !"
            << std::endl;
  return svc;
}

bool BabelComponentModel::haveComponent(const std::string& type)
{
  std::cerr << "CCA(Babel) looking for babel component of type: " << type
            << std::endl;
  return components.find(type) != components.end();
}

ComponentInstance* BabelComponentModel::createInstance(const std::string& name,
						       const std::string& type)
{
  gov::cca::Component component;
  if (true)
    {  //local component 

    componentDB_type::iterator iter = components.find(type);
    
    if(iter == components.end())
      {
      std::cerr << "ERROR: Component " << type << " is not a registered component."
                << std::endl;
      return 0;
      }

    /*
    std::string lastname=type.substr(type.find('.')+1);  
    std::string so_name("lib/libBabel_Components_");
    so_name=so_name+lastname+".so";
    cerr<<"type="<<type<<" soname="<<so_name<<std::endl;

    LIBRARY_HANDLE handle = GetLibraryHandle(so_name.c_str());
    if(!handle){
      cerr << "Cannot load component .so " << type << '\n';
      cerr << SOError() << '\n';
      return 0;
    }

    std::string makername = "make_"+type;
    for(int i=0;i<(int)makername.size();i++)
      if(makername[i] == '.')
	makername[i]='_';
    
    cerr<<"looking for symbol:"<< makername<<std::endl;
    void* maker_v = GetHandleSymbolAddress(handle, makername.c_str());
    if(!maker_v){
      cerr <<"Cannot load component symbol " << type << '\n';
      cerr << SOError() << '\n';
      return 0;
    }
    gov::cca::Component (*maker)() = (gov::cca::Component (*)())(maker_v);
    cerr<<"about to create babel component"<<std::endl;
    component = (*maker)();
    */
    
    // sidl::BaseClass sidl_class = sidl::Loader::createClass(type);
    // For upgrade to babel 0.9.0 --josh 4/21/04
    sidl::DLL library = sidl::Loader::findLibrary(type, "ior/impl",
                  ::sidl::Scope_SCLSCOPE, ::sidl::Resolve_SCLRESOLVE);
    std::cout << "sidl::Loader::getSearchPath=" << sidl::Loader::getSearchPath() << std::endl;
    if (library._not_nil())
      {
      std::cerr << "Found library for class " << type << std::endl;
      }
    else
      {
      std::cerr << "FATAL ERROR: Could not find library for type " << type
                << std::endl;
      return 0;
      }

    sidl::BaseClass sidl_class = library.createClass(type);

    // end upgrade

    // jc--why is this assignment necessary??
    component = sidl_class;
    if ( component._not_nil() )
      { 
      std::cerr << "babel component of type " << type << " is loaded!"
                << std::endl;
      }
    else
      {
      std::cerr << "Cannot load babel component of type " << type << std::endl;
      std::cerr << "FATAL ERROR: babel component not created!" << std::endl;
      return 0;
      }
      
  }
  else{ //remote component: need to be created by framework at url 
    std::cerr << "remote babel components creation is not done!" << std::endl;
    /*
    Object::pointer obj=PIDL::objectFrom(url);
    if(obj.isNull()){
      cerr<<"got null obj (framework) from "<<url<<std::endl;
      return 0;
    }

    sci::cca::AbstractFramework::pointer remoteFramework=
      pidl_cast<sci::cca::AbstractFramework::pointer>(obj);

    std::string comURL=remoteFramework->createComponent(name, type);
    //cerr<<"comURL="<<comURL<<std::endl;
    Object::pointer comObj=PIDL::objectFrom(comURL);
    if(comObj.isNull()){
      cerr<<"got null obj(Component) from "<<url<<std::endl;
      return 0;
    }
    component=pidl_cast<sci::cca::Component::pointer>(comObj);
    */
  }


  std::cerr<<"about to create services"<<std::endl;
  framework::Services svc=framework::Services::_create();
  std::cerr<<"services created !"<<std::endl;
  component.setServices(svc);
  std::cerr<<"component.setService done!"<<std::endl;
  gov::cca::Component nullMap;

  BabelComponentInstance* ci = new BabelComponentInstance(framework, name, type,
							  nullMap, 
							  component,
							  svc);
  std::cerr<<"comopnent instance ci is created!"<<std::endl;
  //ci->addReference();
  return ci;
}

bool BabelComponentModel::destroyInstance(ComponentInstance *ci)
{
  std::cerr<<"BabelComponentModel::destroyInstance() is not done"<<std::endl;
  //make sure why ci->addReference() is called in createInstace();
  delete ci;  
  return false;
}

std::string BabelComponentModel::getName() const
{
  return "babel";
}

void BabelComponentModel::listAllComponentTypes(std::vector<ComponentDescription*>& list,
					      bool /*listInternal*/)
{
  for(componentDB_type::iterator iter=components.begin();
      iter != components.end(); iter++){
    list.push_back(iter->second);
  }
}



std::string BabelComponentModel::createComponent(const std::string& name,
						      const std::string& type)
						     
{
  
  sci::cca::Component::pointer component;
  componentDB_type::iterator iter = components.find(type);
  if(iter == components.end())
    return "";
  
  std::string lastname=type.substr(type.find('.')+1);  
  std::string so_name("lib/libBabel_Components_");
  so_name=so_name+lastname+".so";
  //cerr<<"type="<<type<<" soname="<<so_name<<std::endl;
  
  LIBRARY_HANDLE handle = GetLibraryHandle(so_name.c_str());
  if(!handle){
    std::cerr << "Cannot load component " << type << '\n';
    std::cerr << SOError() << '\n';
    return "";
  }

  std::string makername = "make_"+type;
  for(int i=0;i<(int)makername.size();i++)
    if(makername[i] == '.')
      makername[i]='_';
  
  void* maker_v = GetHandleSymbolAddress(handle, makername.c_str());
  if(!maker_v){
    std::cerr << "Cannot load component " << type << '\n';
    std::cerr << SOError() << '\n';
    return "";
  }
  /*  sci::cca::Component::pointer (*maker)() = (sci::cca::Component::pointer (*)())(maker_v);
  component = (*maker)();
  //need to make sure addReference() will not cause problem
  component->addReference();
  return component->getURL().getString();
  */
  return ""; 
}

} // end namespace SCIRun
