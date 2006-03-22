//  
//  For more information, please see: http://software.sci.utah.edu
//  
//  The MIT License
//  
//  Copyright (c) 2004 Scientific Computing and Imaging Institute,
//  University of Utah.
//  
//  License for the specific language governing rights and limitations under
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//  
 
#include <Core/Datatypes/String.h>
#include <Dataflow/Ports/StringPort.h>
#include <Dataflow/Network/Module.h>
#include <Core/Malloc/Allocator.h>

namespace SCIRun {

using namespace SCIRun;

class JoinStrings : public Module {
public:
  JoinStrings(GuiContext*);

  virtual ~JoinStrings();

  virtual void execute();
};


DECLARE_MAKER(JoinStrings)
JoinStrings::JoinStrings(GuiContext* ctx)
  : Module("JoinStrings", ctx, Source, "String", "SCIRun")
{
}

JoinStrings::~JoinStrings(){
}

void  JoinStrings::execute()
{
  StringHandle input;
  
  std::string str = "";
    
  int p = 0;
  StringIPort *iport;
  while(p < num_input_ports())
  {
  
      iport = dynamic_cast<StringIPort *>(get_iport(p));
      if (iport)
      {
        iport->get(input);
        if (input.get_rep())
        {
          str += input->get();
          input = 0;
        }
        else
        {
          break;
        }
      }
      p++;
  }
  
  StringOPort *oport;
  
  oport = dynamic_cast<StringOPort *>(get_oport(0));
  if (oport == 0)
  {
    error("Could not locate output port");
    return;
  }
  
  StringHandle output(scinew String(str));
  oport->send_and_dereference(output);
}

} // End namespace SCIRun


