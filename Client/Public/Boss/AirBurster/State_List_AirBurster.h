#pragma once
#include "Client_Manager.h"

// ---- Main ------
#include "../Public/Boss/AirBurster/State/State_AirBurster_IDLE.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_Transform.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Control_Phase1.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Control_Phase2.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_HeatDmg.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_BurstDmg.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_FrontMachineGun.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_RearMachineGun.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_EnergyBall.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_FingerBeam.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_EMField.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_TankBurster.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Burner.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_ShoulderBeam.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_RocketArm.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Docking.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_Intro.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_1PhaseEnter.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_2PhaseEnter.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_Dead.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_Turn180.h"

#include "../Public/Boss/AirBurster/State/State_AirBurster_Hook.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Hook_Return.h"

#include "Boss/AirBurster/State/State_AirBurster_Burst.h"

// ---- Arm ------

#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_IDLE.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Separate.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Docking.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_FingerBeam.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Dead.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Term.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Push.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Push_Back.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Return.h"

// ----- Weapon -------

#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"
#include "Boss/AirBurster/Weapon/AirBurster_Burner.h"
#include "Boss/AirBurster/Weapon/AirBurster_EMField.h"
#include "Boss/AirBurster/Weapon/AirBurster_EnergyBall.h"
#include "Boss/AirBurster/Weapon/AirBurster_FingerBeam.h"
#include "Boss/AirBurster/Weapon/AirBurster_MachineGun.h"
#include "Boss/AirBurster/Weapon/AirBurster_TankBurster.h"
#include "Boss/AirBurster/Weapon/AirBurster_Hook.h"
#include "Boss/AirBurster/Weapon/AirBurster_Block.h"