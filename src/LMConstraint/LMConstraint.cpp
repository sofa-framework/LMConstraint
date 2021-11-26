/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#define SOFA_CORE_BEHAVIOR_LMCONSTRAINT_CPP
#include <LMConstraint/LMConstraint.inl>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace core
{

namespace behavior
{

using namespace sofa::defaulttype;

template class LMCONSTRAINT_API LMConstraint<Vec3Types,Vec3Types>;
template class LMCONSTRAINT_API LMConstraint<Vec3Types,Vec2Types>;
template class LMCONSTRAINT_API LMConstraint<Vec3Types,Vec1Types>;
template class LMCONSTRAINT_API LMConstraint<Vec3Types,Rigid3Types>;
template class LMCONSTRAINT_API LMConstraint<Vec3Types,Rigid2Types>;

template class LMCONSTRAINT_API LMConstraint<Vec2Types,Vec3Types>;
template class LMCONSTRAINT_API LMConstraint<Vec2Types,Vec2Types>;
template class LMCONSTRAINT_API LMConstraint<Vec2Types,Vec1Types>;
template class LMCONSTRAINT_API LMConstraint<Vec2Types,Rigid3Types>;
template class LMCONSTRAINT_API LMConstraint<Vec2Types,Rigid2Types>;

template class LMCONSTRAINT_API LMConstraint<Vec1Types,Vec3Types>;
template class LMCONSTRAINT_API LMConstraint<Vec1Types,Vec2Types>;
template class LMCONSTRAINT_API LMConstraint<Vec1Types,Vec1Types>;
template class LMCONSTRAINT_API LMConstraint<Vec1Types,Rigid3Types>;
template class LMCONSTRAINT_API LMConstraint<Vec1Types,Rigid2Types>;

template class LMCONSTRAINT_API LMConstraint<Rigid3Types,Vec3Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid3Types,Vec2Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid3Types,Vec1Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid3Types,Rigid3Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid3Types,Rigid2Types>;

template class LMCONSTRAINT_API LMConstraint<Rigid2Types,Vec3Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid2Types,Vec2Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid2Types,Vec1Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid2Types,Rigid3Types>;
template class LMCONSTRAINT_API LMConstraint<Rigid2Types,Rigid2Types>;


//Need the combinations

} // namespace behavior

} // namespace core

} // namespace sofa
