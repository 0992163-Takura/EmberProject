#include "stdafx.h"
#include "rappleAction.h"
#include <Cry_Math.h>

rappleAction::rappleAction(IEntity * ee)
{
    if(gEnv->pInput)
        gEnv->pInput->AddEventListener(this);
    auto myCVar = gEnv->pConsole->GetCVar("cl_tpvMaxWeapDistDebug");
    myCVar->Set("1");
    player = ee;
	//cMovement = new CPlayerMovementController((CPlayer*)player);
    flowManager = gEnv->pFlowSystem->GetIModuleManager();
					CryLogAlways("rAction startup");
	ready = false;
}


rappleAction::~rappleAction(void)
{
}


void rappleAction::getDist(Vec3 pos, Vec3 hit)
{
    start = pos;
    end = hit;
}

bool rappleAction::OnInputEvent( const SInputEvent &event )
{
    if (event.deviceId!=eDI_Keyboard)
        return false;

    if (event.keyId == eKI_SYS_Commit)
        return false;

    if(eIS_Pressed == event.state)
        {
            if(event.keyId == eKI_J)
                {
                    //executeGrappleAction();
					ready = true;
					CryLogAlways("test this shit");
                    return true;
                }
            if(event.keyId == eKI_U)
                {
                    //executeGrappleAction();
					 auto myCVar = gEnv->pConsole->GetCVar("cl_tpvMaxWeapDistDebug");
					 myCVar->Set("1");
					CryLogAlways("test this shit");
                    return true;
                }
	}

    return false;
}

float rappleAction::returnAngleBetweenTwoVec(Vec3 start, Vec3 end)
{
	float fA, fB, fDot;
    fA = sqrtf(start.x * start.x + start.y * start.y + start.z + start.z);
    fB = sqrtf(end.x * end.x + end.y * end.y + end.z + end.z);
    fDot = start.x * end.x + start.y * end.y + start.z + end.z;
	return cry_acosf(fDot / (fA * fB));
}
//vA:Vec3D, vB:Vec3D\


void  rappleAction::attemptSwing(const float fTime)
{
	
	if(ready)
	{
		//ready = ! ready;
		CryLogAlways("test this shit");
		//gEnv->pRenderer->Draw2dLabel(50,50,5.0, Col_Blue, false, "this is a test");
		/*
		f32 xx = end.x - start.x;
    	f32 yy = end.y - start.y;
    	f32 zz = end.z - start.z;

    	f32 magnitude = sqrt(xx*xx*yy*yy*zz*zz);

		f32 horizontalDistance = magnitude * sin(returnAngleBetweenTwoVec(start, end));
		horizontalDistance *= 2;

		Vec3 endVec = start;
		endVec.x += horizontalDistance;
		endVec.y += horizontalDistance;

		player->SetPos(Vec3::CreateSlerp(start, endVec, fTime));
		*/
	}
	
}

//physex
/*

IPhysicalEntity* pPhysics = GetEntity()->GetPhysics();

if(pPhysics)
{
   pe_action_impulse impulse;
   impulse.angImpulse = Vec3(0.0f,10.0f,0.0f);

   pPhysics->Action(&impulse);
}

*/

void rappleAction::executeGrappleAction()
{

    auto myCVar = gEnv->pConsole->GetCVar("cl_tpvMaxWeapDistDebug");
    


	if(gEnv->pEntitySystem->GetClassRegistry()->FindClass("RopeEntity") == NULL)
		myCVar->Set("1");

    SEntitySpawnParams params;
    params.sName = "TestEntity";
    params.sLayerName = "TestLayer";
    params.nFlags = ENTITY_FLAG_NO_SAVE | ENTITY_FLAG_SPAWNED;
    params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("RopeEntity");//->GetDefaultClass();
    params.vScale = Vec3(1.0f,1.0f,1.0f);
    params.vPosition = end;
    auto e1 = gEnv->pEntitySystem->SpawnEntity(params, true);

    if (e1 != NULL)
    {
    // Pick a default material
    IMaterial *pMat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("material_default");
    e1->SetMaterial(pMat);

    // Load some simple test geometry
    //int slotId = e1->LoadGeometry(0, "objects/default/primitive_cylinder.cgf");
    }
    

}





    //flowManager->GetModule(
    /*
    //gEnv->pRenderer->DrawLabel(end, 20, "This is a test");
    {
    	f32 xx = (end.x - start.x)/100;
    	f32 yy = (end.y - start.y)/100;
    	f32 zz = (end.z - start.z)/100;

    	//f32 magnitude = sqrt(xx*yy*zz);
    	//magnitude /= 100;


    	//Vec3 nIntegral = Vec3(xx, yy, zz);
    	Vec3 staticEnd = end;

    	for(int i = 0; i < 100; i++)
    	{
    		staticEnd.x += xx;
    		staticEnd.y += yy;
    		staticEnd.z += zz;
    		player->SetPos(staticEnd);
    	}
    }
    */
    //player->SetPos(end);


    

