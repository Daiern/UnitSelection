// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnitSelection.h"
#include "UnitSelectionGameMode.h"
#include "UnitSelectionPlayerController.h"
#include "UnitSelectionCharacter.h"

AUnitSelectionGameMode::AUnitSelectionGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AUnitSelectionPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> RTSCameraBPClass(TEXT("/Game/Blueprints/RTSCamera"));
	if (RTSCameraBPClass.Class != NULL)
	{
		DefaultPawnClass = RTSCameraBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> RTSHUDBPClass(TEXT("/Game/Blueprints/RTSHUD"));
	if (RTSHUDBPClass.Class != NULL)
	{
		HUDClass = RTSHUDBPClass.Class;
	}


}