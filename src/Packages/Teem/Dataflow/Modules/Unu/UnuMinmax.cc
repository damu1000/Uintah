//  The contents of this file are subject to the University of Utah Public
//  License (the "License"); you may not use this file except in compliance
//  with the License.
//  
//  Software distributed under the License is distributed on an "AS IS"
//  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
//  License for the specific language governing rights and limitations under
//  the License.
//  
//  The Original Source Code is SCIRun, released March 12, 2001.
//  
//  The Original Source Code was developed by the University of Utah.
//  Portions created by UNIVERSITY are Copyright (C) 2001, 1994
//  University of Utah. All Rights Reserved.
//  

/*
 *  UnuMinmax.cc:  Print out min and max values in one or more nrrds. Unlike other
 *  modules, this doesn't produce a nrrd. It only prints to the UI the max values 
 *  found in the input nrrd(s), and it also indicates if there are non-existant values.
 *
 *  Written by:
 *   Darby Van Uitert
 *   April 2004
 *
 */

#include <Dataflow/Network/Module.h>
#include <Core/GuiInterface/GuiVar.h>
#include <Core/Malloc/Allocator.h>
#include <Core/Containers/StringUtil.h>

#include <Dataflow/share/share.h>

#include <Teem/Dataflow/Ports/NrrdPort.h>


namespace SCITeem {

using namespace SCIRun;

class PSECORESHARE UnuMinmax : public Module {
public:
  UnuMinmax(GuiContext*);

  virtual ~UnuMinmax();

  virtual void execute();

  virtual void tcl_command(GuiArgs&, void*);

private:
  NrrdOPort* onrrd_;

  NrrdDataHandle    onrrd_handle_;  //! the cached output nrrd handle.
  vector<int>       in_generation_; //! all input generation nums.
                                    //! target type for output nrrd.
  GuiInt            nrrds_;
};


DECLARE_MAKER(UnuMinmax)
UnuMinmax::UnuMinmax(GuiContext* ctx)
  : Module("UnuMinmax", ctx, Source, "Unu", "Teem"),
    onrrd_(0), onrrd_handle_(0), in_generation_(0),
    nrrds_(ctx->subVar("nrrds"))
{
}

UnuMinmax::~UnuMinmax(){
}

void
 UnuMinmax::execute(){
  port_range_type range = get_iports("Nrrds");
  if (range.first == range.second) { return; }

  unsigned int i = 0;
  vector<NrrdDataHandle> nrrds;
  bool do_join = false;
  port_map_type::iterator pi = range.first;
  int num_nrrds = 0;
  while (pi != range.second)
  {
    NrrdIPort *inrrd = (NrrdIPort *)get_iport(pi->second);
    if (!inrrd) {
      error("Unable to initialize iport '" + to_string(pi->second) + "'.");
      return;
    }

    NrrdDataHandle nrrd;
    
    if (inrrd->get(nrrd) && nrrd.get_rep()) {
      // check to see if we need to do the join or can output the cached onrrd.
      if (in_generation_.size() <= i) {
	// this is a new input, never been joined.
	do_join = true;
	in_generation_.push_back(nrrd->generation);
      } else if (in_generation_[i] != nrrd->generation) {
	// different input than last execution
	do_join = true;
	in_generation_[i] = nrrd->generation;
      }

      nrrds.push_back(nrrd);
      num_nrrds++;
    }
    ++pi; ++i;
  }

  if (num_nrrds != nrrds_.get()) {
    do_join = true;
  }

  nrrds_.set(num_nrrds);

  vector<Nrrd*> arr(nrrds.size());

  if (do_join) {
    vector<double> mins, maxs;

    int i = 0;
    vector<NrrdDataHandle>::iterator iter = nrrds.begin();
    while(iter != nrrds.end()) {
      NrrdDataHandle nh = *iter;
      ++iter;

      NrrdData* cur_nrrd = nh.get_rep();
      NrrdRange *range = nrrdRangeNewSet(cur_nrrd->nrrd, nrrdBlind8BitRangeFalse);
      mins.push_back(range->min);
      maxs.push_back(range->max);
      ++i;
    }
    

    // build list string
    string min_list = "[list ";
    string max_list = "[list ";

    for (int i=0; i<mins.size(); i++) {
      ostringstream min_str, max_str;
      min_str << mins[i] << " ";
      max_str << maxs[i] << " ";
      min_list += min_str.str();
      max_list += max_str.str();
    }
    min_list += "]";
    max_list += "]";
    
    gui->execute(id + " init_axes " + min_list + " " + max_list);
  }
}

void
 UnuMinmax::tcl_command(GuiArgs& args, void* userdata)
{
  Module::tcl_command(args, userdata);
}

} // End namespace Teem


