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

#include "DataIO.h"
#include "Moose.h"
#include <Fastor/Fastor.h>

/* #include "Fastor/config/config.h" */
/* #include "Fastor/backend/backend.h" */
/* #include "Fastor/simd_vector/SIMDVector.h" */
/* #include "Fastor/tensor/AbstractTensor.h" */
/* #include "Fastor/tensor/Ranges.h" */
/* #include "Fastor/tensor/ForwardDeclare.h" */
/* #include "Fastor/expressions/linalg_ops/linalg_ops.h" */
/* #include "Fastor/tensor/TensorIO.h" */

/* namespace Fastor */
/* { */
/* template < typename T, size_t... Rest > */
/* /1* class CopyableTensor: public AbstractTensor<TensorMap<T, Rest...>,sizeof...(Rest)> *1/ */ 
/* class CopyableTensor: public TensorMap<T, Rest...>{ */
/* public: */
/*   /1* static constexpr Fastor::FASTOR_INDEX size() { return Fastor::pack_prod< Rest... >::value; } *1/ */

/*   constexpr inline T * data() const { return const_cast< T * >( this->_data ); } */

/*   CopyableTensor():TensorMap<T, Rest...>(this->data()){}; */
/*   CopyableTensor(const Fastor::CopyableTensor< T, Rest... > & other ):TensorMap<T, Rest...>(this->data()) */
/*   { */
/*     // This constructor cannot be default */
/*     if ( _data == other.data() ) */
/*       return; */
/*     // fast memcopy */
/*     std::copy( other.data(), other.data() + this->size(), _data ); */
/*   }; */

/*   CopyableTensor( const Fastor::Tensor< T, Rest... > & other ) :TensorMap<T, Rest...> ( this->data() ) */
/*   { */
/*     // This constructor cannot be default */
/*     if ( _data == other.data() ) */
/*       return; */
/*     // fast memcopy */
/*     std::copy( other.data(), other.data() + this->size(), _data ); */
/*   }; */

/*   Fastor::Tensor< T, Rest... > retrieveTensor() const */
/*   { */
/*     return Fastor::Tensor< T, Rest... >( this->data() ); */
/*   }; */

/* /1* #undef SCALAR_INDEXING_NONCONST_H *1/ */
/* /1* #undef SCALAR_INDEXING_CONST_H *1/ */
/* /1* #undef INDEX_RETRIEVER_H *1/ */
/* /1* #include "Fastor/tensor/IndexRetriever.h" *1/ */
/* /1* #include "Fastor/tensor/ScalarIndexing.h" *1/ */
/* /1* #define INDEX_RETRIEVER_H *1/ */
/* /1* #define SCALAR_INDEXING_NONCONST_H *1/ */
/* /1* #define SCALAR_INDEXING_CONST_H *1/ */

/* private: */
/*   T _data[Fastor::pack_prod< Rest... >::value]; */
/* }; */

/* } */

using Tensor3R = Fastor::Tensor< Real, 3 >;
using Tensor33R = Fastor::Tensor< Real, 3, 3 >;
using Tensor333R = Fastor::Tensor< Real, 3, 3, 3 >;
using Tensor3333R = Fastor::Tensor< Real, 3, 3, 3, 3 >;

/* using TensorData3R = Fastor::CopyableTensor< Real, 3 >; */
/* using TensorData33R = Fastor::CopyableTensor< Real, 3, 3 >; */
/* using TensorData333R = Fastor::CopyableTensor< Real, 3, 3, 3 >; */
/* using TensorData3333R = Fastor::CopyableTensor< Real, 3, 3, 3, 3 >; */


/* class Foo */
/* { */
/*  public: */
/*   int bar; */
/*   std::string baz; */
/*   std::vector<unsigned int> qux; */
/* }; */


/* // Definition */
/* template <typename T, size_t ... Rest> */
/* inline void */
/* dataStore(std::ostream & stream, Fastor::Tensor<T, Rest...> & d, void * context) */
/* { */
/*    for (unsigned int i = 0; i < d.size(); i++) */
/*      storeHelper(stream, *(d.data()+i), context); */
/* } */

/* template <typename T, size_t ... Rest> */
/* inline void */
/* dataLoad(std::ostream & stream, Fastor::Tensor<T, Rest...> & d, void * context) */
/* { */
    
/*   for (unsigned int i = 0; i < d.size(); i++) */
/*     loadHelper(stream, *(d.data()+i), context ); */
/* } */

/* template void dataLoad(std::istream & stream, Fastor::Tensor<Real, 33>& d , void * context); */
/* template void dataStore(std::ostream & stream, Tensor3R& d , void * context); */
/* template void dataLoad(std::istream & stream, Tensor33R& d , void * context); */
/* template void dataStore(std::ostream & stream, Tensor33R& d , void * context); */
/* template void dataLoad(std::istream & stream, Tensor333R& d , void * context); */
/* template void dataStore(std::ostream & stream, Tensor333R& d , void * context); */
/* template void dataLoad(std::istream & stream, Tensor3333R& d , void * context); */
/* template void dataStore(std::ostream & stream, Tensor3333R& d , void * context); */


template <>
inline void
dataStore(std::ostream & stream, Tensor3R& d, void * context)
{
   for (unsigned int i = 0; i < d.size(); i++)
     storeHelper(stream, *(d.data()+i), context);
}

template <>
inline void
dataLoad(std::istream & stream, Tensor3R& d , void * context)
{
    
  for (unsigned int i = 0; i < d.size(); i++)
    loadHelper(stream, *(d.data()+i), context );
}

template <>
inline void
dataStore(std::ostream & stream, Tensor33R& d, void * context)
{
   for (unsigned int i = 0; i < d.size(); i++)
     storeHelper(stream, *(d.data()+i), context);
}

template <>
inline void
dataLoad(std::istream & stream, Tensor33R& d , void * context)
{
  for (unsigned int i = 0; i < d.size(); i++)
    loadHelper(stream, *(d.data()+i), context );
}


template <>
inline void
dataStore(std::ostream & stream, Tensor333R& d, void * context)
{
   for (unsigned int i = 0; i < d.size(); i++)
     storeHelper(stream, *(d.data()+i), context);
}

template <>
inline void
dataLoad(std::istream & stream, Tensor333R& d , void * context)
{
  for (unsigned int i = 0; i < d.size(); i++)
    loadHelper(stream, *(d.data()+i), context );
}

template <>
inline void
dataStore(std::ostream & stream, Tensor3333R& d, void * context)
{
   for (unsigned int i = 0; i < d.size(); i++)
     storeHelper(stream, *(d.data()+i), context);
}

template <>
inline void
dataLoad(std::istream & stream, Tensor3333R& d , void * context)
{
  for (unsigned int i = 0; i < d.size(); i++)
    loadHelper(stream, *(d.data()+i), context );
}
