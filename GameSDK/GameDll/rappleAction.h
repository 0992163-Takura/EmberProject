#pragma once

#include <Game.h>
#include <IFlowGraphModuleManager.h>
#include "c:\users\apsalar\downloads\cryengine_pc_v_3_5_4_1509_freesdk\code\cryengine\crycommon\iinput.h"

class rappleAction :
    public IInputEventListener
{
public:
    rappleAction(IEntity * ee);
    ~rappleAction(void);

    VIRTUAL void getDist(Vec3 pos, Vec3 hit);
    VIRTUAL bool OnInputEvent( const SInputEvent &event );

    void executeGrappleAction();
	void attemptSwing(const float fTime);
	float returnAngleBetweenTwoVec(Vec3 start, Vec3 end);
private:
	bool ready;
    Vec3 start;
    Vec3 end;
    IEntity * player;
    IFlowGraphModuleManager * flowManager;
	//CPlayerMovementController * cMovement;
};

