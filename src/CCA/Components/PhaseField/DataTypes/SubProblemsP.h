/*
 * The MIT License
 *
 * Copyright (c) 1997-2018 The University of Utah
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
<<<<<<< HEAD
 * @file CCA/Components/PhaseField/DataTypes/SubProblemsP.h
=======
<<<<<<< HEAD:src/CCA/Components/PhaseField/AMR/detail/amr_interface0.h
 * @file CCA/Components/PhaseField/AMR/detail/amr_interface0.h
=======
 * @file CCA/Components/PhaseField/DataTypes/SubProblemsP.h
>>>>>>> origin/master:src/CCA/Components/PhaseField/DataTypes/SubProblemsP.h
>>>>>>> origin/master
 * @author Jon Matteo Church [j.m.church@leeds.ac.uk]
 * @date 2018/12
 */

<<<<<<< HEAD
=======
<<<<<<< HEAD:src/CCA/Components/PhaseField/AMR/detail/amr_interface0.h
#ifndef Packages_Uintah_CCA_Components_PhaseField_AMR_detail_amr_interface0_h
#define Packages_Uintah_CCA_Components_PhaseField_AMR_detail_amr_interface0_h

#include <CCA/Components/PhaseField/Util/Definitions.h>
=======
>>>>>>> origin/master
#ifndef Packages_Uintah_CCA_Components_PhaseField_DataTypes_SubProblemsP_h
#define Packages_Uintah_CCA_Components_PhaseField_DataTypes_SubProblemsP_h

#include <Core/Util/Handle.h>
<<<<<<< HEAD
=======
>>>>>>> origin/master:src/CCA/Components/PhaseField/DataTypes/SubProblemsP.h
>>>>>>> origin/master

namespace Uintah
{
namespace PhaseField
{
<<<<<<< HEAD
=======
<<<<<<< HEAD:src/CCA/Components/PhaseField/AMR/detail/amr_interface0.h
namespace detail
{

/**
 * @brief Interface for amr
 *
 * groups together various methods to get info about amr patches and levels which
 * depend on the different types of variable representation allowing to choose
 * the relevant implementation at compile time
 *
 * @tparam VAR type of variable representation
 */
template < VarType VAR > class amr_interface0;

} // namespace detail
} // namespace PhaseField
} // namespace Uintah

#include <CCA/Components/PhaseField/AMR/detail/amr_interface0_CC.h>
#include <CCA/Components/PhaseField/AMR/detail/amr_interface0_NC.h>

#endif // Packages_Uintah_CCA_Components_PhaseField_AMR_detail_amr_interface0_h
=======
>>>>>>> origin/master

template <typename Problem> struct SubProblems;

/**
 * @brief Handle for SubProblems
 *
 * @tparam Problem type of PhaseField problem
 */
template <typename Problem> using SubProblemsP = Handle < SubProblems<Problem> >;

} // namespace PhaseField

/**
 * @brief Fix Endianess
 *
 * Ovverride for preventing compiler errors
 * @remark Should never be called
 *
 * @tparam Problem type of PhaseField problem
 * @param problem unused argument
 */
template <typename Problem>
inline void
swapbytes (
    PhaseField::SubProblemsP<Problem> & _DOXYARG ( problem )
)
{
    SCI_THROW ( InternalError ( "Swap bytes for ProblemsP is not implemented", __FILE__, __LINE__ ) );
};

} // namespace Uintah

#endif // Packages_Uintah_CCA_Components_PhaseField_DataTypes_SubProblemsP_h
<<<<<<< HEAD
=======
>>>>>>> origin/master:src/CCA/Components/PhaseField/DataTypes/SubProblemsP.h
>>>>>>> origin/master
