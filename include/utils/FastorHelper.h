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

using Tensor3R = Fastor::Tensor< Real, 3 >;
using Tensor33R = Fastor::Tensor< Real, 3, 3 >;
using Tensor333R = Fastor::Tensor< Real, 3, 3, 3 >;
using Tensor3333R = Fastor::Tensor< Real, 3, 3, 3, 3 >;

template <>
inline void
dataStore( std::ostream & stream, Tensor3R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    storeHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataLoad( std::istream & stream, Tensor3R & d, void * context )
{

  for ( unsigned int i = 0; i < d.size(); i++ )
    loadHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataStore( std::ostream & stream, Tensor33R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    storeHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataLoad( std::istream & stream, Tensor33R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    loadHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataStore( std::ostream & stream, Tensor333R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    storeHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataLoad( std::istream & stream, Tensor333R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    loadHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataStore( std::ostream & stream, Tensor3333R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    storeHelper( stream, *( d.data() + i ), context );
}

template <>
inline void
dataLoad( std::istream & stream, Tensor3333R & d, void * context )
{
  for ( unsigned int i = 0; i < d.size(); i++ )
    loadHelper( stream, *( d.data() + i ), context );
}
