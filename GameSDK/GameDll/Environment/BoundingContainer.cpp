/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2012.
-------------------------------------------------------------------------

Description: Bounding container to hide / unhide items

-------------------------------------------------------------------------
History:
- 18:04:2012: Created by Dean Claassen

*************************************************************************/

#include "StdAfx.h"
#include "BoundingContainer.h"

#include "EntityUtility/EntityScriptCalls.h"
#include <IItemSystem.h>

//////////////////////////////////////////////////////////////////////////

namespace BC
{
	void RegisterEvents( IGameObjectExtension& goExt, IGameObject& gameObject )
	{
		const int eventID = eGFE_ScriptEvent;
		gameObject.UnRegisterExtForEvents( &goExt, NULL, 0 );
		gameObject.RegisterExtForEvents( &goExt, &eventID, 1 );
	}
}

CBoundingContainer::CBoundingContainer()
: m_vBoundingMin(0.0f, 0.0f, 0.0f)
, m_vBoundingMax(0.0f, 0.0f, 0.0f)
{
}

CBoundingContainer::~CBoundingContainer()
{
}

bool CBoundingContainer::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);

	SetupEntity();

	return true;
}

void CBoundingContainer::PostInit( IGameObject * pGameObject )
{
	BC::RegisterEvents( *this, *pGameObject );
}

bool CBoundingContainer::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();
	BC::RegisterEvents( *this, *pGameObject );

	CRY_ASSERT_MESSAGE(false, "CBoundingContainer::ReloadExtension not implemented");

	return false;
}

bool CBoundingContainer::GetEntityPoolSignature( TSerialize signature )
{
	CRY_ASSERT_MESSAGE(false, "CBoundingContainer::GetEntityPoolSignature not implemented");

	return true;
}

void CBoundingContainer::Release()
{
	delete this;
}

void CBoundingContainer::FullSerialize( TSerialize serializer )
{
	serializer.Value("?Hidden?Entities", m_hiddenEntities);
}

void CBoundingContainer::PostSerialize()
{
}

void CBoundingContainer::Update( SEntityUpdateContext& ctx, int slot )
{
}

void CBoundingContainer::HandleEvent( const SGameObjectEvent& gameObjectEvent )
{
	if ((gameObjectEvent.event == eGFE_ScriptEvent) && (gameObjectEvent.param != NULL))
	{
		const char* szEventName = static_cast<const char*>(gameObjectEvent.param);

		if (strcmp(szEventName, "ShowContainedItems") == 0)
		{
			ShowContainedItems();
		}
		else if (strcmp(szEventName, "HideContainedItems") == 0)
		{
			HideContainedItems();
		}
	}
}

void CBoundingContainer::ProcessEvent( SEntityEvent& entityEvent )
{
	switch(entityEvent.event)
	{
	case ENTITY_EVENT_RESET:
		{
			const bool bEnteringGameMode = ( entityEvent.nParam[ 0 ] == 1 );
			Reset( bEnteringGameMode );
		}
		break;
	case ENTITY_EVENT_START_LEVEL: // Need to wait till all entities are loaded since other entities can be inside
		{
			bool bHideContainedItemsOnStart = false;
			EntityScripts::GetEntityProperty(GetEntity(), "bHideContainedItemsOnStart", bHideContainedItemsOnStart);
			if (bHideContainedItemsOnStart && !gEnv->IsEditor())
			{
				HideContainedItems();
			}
		}
		break;
	}
}

void CBoundingContainer::GetMemoryUsage( ICrySizer *pSizer ) const
{

}

void CBoundingContainer::SetupEntity()
{
	IEntity* pEntity = GetEntity();
	CRY_ASSERT(pEntity != NULL);

	IScriptTable* pScriptTable = pEntity->GetScriptTable();
	if (pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			Vec3 boundingDimensions(0.0f,0.0f,0.0f);
			propertiesTable->GetValue("DimX", boundingDimensions.x);
			propertiesTable->GetValue("DimY", boundingDimensions.y);
			propertiesTable->GetValue("DimZ", boundingDimensions.z);

			m_vBoundingMin = -boundingDimensions/2.0f;
			m_vBoundingMax = boundingDimensions/2.0f;
		}
	}
}

void CBoundingContainer::Reset( const bool bEnteringGameMode )
{
	SetupEntity();

	if (bEnteringGameMode)
	{
		HideContainedItems();
	}
}

void CBoundingContainer::ShowContainedItems()
{
	THiddenEntities::iterator iter = m_hiddenEntities.begin();
	const THiddenEntities::const_iterator iterEnd = m_hiddenEntities.end();
	while (iter != iterEnd)
	{
		EntityId entityId = *iter;
		IEntity* pHiddenEntity = gEnv->pEntitySystem->GetEntity(entityId);
		if (pHiddenEntity)
		{
			pHiddenEntity->Hide(false);
		}

		++iter;
	}
	m_hiddenEntities.clear();
}

void CBoundingContainer::HideContainedItems()
{
	IEntity* pEntity = GetEntity();
	if (pEntity)
	{
		// Proximity query all entities in area
		AABB aabbBounds(m_vBoundingMin, m_vBoundingMax);

		OBB	obbBounds;
		Matrix34	worldTM = pEntity->GetWorldTM();
		obbBounds.SetOBBfromAABB(Matrix33(worldTM), aabbBounds);
		aabbBounds.Reset();
		aabbBounds.SetAABBfromOBB(pEntity->GetWorldPos(), obbBounds);

		SEntityProximityQuery query;
		query.box = aabbBounds;
		gEnv->pEntitySystem->QueryProximity(query);
		const int iQueryCount = query.nCount;
		for (int i = 0; i < iQueryCount; ++i)
		{
			IEntity* pQueryEntity = query.pEntities[i];
			if (pQueryEntity)
			{
				const EntityId queryEntityId = pQueryEntity->GetId();
				
				if (!stl::find(m_hiddenEntities, queryEntityId)) // Only if not already hidden
				{
					// Make sure entity type should be hidden and entity pos is also inside, not just an intersection
					if (ShouldHide(queryEntityId, pQueryEntity) && aabbBounds.IsContainPoint(pQueryEntity->GetWorldPos()))
					{
						pQueryEntity->Hide(true);
						m_hiddenEntities.push_back(pQueryEntity->GetId());
					}
				}
			}
		}
	}
}

bool CBoundingContainer::ShouldHide( const EntityId entityId, const IEntity* pEntity )
{
	CRY_ASSERT( pEntity != NULL );
	
	// First check if is an item
	const IItem* pItem = g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem( entityId );
	if ( pItem != NULL)
	{
		return true;
	}

	// Try hardcoded entity types
	static IEntityClass* s_pInteractiveEntityClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("InteractiveEntity");

	IEntityClass* pEntityClass = pEntity->GetClass();
	if (pEntityClass != NULL && pEntityClass == s_pInteractiveEntityClass)
	{
		return true;
	}

	return false;
}

/*
void CBoundingContainer::DrawDebugInfo()
{
	IRenderAuxGeom* pRenderAuxGeom = gEnv->pRenderer->GetIRenderAuxGeom();
	AABB aabbBounds(m_vBoundingMin, m_vBoundingMax);
	IEntity* pEntity = GetEntity();

	OBB	obbBounds;
	Matrix34	worldTM = pEntity->GetWorldTM();
	obbBounds.SetOBBfromAABB(Matrix33(worldTM), aabbBounds);
	aabbBounds.Reset();
	aabbBounds.SetAABBfromOBB(pEntity->GetWorldPos(), obbBounds);

	pRenderAuxGeom->DrawAABB(aabbBounds, Matrix34::CreateIdentity(), false, Col_CornflowerBlue, eBBD_Faceted);
}
*/