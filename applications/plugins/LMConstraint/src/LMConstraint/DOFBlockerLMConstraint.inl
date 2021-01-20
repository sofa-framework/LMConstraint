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
#include <LMConstraint/DOFBlockerLMConstraint.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/simulation/Simulation.h>
#include <SofaBaseTopology/TopologySubsetData.inl>
#include <sofa/helper/types/RGBAColor.h>


namespace sofa::component::constraintset
{

// Define TestNewPointFunction
template< class DataTypes>
bool DOFBlockerLMConstraint<DataTypes>::FCTPointHandler::applyTestCreateFunction(Index /*nbPoints*/, const sofa::helper::vector< Index > &, const sofa::helper::vector< double >& )
{
    if (fc)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Define RemovalFunction
template< class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::FCTPointHandler::applyDestroyFunction(Index pointIndex, value_type &)
{
    if (fc)
    {
        fc->removeConstraint((Index) pointIndex);
    }
    return;
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::clearConstraints()
{
    SetIndexArray& _indices = *f_indices.beginEdit();
    _indices.clear();
    f_indices.endEdit();
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::addConstraint(Index index)
{
    f_indices.beginEdit()->push_back(index);
    f_indices.endEdit();
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::removeConstraint(Index index)
{
    removeValue(*f_indices.beginEdit(),index);
    f_indices.endEdit();
}


template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::init()
{
    core::behavior::LMConstraint<DataTypes,DataTypes>::init();

    if (l_topology.empty())
    {
        msg_info() << "link to Topology container should be set to ensure right behavior. First Topology found in current context will be used.";
        l_topology.set(this->getContext()->getMeshTopologyLink());
    }

    sofa::core::topology::BaseMeshTopology* _topology = l_topology.get();

    if (_topology)
    {
        msg_info() << "Topology path used: '" << l_topology.getLinkedPath() << "'";
        
        // Initialize functions and parameters
        m_pointHandler = new FCTPointHandler(this, &f_indices);
        f_indices.createTopologicalEngine(_topology, m_pointHandler);
        f_indices.registerTopologicalData();        
    }
    else
    {
        msg_info() << "No topology component found at path: " << l_topology.getLinkedPath() << ", nor in current context: " << this->getContext()->name;
    }
}


template<class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::resetConstraint()
{
    core::behavior::LMConstraint<DataTypes,DataTypes>::resetConstraint();
    idxEquations.clear();
}

template<class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::buildConstraintMatrix(const core::ConstraintParams* /*cParams*/, core::MultiMatrixDerivId cId, unsigned int &cIndex)
{
    if(!idxEquations.empty() ) return;

    using namespace core::objectmodel;
    Data<MatrixDeriv>* dC = cId[this->constrainedObject1].write();
    helper::WriteAccessor<Data<MatrixDeriv> > c = *dC;

    const SetIndexArray &indices = f_indices.getValue();
    const helper::vector<Deriv> &axis=BlockedAxis.getValue();
    idxEquations.resize(indices.size());
    unsigned int numParticle=0;

    for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it, ++numParticle)
    {
        const Index index=*it;
        for (unsigned int i=0; i<axis.size(); ++i)
        {
            c->writeLine(cIndex).addCol(index,axis[i]);
            idxEquations[numParticle].push_back(cIndex++);
        }
        this->constrainedObject1->forceMask.insertEntry(index);
    }


}

template<class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::writeConstraintEquations(unsigned int& lineNumber, core::MultiVecId id, ConstOrder Order)
{
    using namespace core;
    using namespace core::objectmodel;
    //We don't constrain the Position, only the velocities and accelerations
    if (idxEquations.empty() ||
        Order==core::ConstraintParams::POS) return;

    const SetIndexArray & indices = f_indices.getValue();
    const helper::vector<SReal> &factor=factorAxis.getValue();

    for (unsigned int numParticle=0; numParticle<indices.size(); ++numParticle)
    {
        for (unsigned int i=0; i<idxEquations[numParticle].size(); ++i)
        {
            core::behavior::ConstraintGroup *constraint = this->addGroupConstraint(Order);
            SReal correction=0;
            switch(Order)
            {
            case core::ConstraintParams::ACC :
            case core::ConstraintParams::VEL :
            {
                ConstVecId v1 = id.getId(this->constrainedObject1);
                correction = this->constrainedObject1->getConstraintJacobianTimesVecDeriv(idxEquations[numParticle][i],v1);
                break;
            }
            default: break;
            };
            if (!factor.empty())
            {
                if (i < factor.size()) correction*=factor[i];
                else                   correction*=factor.back();
            }
            constraint->addConstraint( lineNumber, idxEquations[numParticle][i], -correction);
        }
    }
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::draw(const core::visual::VisualParams* vparams)
{
    if (!vparams->displayFlags().getShowForceFields()) return;
    const VecCoord& x =this->constrainedObject1->read(core::ConstVecCoordId::position())->getValue();

    vparams->drawTool()->saveLastState();

    const SetIndexArray & indices = f_indices.getValue();
    sofa::helper::types::RGBAColor color = sofa::helper::types::RGBAColor::yellow();

    for (SetIndexArray::const_iterator it = indices.begin();
            it != indices.end();
            ++it)
    {
        Index index=(*it);
        Coord pos=x[index];
        defaulttype::Vector3 position;
        DataTypes::get(position[0], position[1], position[2], pos);
        const helper::vector<Deriv>& axis=BlockedAxis.getValue();
        for (unsigned int i=0; i<axis.size(); ++i)
        {
            defaulttype::Vector3 direction;
            DataTypes::get(direction[0], direction[1], direction[2],axis[i]);
            vparams->drawTool()->drawArrow(position, position+direction*showSizeAxis.getValue(),
                                           showSizeAxis.getValue()*0.03,color);
        }
    }
    vparams->drawTool()->restoreLastState();
}

} //namespace sofa::component::constraintset
