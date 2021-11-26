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
#include <LMConstraint/BarycentricDistanceLMConstraintContact.inl>
#include <SofaMeshCollision/BarycentricContactMapper.h>
#include <SofaMiscCollision/TetrahedronModel.h>

using namespace sofa::defaulttype;
using namespace sofa::core::collision;

namespace sofa
{

namespace component
{

namespace collision
{

Creator<Contact::Factory, BarycentricDistanceLMConstraintContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>> > TetrahedronSphereDistanceLMConstraintContactClass("distanceLMConstraint",true);
Creator<Contact::Factory, BarycentricDistanceLMConstraintContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>> > TetrahedronPointDistanceLMConstraintContactClass("distanceLMConstraint",true);
Creator<Contact::Factory, BarycentricDistanceLMConstraintContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>> > TetrahedronLineDistanceLMConstraintContactClass("distanceLMConstraint",true);
Creator<Contact::Factory, BarycentricDistanceLMConstraintContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>> > TetrahedronTriangleDistanceLMConstraintContactClass("distanceLMConstraint",true);
Creator<Contact::Factory, BarycentricDistanceLMConstraintContact<TetrahedronCollisionModel, TetrahedronCollisionModel> > TetrahedronTetrahedronDistanceLMConstraintContactClass("distanceLMConstraint",true);

} // namespace collision

} // namespace component

} // namespace sofa

