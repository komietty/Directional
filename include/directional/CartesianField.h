// This file is part of Directional, a library for directional field processing.
// Copyright (C) 2022 Amir Vaxman <avaxman@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#ifndef DIRECTIONAL_CARTESIAN_FIELD_H
#define DIRECTIONAL_CARTESIAN_FIELD_H

#include <iostream>
#include <Eigen/Geometry>
#include <Eigen/Sparse>
#include <directional/TangentBundle.h>

/***
 The class implements general cartesian fields in intrinsic dimension 2, which are attached to a tangent bundle. These fields can be of any degree N, where the unifying principle is that
 they are represented by Cartesian coordinates (intrinsically and possibly extrinsically). The class supports either direct raw fields (just a list of vectors in each
 tangent space in order), or power and polyvector fields, representing fields as root of polynomials irrespective of order.

 This class assumes extrinsic representation in 3D space.
 ***/

namespace directional{

    enum class fieldTypeEnum{RAW_FIELD, POWER_FIELD, POLYVECTOR_FIELD};

    class CartesianField{
    public:

        const TangentBundle* tb;            //Referencing the tangent bundle on which the field is defined

        int N;                              //Degree of field (how many vectors are in each point);
        fieldTypeEnum fieldType;                      //The representation of the field (for instance, either a raw field or a power/polyvector field)

        Eigen::MatrixXd intField;           //Intrinsic representation (depending on the local basis of the face). Size #T x 2N
        Eigen::MatrixXd extField;           //Ambient coordinates. Size Size #T x 3N

        Eigen::VectorXi matching;           //Matching(i)=j when vector k in adjSpaces(i,0) matches to vector (k+j)%N in adjSpaces(i,1)
        Eigen::VectorXd effort;             //Effort of the entire matching (sum of deviations from parallel transport)
        Eigen::VectorXi singLocalCycles;    //Singular (dual elements). Only the local cycles! not the generators or boundary cycles
        Eigen::VectorXi singIndices;        //Corresponding indices (this is the numerator where the true fractional index is singIndices/N);

        CartesianField(){}
        CartesianField(const TangentBundle& _tb):tb(&_tb){}
        ~CartesianField(){}

        //Initializing the field with the proper tangent spaces
        void IGL_INLINE init(const TangentBundle& _tb, const fieldTypeEnum _fieldType, const int _N){
            tb = &_tb;
            fieldType = _fieldType;
            N=_N;
            intField.resize(tb->sources.rows(),2*N);
            extField.resize(tb->sources.rows(),3*N);
        };

        void IGL_INLINE set_intrinsic_field(const Eigen::MatrixXd& _intField){
            assert (!(fieldType==fieldTypeEnum::POWER_FIELD) || (_intField.cols()==2));
            assert ((_intField.cols()==2*N) || !(fieldType==fieldTypeEnum::POLYVECTOR_FIELD || fieldType==fieldTypeEnum::RAW_FIELD));
            intField = _intField;

            extField = tb->project_to_extrinsic(Eigen::VectorXi(), intField);
        }

        //The same, just with complex coordinates
        void virtual IGL_INLINE set_intrinsic_field(const Eigen::MatrixXcd& _intField){
            intField.resize(_intField.rows(),_intField.cols()*2);
            for (int i=0;i<N;i++){
                intField.col(2*i)=_intField.col(i).real();
                intField.col(2*i+1)=_intField.col(i).imag();
            }
            set_intrinsic_field(intField);
        }

        //Setting the field by the extrinsic ambient field, which will get projected to the intrinsic tangent spaces.
        void IGL_INLINE set_extrinsic_field(const Eigen::MatrixXd& _extField){
            assert(_extField.cols()==3*N);
            extField=_extField;
            intField = tb->project_to_intrinsic(Eigen::VectorXi::LinSpaced(extField.rows(), 0,extField.rows()-1), extField);
        }


        //Directly setting the singularities of the the field (only at the local dual elements; not at generator or boundary cycles).
        void IGL_INLINE set_singularities(const Eigen::VectorXi& _singLocalCycles,
                                          const Eigen::VectorXi& _singIndices){

            //TODO: remove boundary elements
            singLocalCycles = _singLocalCycles;
            singIndices = _singIndices;
        }
    };

}



#endif 
