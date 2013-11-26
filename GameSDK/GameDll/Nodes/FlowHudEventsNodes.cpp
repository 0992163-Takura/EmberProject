////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2013.
// -------------------------------------------------------------------------
//  File name:   FlowHudEventsNodes.cpp
//  Version:     v1.00
//  Created:     August 8th 2013 by Michiel Meesters.
//  Compilers:   Visual Studio.NET 2012
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UI/HUD/HUDEventDispatcher.h"
#include "Nodes/G2FlowBaseNode.h"
#include "UI/HUD/HUDMissionObjectiveSystem.h"

class CFlowNode_MissionStateListener : public CFlowBaseNode<eNCT_Instanced>, IHUDEventListener
{
private:
	SActivationInfo m_pActInfo;
public:
	//////////////////////////////////////////////////////////////////////////
	CFlowNode_MissionStateListener(SActivationInfo *pActInfo) {m_pActInfo = *pActInfo;}

	//////////////////////////////////////////////////////////////////////////
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
	{
		return new CFlowNode_MissionStateListener(pActInfo);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual ~CFlowNode_MissionStateListener()
	{
		CHUDEventDispatcher::RemoveHUDEventListener(this);
	}

	//////////////////////////////////////////////////////////////////////////
	enum OutputPorts
	{
		eOP_StateChanged = 0,
		eOP_Desc,
		eOP_Msg,
		eOP_State
	};

	//////////////////////////////////////////////////////////////////////////
	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		// declare input ports
		static const SOutputPortConfig out_ports[] = 
		{
			OutputPortConfig_Void("StateChanged", "fires when a mission's state is changed"),
			OutputPortConfig<string>("Name", "Missions name"),
			OutputPortConfig<string>("Description", "Missions description"),
			OutputPortConfig<int>("Status", "0=deactivated, 1=completed, 2=failed, 3=activated"),
			{0}
		};
		
		// we set pointers in "config" here to specify which input and output ports the node contains
		config.sDescription = _HELP("Fires whenever a mission's state has been changed");
		config.pInputPorts = 0;
		config.pOutputPorts = out_ports;
		config.SetCategory(EFLN_APPROVED);
	}
	
	//////////////////////////////////////////////////////////////////////////
	void Serialize(SActivationInfo* pActInfo, TSerialize ser)
	{
	}
 
	//////////////////////////////////////////////////////////////////////////
	virtual void OnHUDEvent(const SHUDEvent& event)
	{
		// Since I only handle OnObjectiveChanged I only expect a void pointer and a boolean, fail in other cases.
		CHUDMissionObjective* pMissionObjective = NULL;
		bool bIsSilent = false;

		if(event.eventType == eHUDEvent_OnObjectiveChanged)
		{
			for(unsigned int i = 0; i < event.GetDataSize(); i++)
			{
				switch(event.GetData(i).m_type)
				{
				case SHUDEventData::eSEDT_voidptr:
					pMissionObjective = (CHUDMissionObjective*)event.GetData(i).GetPtr();
					break;
				case SHUDEventData::eSEDT_bool:
					bIsSilent = event.GetData(i).GetBool();
					break;
				case SHUDEventData::eSEDT_int:
					// not nescessary now
					break;
				case SHUDEventData::eSEDT_float:
					// not nescessary now
					break;
				case SHUDEventData::eSEDT_undef:
				default:
					CryWarning(VALIDATOR_MODULE_FLOWGRAPH, VALIDATOR_WARNING, "[CFlowNode_MissionStateListener] HudEvent data unknown.");
					break;
				}
			}
			if(pMissionObjective && !bIsSilent)
			{
				// activate outputport
				ActivateOutput(&m_pActInfo, eOP_StateChanged, 1);
				ActivateOutput<string>(&m_pActInfo, eOP_Desc, string(pMissionObjective->GetShortDescription()));
				ActivateOutput<string>(&m_pActInfo, eOP_Msg, string(pMissionObjective->GetMessage()));
				ActivateOutput<int>(&m_pActInfo, eOP_State, pMissionObjective->GetStatus());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		if (event == eFE_Initialize)
		{
			m_pActInfo = *pActInfo;
			CHUDEventDispatcher::AddHUDEventListener(this, "OnObjectiveChanged");
		}
	}

	//////////////////////////////////////////////////////////////////////////
	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}
};


class CFlowNode_EntityTrackedListener : public CFlowBaseNode<eNCT_Instanced>, IHUDEventListener
{
private:
	SActivationInfo m_pActInfo;
public:
	//////////////////////////////////////////////////////////////////////////
	CFlowNode_EntityTrackedListener(SActivationInfo *pActInfo) {m_pActInfo = *pActInfo;}

	//////////////////////////////////////////////////////////////////////////
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo)
	{
		return new CFlowNode_EntityTrackedListener(pActInfo);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual ~CFlowNode_EntityTrackedListener()
	{
		CHUDEventDispatcher::RemoveHUDEventListener(this);
	}

	//////////////////////////////////////////////////////////////////////////
	enum InputPorts
	{
		eIP_MissionOnly = 0,
		eIP_Class,
		eIP_CustomClass
	};
	
	enum OutputPorts
	{
		eOP_EntityAdded = 0,
		eOP_EntityRemoved,
		eOP_EntityId
	};

	//////////////////////////////////////////////////////////////////////////
	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		// declare input ports
		static const SInputPortConfig in_ports[] = 
		{
			InputPortConfig<bool>("MissionOnly", "Only trigger on events involving mission tracked entities"),
			InputPortConfig<string>("Class", "AllClasses", "Class yuou want to filter on, CustomClasses: fill in box below", 0,  _UICONFIG("enum_global:entity_classes") ),
			InputPortConfig<string>("CustomClasses", "Optionally: Add classes that you want to filter the entities on (comma separated)"),
			{0}
		};

		// declare output ports
		static const SOutputPortConfig out_ports[] = 
		{
			OutputPortConfig_Void("EntityAdded", "fires when a entity is added to the radar from the Mission"),
			OutputPortConfig_Void("EntityRemoved", "fires when a entity is removed from the radar by a Mission"),
			OutputPortConfig<EntityId>("Entity", "The entity that needs adding"),
			{0}
		};

		// we set pointers in "config" here to specify which input and output ports the node contains
		config.sDescription = _HELP("Fires when entities are added or removed to the radar from a mission");
		config.pInputPorts = in_ports;
		config.pOutputPorts = out_ports;
		config.SetCategory(EFLN_APPROVED);
	}

	//////////////////////////////////////////////////////////////////////////
	inline void splitStringList(std::vector<string>* result, const char *str, char delimeter)
	{
		result->clear();

		const char* ptr = str;
		for(; *ptr; ++ptr)
		{
			if(*ptr == delimeter){
				result->push_back(string(str, ptr));
				str = ptr + 1;
			}
		}
		result->push_back(string(str, ptr));
	}

	//////////////////////////////////////////////////////////////////////////
	void Serialize(SActivationInfo* pActInfo, TSerialize ser)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	bool IsClassAllowed(const char* entClass)
	{
		const char* classFilter = GetPortString(&m_pActInfo, eIP_Class).c_str();
		if(strcmp(classFilter, "AllClasses") == 0 || strcmp(classFilter, entClass) == 0)
		{
			return true;
		}
		else if(strcmp(classFilter, "CustomClasses") == 0)
		{
			string customClasses = GetPortString(&m_pActInfo, eIP_CustomClass);
			std::vector<string> CustomClassList;
			splitStringList(&CustomClassList, customClasses.c_str(), ',');
			for (unsigned int i = 0; i < CustomClassList.size(); i++)
			{
				if(strcmp(entClass, CustomClassList[i]) == 0)
					return true;
			}
			
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	virtual void OnHUDEvent(const SHUDEvent& event)
	{
		// Only add/remove entity is handled here, so we only care about the int param
		EntityId entityId = 0;

		if(event.eventType == eHUDEvent_AddEntity || event.eventType == eHUDEvent_RemoveEntity)
		{
			for(unsigned int i = 0; i < event.GetDataSize(); i++)
			{
				switch(event.GetData(i).m_type)
				{
				case SHUDEventData::eSEDT_voidptr:
					break;
				case SHUDEventData::eSEDT_bool:
					break;
				case SHUDEventData::eSEDT_int:
					entityId = event.GetData(i).GetInt();
					break;
				case SHUDEventData::eSEDT_float:
					break;
				case SHUDEventData::eSEDT_undef:
				default:
					CryWarning(VALIDATOR_MODULE_FLOWGRAPH, VALIDATOR_WARNING, "[CFlowNode_EntityTrackedListener] HudEvent data unknown.");
					break;
				}
			}
			if(entityId != 0)
			{
				CHUDMissionObjective* pMO = NULL;
				pMO = g_pGame->GetMOSystem()->GetMissionObjectiveByEntityId(entityId);

				string entityClassName = gEnv->pEntitySystem->GetEntity(entityId)->GetClass()->GetName();
				bool bMissionOnly = GetPortBool(&m_pActInfo, eIP_MissionOnly);
				bool bTrigger = false;

				if(bMissionOnly)
				{
						bTrigger = IsClassAllowed(entityClassName) && pMO;
				}
				else
				{
						bTrigger = IsClassAllowed(entityClassName);
				}

				if(bTrigger)
				{
					// activate outputport
					if(event.eventType == eHUDEvent_AddEntity)
						ActivateOutput(&m_pActInfo, eOP_EntityAdded, 1);
					else
						ActivateOutput(&m_pActInfo, eOP_EntityRemoved, 1);

					ActivateOutput<EntityId>(&m_pActInfo, eOP_EntityId, entityId);
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		if (event == eFE_Initialize)
		{
			m_pActInfo = *pActInfo;
			CHUDEventDispatcher::AddHUDEventListener(this, "AddEntity");
			CHUDEventDispatcher::AddHUDEventListener(this, "RemoveEntity");
		}
	}

	//////////////////////////////////////////////////////////////////////////
	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}
};

REGISTER_FLOW_NODE("HUD:MissionStateListener",	CFlowNode_MissionStateListener);
REGISTER_FLOW_NODE("HUD:EntityTrackedListener",	CFlowNode_EntityTrackedListener);