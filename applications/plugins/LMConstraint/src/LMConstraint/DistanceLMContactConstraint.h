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
#include <LMConstraint/LMConstraint.h>
#include <LMConstraint/ContactDescription.h>

#include <sofa/core/VecId.h>
#include <sofa/core/ConstraintParams.h>
#include <sofa/core/behavior/BaseMass.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/simulation/Node.h>


namespace sofa::component::constraintset
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template <class DataTypes>
class DistanceLMContactConstraintInternalData
{
};




/** Keep two particules at an initial distance
*/
template <class DataTypes>
class DistanceLMContactConstraint :  public core::behavior::LMConstraint<DataTypes,DataTypes>, public ContactDescriptionHandler
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(DistanceLMContactConstraint,DataTypes),SOFA_TEMPLATE2(sofa::core::behavior::LMConstraint, DataTypes, DataTypes));

    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::MatrixDeriv MatrixDeriv;
    typedef typename DataTypes::MatrixDeriv::RowIterator MatrixDerivRowIterator;

    typedef typename core::behavior::MechanicalState<DataTypes> MechanicalState;
    typedef typename sofa::core::topology::BaseMeshTopology::SeqEdges SeqEdges;
    typedef typename sofa::core::topology::BaseMeshTopology::Edge Edge;
    typedef core::ConstraintParams::ConstOrder ConstOrder;
    typedef core::behavior::ConstraintGroup ConstraintGroup;


protected:
    DistanceLMContactConstraint();
    DistanceLMContactConstraint( MechanicalState *dof);
    DistanceLMContactConstraint( MechanicalState *dof1, MechanicalState * dof2);


    ~DistanceLMContactConstraint() override {}
public:
    // -- LMConstraint interface
    void buildConstraintMatrix(const core::ConstraintParams* cParams, core::MultiMatrixDerivId cId, unsigned int &cIndex) override;

    void writeConstraintEquations(unsigned int& lineNumber, core::MultiVecId id, ConstOrder order) override;
    void LagrangeMultiplierEvaluation(const SReal* Wptr, const SReal* cptr, SReal* LambdaInitptr,
            core::behavior::ConstraintGroup * group) override;

    bool isCorrectionComputedWithSimulatedDOF(ConstOrder order) const override;
    void clear();
    /// register a new contact
    void addContact(unsigned m1, unsigned m2);
    void draw(const core::visual::VisualParams* vparams) override;

protected :

    /// Each scalar constraint (up to three per contact) has an associated index
    helper::vector<  unsigned int > scalarConstraintsIndices;

public:
    /// Contacts are represented by pairs of point indices
    Data< SeqEdges > pointPairs;

    /// Friction coefficients (same for all contacts)
    Data< SReal > contactFriction;

protected:
    ///Compute the length of an edge given the vector of coordinates corresponding
    double lengthEdge(const Edge &e, const VecCoord &x1,const VecCoord &x2) const;
    /// Contact normal
    Deriv computeNormal(const Edge &e, const VecCoord &x1, const VecCoord &x2) const;
    /// Contact tangent vectors
    void computeTangentVectors( Deriv& T1, Deriv& T2, const Deriv& N );

    struct Contact
    {
        //Constrained Axis
        Deriv n,t1,t2;
        Contact() {}
        Contact( Deriv norm, Deriv tgt1, Deriv tgt2 ):n(norm),t1(tgt1),t2(tgt2),contactForce(Deriv()) {}
        Deriv contactForce;
    };

    std::map< Edge, Contact > edgeToContact;
    std::map< ConstraintGroup*, Contact* > constraintGroupToContact;
    core::collision::Intersection* intersection;
protected:
    DistanceLMContactConstraintInternalData<DataTypes> data;
    friend class DistanceLMContactConstraintInternalData<DataTypes>;


    void initColorContactState()
    {
        colorsContactState.clear();
        //Vanishing
        colorsContactState.push_back(sofa::helper::types::RGBAColor(0.0f,1.0f,0.0f,1.0));
        //Sticking
        colorsContactState.push_back(sofa::helper::types::RGBAColor(1.0f,0.0f,0.0f,1.0));
        //Sliding
        colorsContactState.push_back(sofa::helper::types::RGBAColor(1.0f,1.0f,0.0f,1.0));
        //Sliding Direction
        colorsContactState.push_back(sofa::helper::types::RGBAColor(1.0f,0.0f,1.0f,1.0));
    }

    helper::vector< sofa::helper::types::RGBAColor > colorsContactState;
};


#if  !defined(SOFA_COMPONENT_CONSTRAINTSET_DistanceLMContactConstraint_CPP)
extern template class LMCONSTRAINT_API DistanceLMContactConstraint<defaulttype::Vec3Types>;

#endif

} //namespace sofa::component::constraintset
