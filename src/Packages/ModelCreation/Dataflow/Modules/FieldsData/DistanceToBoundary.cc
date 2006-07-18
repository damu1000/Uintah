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


#include <Dataflow/Network/Module.h>
#include <Core/Malloc/Allocator.h>

#include <Dataflow/Network/Ports/FieldPort.h>
#include <Dataflow/Network/Ports/MatrixPort.h>

#include <Core/Datatypes/Field.h>
#include <Core/Datatypes/Matrix.h>

#include <Core/Algorithms/Fields/FieldsAlgo.h>

namespace ModelCreation {

using namespace SCIRun;

class DistanceToBoundary : public Module {
public:
  DistanceToBoundary(GuiContext*);
  virtual void execute();
};


DECLARE_MAKER(DistanceToBoundary)
DistanceToBoundary::DistanceToBoundary(GuiContext* ctx)
  : Module("DistanceToBoundary", ctx, Source, "FieldsData", "ModelCreation")
{
}

void DistanceToBoundary::execute()
{
  FieldHandle input,output;
  FieldHandle object;

  if (!(get_input_handle("Field",input,true))) return;  
  
  SCIRunAlgo::FieldsAlgo algo(this);

  MatrixHandle dummy;
  if(!(algo.FieldBoundary(input,object,dummy))) return;
  if(!(algo.DistanceField(input,output,object))) return;

  send_output_handle("DistanceField",output,false);
}

} // End namespace ModelCreation


