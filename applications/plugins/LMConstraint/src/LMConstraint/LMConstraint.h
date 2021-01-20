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
#ifndef SOFA_CORE_BEHAVIOR_LMCONSTRAINT_H
#define SOFA_CORE_BEHAVIOR_LMCONSTRAINT_H

#include <LMConstraint/config.h>
#include <LMConstraint/BaseLMConstraint.h>
#include <sofa/core/behavior/MechanicalState.h>

namespace sofa
{

namespace core
{

namespace behavior
{

/**
 *  \brief Component computing constraints within a simulated body.
 *
 *  This class define the abstract API common to constraints using a given type
 *  of DOFs.
 *  A LMConstraint computes complex constraints applied to two bodies
 *
 */
template<class DataTypes1,class DataTypes2>
class LMConstraint : public BaseLMConstraint
{
public:
    SOFA_ABSTRACT_CLASS(SOFA_TEMPLATE2(LMConstraint,DataTypes1,DataTypes2), BaseLMConstraint);

    typedef typename DataTypes1::Real Real1;
    typedef typename DataTypes1::VecCoord VecCoord1;
    typedef typename DataTypes1::VecDeriv VecDeriv1;
    typedef typename DataTypes1::Coord Coord1;
    typedef typename DataTypes1::Deriv Deriv1;
    typedef typename DataTypes1::MatrixDeriv MatrixDeriv1;
    typedef typename DataTypes1::MatrixDeriv::RowConstIterator MatrixDeriv1RowConstIterator;
    typedef typename DataTypes1::MatrixDeriv::ColConstIterator MatrixDeriv1ColConstIterator;
    typedef typename DataTypes1::MatrixDeriv::RowIterator MatrixDeriv1RowIterator;
    typedef typename DataTypes1::MatrixDeriv::ColIterator MatrixDeriv1ColIterator;
    typedef typename DataTypes1::MatrixDeriv::RowType MatrixDerivRowType1;

    typedef typename DataTypes2::Real Real2;
    typedef typename DataTypes2::VecCoord VecCoord2;
    typedef typename DataTypes2::VecDeriv VecDeriv2;
    typedef typename DataTypes2::Coord Coord2;
    typedef typename DataTypes2::Deriv Deriv2;
    typedef typename DataTypes2::MatrixDeriv MatrixDeriv2;
    typedef typename DataTypes2::MatrixDeriv::RowConstIterator MatrixDeriv2RowConstIterator;
    typedef typename DataTypes2::MatrixDeriv::ColConstIterator MatrixDeriv2ColConstIterator;
    typedef typename DataTypes2::MatrixDeriv::RowIterator MatrixDeriv2RowIterator;
    typedef typename DataTypes2::MatrixDeriv::ColIterator MatrixDeriv2ColIterator;
    typedef typename DataTypes2::MatrixDeriv::RowType MatrixDerivRowType2;

protected:
    LMConstraint();
    LMConstraint( MechanicalState<DataTypes1> *dof1, MechanicalState<DataTypes2> *dof2);

    ~LMConstraint() override;
public:
    void init() override;

    /// get Mechanical State 1 where the constraint will be expressed (can be a Mapped mechanical state)
    BaseMechanicalState* getConstrainedMechModel1() const override {return constrainedObject1;}
    /// get Mechanical State 2 where the constraint will be expressed (can be a Mapped mechanical state)
    BaseMechanicalState* getConstrainedMechModel2() const override {return constrainedObject2;}

    /// get Mechanical State 1 where the constraint will be solved
    BaseMechanicalState* getSimulatedMechModel1() const override {return simulatedObject1;}
    /// get Mechanical State 2 where the constraint will be solved
    BaseMechanicalState* getSimulatedMechModel2() const override {return simulatedObject2;}

    /// Pre-construction check method called by ObjectFactory.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (arg->getAttribute("object1") || arg->getAttribute("object2"))
        {
            if (dynamic_cast<MechanicalState<DataTypes1>*>(arg->findObject(arg->getAttribute("object1",".."))) == nullptr) {
                arg->logError("Data attribute 'object1' must point the a valid mechanical state of datatype '" + std::string(DataTypes1::Name()) + "'.");
                return false;
            }

            if (dynamic_cast<MechanicalState<DataTypes2>*>(arg->findObject(arg->getAttribute("object2",".."))) == nullptr) {
                arg->logError("Data attribute 'object2' must point the a valid mechanical state of datatype '" + std::string(DataTypes2::Name()) + "'.");
                return false;
            }
        }
        else
        {
            if (dynamic_cast<MechanicalState<DataTypes1>*>(context->getMechanicalState()) == nullptr) {
                arg->logError("No mechanical state with the datatype '" + std::string(DataTypes1::Name()) + "' found in the context node.");
                return false;
            }
        }
        return sofa::core::objectmodel::BaseObject::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static typename T::SPtr create(T* p0, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        typename T::SPtr obj = sofa::core::objectmodel::BaseObject::create(p0, context, arg);

        if (arg && (arg->getAttribute("object1") || arg->getAttribute("object2")))
        {
            obj->constrainedObject1 = dynamic_cast<MechanicalState<DataTypes1>*>(arg->findObject(arg->getAttribute("object1","..")));
            obj->constrainedObject2 = dynamic_cast<MechanicalState<DataTypes2>*>(arg->findObject(arg->getAttribute("object2","..")));
        }
        else if (context)
        {
            obj->constrainedObject1 =
                obj->constrainedObject2 =
                        dynamic_cast<MechanicalState<DataTypes1>*>(context->getMechanicalState());
        }

        return obj;
    }

protected:
    MechanicalState<DataTypes1> *constrainedObject1;
    MechanicalState<DataTypes2> *constrainedObject2;

    BaseMechanicalState         *simulatedObject1;
    BaseMechanicalState         *simulatedObject2;


    /// Mask are handled manually in LMConstraints (MUST be setted in buildConstaintMatrix)
    void updateForceMask() override {}
};

#if  !defined(SOFA_CORE_BEHAVIOR_LMCONSTRAINT_CPP)
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec3Types,defaulttype::Vec3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec3Types,defaulttype::Vec2Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec3Types,defaulttype::Vec1Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec3Types,defaulttype::Rigid3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec3Types,defaulttype::Rigid2Types>;

extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec2Types,defaulttype::Vec3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec2Types,defaulttype::Vec2Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec2Types,defaulttype::Vec1Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec2Types,defaulttype::Rigid3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec2Types,defaulttype::Rigid2Types>;

extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec1Types,defaulttype::Vec3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec1Types,defaulttype::Vec2Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec1Types,defaulttype::Vec1Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec1Types,defaulttype::Rigid3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Vec1Types,defaulttype::Rigid2Types>;

extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid3Types,defaulttype::Vec3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid3Types,defaulttype::Vec2Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid3Types,defaulttype::Vec1Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid3Types,defaulttype::Rigid3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid3Types,defaulttype::Rigid2Types>;

extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid2Types,defaulttype::Vec3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid2Types,defaulttype::Vec2Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid2Types,defaulttype::Vec1Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid2Types,defaulttype::Rigid3Types>;
extern template class LMCONSTRAINT_API LMConstraint<defaulttype::Rigid2Types,defaulttype::Rigid2Types>;


#endif

} // namespace behavior

} // namespace core

} // namespace sofa

#endif
