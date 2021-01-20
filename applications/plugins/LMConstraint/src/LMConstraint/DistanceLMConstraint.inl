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
#pragma once
#include <sofa/core/topology/BaseMeshTopology.h>
#include <LMConstraint/BaseLMConstraint.h>
#include <LMConstraint/DistanceLMConstraint.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/Node.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/vector.h>
#include <sofa/helper/types/RGBAColor.h>
#include <iostream>


namespace sofa::component::constraintset
{

template <class DataTypes>
DistanceLMConstraint<DataTypes>::DistanceLMConstraint( MechanicalState *dof1, MechanicalState * dof2)
    : core::behavior::LMConstraint<DataTypes,DataTypes>(dof1,dof2)
    , vecConstraint(sofa::core::objectmodel::Base::initData(&vecConstraint, "vecConstraint", "List of the edges to constrain"))
    , l_topology(initLink("topology", "link to the topology container"))
{
}

template <class DataTypes>
DistanceLMConstraint<DataTypes>::DistanceLMConstraint( MechanicalState *dof)
    : DistanceLMConstraint(dof,dof)
{
}

template <class DataTypes>
DistanceLMConstraint<DataTypes>::DistanceLMConstraint()
    : DistanceLMConstraint(nullptr, nullptr)
{
}

template <class DataTypes>
void DistanceLMConstraint<DataTypes>::init()
{
    sofa::core::behavior::LMConstraint<DataTypes,DataTypes>::init();
    
    // TODO epenod 2019-12-05: Adapt code to not look for topology if constraint is manually set. Need to dig more in the code to understand how topology is used.
    if (vecConstraint.getValue().size() != 0)
    {
        // nothing to do in this case
        return;
    }

    if (l_topology.empty())
    {
        msg_info() << "link to Topology container should be set to ensure right behavior. First Topology found in current context will be used.";
        l_topology.set(this->getContext()->getMeshTopologyLink());
    }
    
    core::topology::BaseMeshTopology *_topology = l_topology.get();
    msg_info() << "Topology path used: '" << l_topology.getLinkedPath() << "'";

    if (!_topology)
    {
        msg_error() << "No topology component found at path: " << l_topology.getLinkedPath() << ", nor in current context: " << this->getContext()->name;
        sofa::core::objectmodel::BaseObject::d_componentState.setValue(sofa::core::objectmodel::ComponentState::Invalid);
        return;
    }

    if (vecConstraint.getValue().size() == 0 && (this->constrainedObject1==this->constrainedObject2) ) 
        vecConstraint.setValue(_topology->getEdges());
}

template <class DataTypes>
void DistanceLMConstraint<DataTypes>::reinit()
{
    updateRestLength();
}

template <class DataTypes>
double DistanceLMConstraint<DataTypes>::lengthEdge(const Edge &e, const VecCoord &x1, const VecCoord &x2) const
{
    return (x2[e[1]] -  x1[e[0]]).norm();
}

template <class DataTypes>
void DistanceLMConstraint<DataTypes>::updateRestLength()
{
    const VecCoord &x0_1= this->constrainedObject1->read(core::ConstVecCoordId::position())->getValue();
    const VecCoord &x0_2= this->constrainedObject2->read(core::ConstVecCoordId::position())->getValue();
    const SeqEdges &edges =  vecConstraint.getValue();
    this->l0.resize(edges.size());
    for (unsigned int i=0; i<edges.size(); ++i)
    {
        this->l0[i] = lengthEdge(edges[i],x0_1,x0_2);
    }
}

template<class DataTypes>
typename DataTypes::Deriv DistanceLMConstraint<DataTypes>::getDirection(const Edge &e, const VecCoord &x1, const VecCoord &x2) const
{
    Deriv V12 = (x2[e[1]] - x1[e[0]]);
    V12.normalize();
    return V12;
}


template<class DataTypes>
void DistanceLMConstraint<DataTypes>::buildConstraintMatrix(const core::ConstraintParams* cParams, core::MultiMatrixDerivId cId, unsigned int &cIndex)
{
    Data<MatrixDeriv>* dC1 = cId[this->constrainedObject1].write();
    helper::WriteAccessor<sofa::core::objectmodel::Data<MatrixDeriv> > c1 = *dC1;

    Data<MatrixDeriv>* dC2 = cId[this->constrainedObject2].write();
    helper::WriteAccessor<sofa::core::objectmodel::Data<MatrixDeriv> > c2 = *dC2;

    helper::ReadAccessor<sofa::core::objectmodel::Data<VecCoord> > x1 = *cParams->readX(this->constrainedObject1);
    helper::ReadAccessor<sofa::core::objectmodel::Data<VecCoord> > x2 = *cParams->readX(this->constrainedObject2);

    const SeqEdges &edges = vecConstraint.getValue();

    if (this->l0.size() != edges.size()) updateRestLength();

    registeredConstraints.clear();

    for (unsigned int i = 0; i < edges.size(); ++i)
    {
        unsigned int idx1 = edges[i][0];
        unsigned int idx2 = edges[i][1];

        const Deriv V12 = getDirection(edges[i], x1.ref() , x2.ref() );

        MatrixDerivRowIterator c1_it = c1->writeLine(cIndex);
        c1_it.addCol(idx1, V12);
        MatrixDerivRowIterator c2_it = c2->writeLine(cIndex);
        c2_it.addCol(idx2, -V12);

        registeredConstraints.push_back(cIndex);
        cIndex++;

        this->constrainedObject1->forceMask.insertEntry(idx1);
        this->constrainedObject2->forceMask.insertEntry(idx2);
    }
}

template<class DataTypes>
void DistanceLMConstraint<DataTypes>::writeConstraintEquations(unsigned int& lineNumber, core::MultiVecId id, ConstOrder Order)
{
    using namespace core;
    using namespace core::objectmodel;
    const SeqEdges &edges =  vecConstraint.getValue();

    if (registeredConstraints.empty()) return;
    for (unsigned int i=0; i<edges.size(); ++i)
    {
        core::behavior::ConstraintGroup *constraint = this->addGroupConstraint(Order);
        SReal correction=0;
        switch(Order)
        {
        case core::ConstraintParams::ACC :
        case core::ConstraintParams::VEL :
        {
            ConstVecId v1 = id.getId(this->constrainedObject1);
            ConstVecId v2 = id.getId(this->constrainedObject2);
            correction = this->constrainedObject1->getConstraintJacobianTimesVecDeriv(registeredConstraints[i],v1);
            correction += this->constrainedObject2->getConstraintJacobianTimesVecDeriv(registeredConstraints[i],v2);
            break;
        }
        case core::ConstraintParams::POS :
        case core::ConstraintParams::POS_AND_VEL :
        {
            const VecCoord &x1 = this->constrainedObject1->read(core::ConstVecCoordId(id.getId(this->constrainedObject1) ))->getValue();
            const VecCoord &x2 = this->constrainedObject2->read(core::ConstVecCoordId(id.getId(this->constrainedObject2) ))->getValue();
            SReal length     = lengthEdge(edges[i],x1,x2);
            SReal restLength = this->l0[i];
            correction= restLength-length;
            break;
        }
        };
        constraint->addConstraint( lineNumber, registeredConstraints[i], -correction);
    }
}


template <>
void DistanceLMConstraint<defaulttype::Rigid3Types>::draw(const core::visual::VisualParams* vparams);


template <class DataTypes>
void DistanceLMConstraint<DataTypes>::draw(const core::visual::VisualParams* vparams)
{
    if (this->l0.size() != vecConstraint.getValue().size()) updateRestLength();

    vparams->drawTool()->saveLastState();
    if (vparams->displayFlags().getShowBehaviorModels())
    {
        const VecCoord &x1= this->constrainedObject1->read(core::ConstVecCoordId::position())->getValue();
        const VecCoord &x2= this->constrainedObject2->read(core::ConstVecCoordId::position())->getValue();

        std::vector< sofa::defaulttype::Vector3 > points;
        const SeqEdges &edges =  vecConstraint.getValue();
        for (unsigned int i=0; i<edges.size(); ++i)
        {
            points.push_back(x1[edges[i][0]]);
            points.push_back(x2[edges[i][1]]);
        }
        vparams->drawTool()->drawLines(points, 1, sofa::helper::types::RGBAColor::green());
    }
    vparams->drawTool()->restoreLastState();

}

} //namespace sofa::component::constraintset
