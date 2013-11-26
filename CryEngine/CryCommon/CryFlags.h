////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2012.
// -------------------------------------------------------------------------
//  File name:   CryFlags.h
//  Created:     24/01/2012 by Stephen M. North.
//  Description: File wrapper.
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __CryFlags_h__
#define __CryFlags_h__

template< typename STORAGE >
class CCryFlags
{
public:

	CCryFlags() : m_bitStorage(0) {}
	ILINE void AddFlags( STORAGE flags ) { m_bitStorage |= flags; }
	ILINE void ClearFlags( STORAGE flags ) { m_bitStorage &= ~flags; }
	ILINE bool AreAllFlagsActive( STORAGE flags ) const { return( (m_bitStorage & flags) == flags ); }
	ILINE bool AreAnyFlagsActive( STORAGE flags ) const { return( (m_bitStorage & flags) != 0 ); }
	ILINE void ClearAllFlags() { m_bitStorage = 0; }
	ILINE void SetFlags( STORAGE flags, const bool bSet ) { if( bSet ) m_bitStorage |= flags; else	m_bitStorage &= ~flags; }
	ILINE STORAGE GetRawFlags() const { return m_bitStorage; }

private:
	STORAGE m_bitStorage;
};

#endif // __CryFlags_h__
