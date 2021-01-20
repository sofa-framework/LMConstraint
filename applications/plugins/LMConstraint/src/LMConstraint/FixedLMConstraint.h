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
#include <LMConstraint/config.h>

#include <sofa/core/topology/BaseMeshTopology.h>
#include <LMConstraint/LMConstraint.h>
#include <SofaBaseTopology/TopologySubsetData.h>
#include <sofa/simulation/Node.h>


namespace sofa::component::constraintset
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template <class DataTypes>
class FixedLMConstraintInternalData
{
};



/** Keep two particules at an initial distance
*/
template <class DataTypes>
class FixedLMConstraint :  public core::behavior::LMConstraint<DataTypes,DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(FixedLMConstraint,DataTypes),SOFA_TEMPLATE2(sofa::core::behavior::LMConstraint, DataTypes, DataTypes));

    using Index = sofa::Index;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::MatrixDeriv MatrixDeriv;
    typedef typename DataTypes::MatrixDeriv::RowIterator MatrixDerivRowIterator;
    typedef typename core::behavior::MechanicalState<DataTypes> MechanicalState;


    typedef helper::vector<Index> SetIndexArray;
    typedef sofa::component::topology::PointSubsetData< SetIndexArray > SetIndex;

    typedef core::ConstraintParams::ConstOrder ConstOrder;

protected:
    FixedLMConstraintInternalData<DataTypes> data;
    friend class FixedLMConstraintInternalData<DataTypes>;

    FixedLMConstraint( MechanicalState *dof = nullptr);

    ~FixedLMConstraint()
    {
        if (m_pointHandler)
            delete m_pointHandler;
    }

public:
    void clearConstraints();
    void addConstraint(Index index);
    void removeConstraint(Index index);

    void init() override;
    void draw(const core::visual::VisualParams* vparams) override;
    void initFixedPosition();
    void reset() override {initFixedPosition();}

    // -- LMConstraint interface
    void buildConstraintMatrix(const core::ConstraintParams* cParams, core::MultiMatrixDerivId cId, unsigned int &cIndex) override;
    void writeConstraintEquations(unsigned int& lineNumber, core::MultiVecId id, ConstOrder order) override;

    bool isCorrectionComputedWithSimulatedDOF(ConstOrder /*order*/) const override
    {
        simulation::Node* node=(simulation::Node*) this->constrainedObject1->getContext();
        if (node->mechanicalMapping.empty()) return true;
        else return false;
    }

    SetIndex f_indices; ///< List of the index of particles to be fixed
    Data<double> _drawSize; ///< 0 -> point based rendering, >0 -> radius of spheres

    /// Link to be set to the topology container in the component graph.
    SingleLink<FixedLMConstraint<DataTypes>, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

    class FCPointHandler : public sofa::component::topology::TopologySubsetDataHandler<core::topology::BaseMeshTopology::Point, SetIndexArray >
    {
    public:
        typedef typename FixedLMConstraint<DataTypes>::SetIndexArray SetIndexArray;
        FCPointHandler(FixedLMConstraint<DataTypes>* _fc, sofa::component::topology::PointSubsetData<SetIndexArray >* _data)
            : sofa::component::topology::TopologySubsetDataHandler<core::topology::BaseMeshTopology::Point, SetIndexArray >(_data), fc(_fc) {}



        void applyDestroyFunction(Index /*index*/, value_type& /*T*/);


        bool applyTestCreateFunction(Index /*index*/,
                const sofa::helper::vector< Index > & /*ancestors*/,
                const sofa::helper::vector< double > & /*coefs*/);
    protected:
        FixedLMConstraint<DataTypes> *fc;
    };

protected :

    Deriv X,Y,Z;
    SetIndexArray idxX, idxY, idxZ;
    std::map< Index, Coord> restPosition;


    FCPointHandler* m_pointHandler;

};


#if  !defined(SOFA_COMPONENT_CONSTRAINTSET_FIXEDLMCONSTRAINT_CPP)
extern template class LMCONSTRAINT_API FixedLMConstraint<defaulttype::Vec3Types>;
extern template class LMCONSTRAINT_API FixedLMConstraint<defaulttype::Rigid3Types>;

#endif

} //namespace sofa::component::constraintset
