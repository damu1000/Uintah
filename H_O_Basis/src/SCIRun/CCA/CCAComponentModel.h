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
 *  CCAComponentModel.h: 
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   October 2001
 *
 */

#ifndef SCIRun_CCA_CCAComponentModel_h
#define SCIRun_CCA_CCAComponentModel_h

#include <SCIRun/ComponentModel.h>
#include <SCIRun/ComponentInstance.h>
#include <Core/CCA/spec/cca_sidl.h>
#include <vector>
#include <string>
#include <map>

namespace SCIRun {

class SCIRunFramework;
class CCAComponentDescription;
class CCAComponentInstance;

/**
 * \class CCAComponentModel
 *
 * A SCIRun metacomponent model for CCA compliant components.  This class
 * handles the allocation/destruction of CCA components and maintains a
 * database of CCA components that may be instantiated by the SCIRun
 * framework. See ComponentModel for more information.
 *
 * \sa BabelComponentModel InternalComponentModel SCIRunComponentModel VtkComponentModel
*/
class CCAComponentModel : public ComponentModel
{
public:
  CCAComponentModel(SCIRunFramework* framework);
  virtual ~CCAComponentModel();

  /** ? */
  sci::cca::Services::pointer createServices(const std::string& instanceName,
                                             const std::string& className,
                                             const sci::cca::TypeMap::pointer& properties);

  /** ? */
  bool destroyServices(const sci::cca::Services::pointer& svc);

  /** Returns true if component type \em type has been registered with this
      component model.  In other words, returns true if this ComponentModel
      knows how to instantiate component \em type. */
  virtual bool haveComponent(const std::string& type);

  /** Allocates an instance of the component of type \em type.  The parameter
      \em name is assigned as the unique name of the newly created instance.
      Returns a smart pointer to the newly created instance, or a null pointer
      on failure. */
  virtual ComponentInstance* createInstance(const std::string& name,
                                            const std::string& type,
                                            const sci::cca::TypeMap::pointer& properties);
  
  /** Deallocates the component instance \em ci.  Returns \code true on success and
      \code false on failure. */
  virtual bool destroyInstance(ComponentInstance *ci);

  /** Returns the name (as a string) of this component model. */
  virtual std::string getName() const;
  
  /** Creates a list of all the available components (as ComponentDescriptions)
      registered in this ComponentModel. */
  virtual void listAllComponentTypes(std::vector<ComponentDescription*>&,
                                     bool);

  /** ? */
  virtual void destroyComponentList();

  /** ? */
  virtual void buildComponentList();

  /** ? */
  int addLoader(resourceReference *rr);

  /** ? */
  int removeLoader(const std::string &loaderName);

  static const std::string DEFAULT_PATH;
  
private:
  SCIRunFramework* framework;
  typedef std::map<std::string, CCAComponentDescription*> componentDB_type;
  componentDB_type components;
    
  void readComponentDescription(const std::string& file);
  std::string getSidlXMLPath();
  resourceReference *getLoader(std::string loaderName);

  CCAComponentModel(const CCAComponentModel&);
  CCAComponentModel& operator=(const CCAComponentModel&);
  
  std::vector<resourceReference*> loaderList;
};

} // end namespace SCIRun

#endif
