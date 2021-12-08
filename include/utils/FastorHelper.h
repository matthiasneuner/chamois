/* ---------------------------------------------------------------------
 *       _                           _
 *   ___| |__   __ _ _ __ ___   ___ (_)___
 *  / __| '_ \ / _` | '_ ` _ \ / _ \| / __|
 * | (__| | | | (_| | | | | | | (_) | \__ \
 *  \___|_| |_|\__,_|_| |_| |_|\___/|_|___/
 *
 * Chamois - a MOOSE interface to constitutive models developed at the
 * Unit of Strength of Materials and Structural Analysis
 * University of Innsbruck,
 * 2020 - today
 *
 * Matthias Neuner matthias.neuner@uibk.ac.at
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The full text of the license can be found in the file LICENSE.md at
 * the top level directory of chamois.
 * ---------------------------------------------------------------------
 */

#pragma once

#include "Moose.h"
#include <Fastor/Fastor.h>

#include "Fastor/config/config.h"
#include "Fastor/backend/backend.h"
#include "Fastor/simd_vector/SIMDVector.h"
#include "Fastor/tensor/AbstractTensor.h"
#include "Fastor/tensor/Ranges.h"
#include "Fastor/tensor/ForwardDeclare.h"
#include "Fastor/expressions/linalg_ops/linalg_ops.h"
#include "Fastor/tensor/TensorIO.h"

namespace Fastor
{
template < typename T, size_t... Rest >
/* class CopyableTensor: public AbstractTensor<TensorMap<T, Rest...>,sizeof...(Rest)> */ 
class CopyableTensor: public TensorMap<T, Rest...>{
public:
  static constexpr Fastor::FASTOR_INDEX size() { return Fastor::pack_prod< Rest... >::value; }

  constexpr inline T * data() const { return const_cast< T * >( this->_data ); }

  CopyableTensor():TensorMap<T, Rest...>(_data){};
  CopyableTensor( const Fastor::Tensor< T, Rest... > & other )
      :TensorMap<T, Rest...> ( _data )
  {
    // This constructor cannot be default
    if ( _data == other.data() )
      return;
    // fast memcopy
    std::copy( other.data(), other.data() + size(), _data );
  };

  Fastor::Tensor< T, Rest... > retrieveTensor() const
  {
    return Fastor::Tensor< T, Rest... >( this->data() );
  };

/* #undef SCALAR_INDEXING_NONCONST_H */
/* #undef SCALAR_INDEXING_CONST_H */
/* #undef INDEX_RETRIEVER_H */
/* #include "Fastor/tensor/IndexRetriever.h" */
/* #include "Fastor/tensor/ScalarIndexing.h" */
/* #define INDEX_RETRIEVER_H */
/* #define SCALAR_INDEXING_NONCONST_H */
/* #define SCALAR_INDEXING_CONST_H */

private:
  T _data[Fastor::pack_prod< Rest... >::value];
};

}

using Tensor3d = Fastor::Tensor< double, 3 >;
using Tensor33d = Fastor::Tensor< double, 3, 3 >;
using Tensor333d = Fastor::Tensor< double, 3, 3, 3 >;
using Tensor3333d = Fastor::Tensor< double, 3, 3, 3, 3 >;

using TensorData3R = Fastor::CopyableTensor< Real, 3 >;
using TensorData33R = Fastor::CopyableTensor< Real, 3, 3 >;
using TensorData333R = Fastor::CopyableTensor< Real, 3, 3, 3 >;
using TensorData3333R = Fastor::CopyableTensor< Real, 3, 3, 3, 3 >;
