/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// FILE: INIParticleSys.cpp ///////////////////////////////////////////////////////////////////////////
// Author: Michael S. Booth, November 2001
// Desc:   Parsing Particle System INI entries
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"	// This must go first in EVERY cpp file in the GameEngine

#include "Common/INI.h"
#include "GameClient/ParticleSys.h"


/**
 * Parse entry
 */
void INI::parseParticleSystemDefinition( INI* ini )
{
	AsciiString name;

	// read the name
	const char* c = ini->getNextToken();
	name.set( c );

	// find existing item if present
	ParticleSystemTemplate *sysTemplate = const_cast<ParticleSystemTemplate*>(TheParticleSystemManager->findTemplate( name ));
	if (sysTemplate == nullptr)
	{
		// no item is present, create a new one
		sysTemplate = TheParticleSystemManager->newTemplate( name );
	}

	// parse the ini definition
	ini->initFromINI( sysTemplate, sysTemplate->getFieldParse() );

#if PRESERVE_RETAIL_PARTICLES
	// TheSuperHackers @info Hack to allow isUsingSmudge() functionality with retail smudge particles
	// The retail data template for smudge particles is not correctly configured with the smudge particle type
	if (sysTemplate->m_particleType != ParticleSystemInfo::SMUDGE && sysTemplate->m_particleTypeName.startsWithNoCase("SMUDGE."))
	{
		sysTemplate->m_particleType = ParticleSystemInfo::SMUDGE;
	}

	// In retail, volume particle depth was not setup through ini and was hard coded to a particle depth of 6
	if (sysTemplate->m_particleType == ParticleSystemInfo::VOLUME_PARTICLE && sysTemplate->m_volumeParticleDepth == DEFAULT_VOLUME_PARTICLE_DEPTH)
	{
		sysTemplate->m_volumeParticleDepth = OPTIMUM_VOLUME_PARTICLE_DEPTH;
	}
#endif
}
