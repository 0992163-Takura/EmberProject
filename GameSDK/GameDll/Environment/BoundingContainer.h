/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2012.
-------------------------------------------------------------------------

Description: Bounding container to hide / unhide items

-------------------------------------------------------------------------
History:
- 18:04:2012: Created by Dean Claassen

*************************************************************************/

#pragma once

#ifndef _BOUNDING_CONTAINER_H_
#define _BOUNDING_CONTAINER_H_

#include <IGameObject.h>
#include "../State.h"

#if defined(USER_dean) && !defined(_RELEASE)
	#define DEBUG_BOUNDING_CONTAINER
#endif

//////////////////////////////////////////////////////////////////////////
/// BOUNDING CONTAINER

class CBoundingContainer : public CGameObjectExtensionHelper<CBoundingContainer, IGameObjectExtension>
{

public:
	CBoundingContainer();
	virtual ~CBoundingContainer();

	// IGameObjectExtension
	virtual bool Init( IGameObject * pGameObject );
	virtual void InitClient( int channelId ) {};
	virtual void PostInit( IGameObject * pGameObject );
	virtual void PostInitClient( int channelId ) {};
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {}
	virtual bool GetEntityPoolSignature( TSerialize signature );
	virtual void Release();
	virtual void FullSerialize( TSerialize ser );
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) { return false; };
	virtual void PostSerialize();
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() {return 0;}
	virtual void Update( SEntityUpdateContext& ctx, int slot );
	virtual void HandleEvent( const SGameObjectEvent& gameObjectEvent );
	virtual void ProcessEvent( SEntityEvent& entityEvent );
	virtual void SetChannelId( uint16 id ) {};
	virtual void SetAuthority( bool auth ) {};
	virtual void PostUpdate( float frameTime ) { CRY_ASSERT(false); }
	virtual void PostRemoteSpawn() {};
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const;
	// ~IGameObjectExtension

private:
	void SetupEntity();
	void Reset( const bool bEnteringGameMode );
	void ShowContainedItems();
	void HideContainedItems();
	bool ShouldHide( const EntityId entityId, const IEntity* pEntity );

	typedef std::vector<EntityId> THiddenEntities;
	THiddenEntities m_hiddenEntities;

	Vec3 m_vBoundingMin;
	Vec3 m_vBoundingMax;
};

#endif //_BOUNDING_CONTAINER_H_