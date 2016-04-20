// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitSelection.h"
#include "UnrealNetwork.h"
#include "Engine.h"
#include "USAIController.h"




AUSAIController::AUSAIController() {
	
}

void AUSAIController::Tick(float DeltaTime)
{
}

void AUSAIController::SetNewMoveDestination(const FVector DestLocation)
{

	if (Role < ROLE_Authority) {
		ServerSetNewMoveDestination(DestLocation);
	}

	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		MoveToLocation(DestLocation);
	}

	
}

void AUSAIController::ServerSetNewMoveDestination_Implementation(const FVector DestLocation) {

	
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = UNavigationSystem::GetCurrent(this);
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

bool AUSAIController::ServerSetNewMoveDestination_Validate(const FVector DestLocation) {
	return true;
}
