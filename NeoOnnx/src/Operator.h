/* Copyright © 2017-2023 ABBYY

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
--------------------------------------------------------------------------------------------------------------*/

#pragma once

#include "onnx.pb.h"

#include "Tensor.h"
#include "AttributeGetters.h"

namespace NeoOnnx {

// Opset versioning support
const int MaxOpsetVersion = 16;

// onnx operator
class COperator {
public:
	virtual ~COperator() = default;

	COperator( const COperator& other ) = delete;
	COperator& operator= ( const COperator& other ) = delete;

	// Static methods

	// Fabric method
	// Creates COperator's derivative for the given onnx proto
	static COperator* CreateOperator( const onnx::NodeProto& onnxNode, int opsetVersion );

	// Returns true if operatorType is supported by NeoOnnx
	static bool IsSupportedOperator( const CString& operatorType );

	// Properties

	// Operator name
	const CString& Name() const { return name; }
	// Operator type ('Conv', 'Pool' etc.)
	const CString& Type() const { return type; }
	// Number of inputs
	int InputCount() const { return inputNames.Size(); }
	// Name of the index'th input
	const CString& InputName( int index ) const;
	// Number of outputs
	int OutputCount() const { return outputNames.Size(); }
	// Name of the index'th output
	const CString& OutputName( int outputIndex ) const;

	// Virtual methods

	// Processes input tensors and puts the result to the output array
	// If data can be calculated the output tensors will be of CDataTensor type
	// Otherwise output tensors will be of CUserTensor type and corresponding layers will be added to the dnn
	virtual void ProcessTensors( const CTensorArray& inputs, CDnn& dnn, CTensorArray& outputs ) const = 0;

protected:
	// Opset version
	const int OpsetVersion;

	COperator( const onnx::NodeProto& node, int opsetVersion );

	// Gets attribute value
	// Returns false if there is no attribute with the given name
	template<class T>
	bool GetAttribute( const CString& name, T& value ) const;

	// Throws an exception if operator has nullptr among its inputs
	void CheckNoNullInputs( const CTensorArray& inputs ) const;

	// Throws an exception if operator has CUserTensor among its inputs
	void CheckNoUserInputs( const CTensorArray& inputs ) const;

	// Throws an exception if operator has CShapeTensor among its inputs
	void CheckNoShapeInputs( const CTensorArray& inputs ) const;

	// Returns true if any of inputs is a CUserTensor
	bool HasUserInput( const CTensorArray& inputs ) const;

private:
	// Operator name
	const CString name;
	// Operator type ('Conv', 'Pool' etc.)
	const CString type;
	// Operator attributes
	CMap<CString, const onnx::AttributeProto&> attributes;
	// Input names
	CArray<CString> inputNames;
	// Output names
	CArray<CString> outputNames;
};

template<class T>
inline bool COperator::GetAttribute( const CString& name, T& value ) const
{
	const int pos = attributes.GetFirstPosition( name );
	if( pos == NotFound ) {
		return false;
	}

	const onnx::AttributeProto& attribute = attributes.GetValue( pos );
	GetAttributeValue<T>( attribute, value, *this );
	return true;
}

} // namespace NeoOnnx

