// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "USAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNITSELECTION_API AUSAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUSAIController();
	
	virtual void Tick(float DeltaTime) override;

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetNewMoveDestination(const FVector DestLocation);
	void ServerSetNewMoveDestination_Implementation(const FVector DestLocation);
	bool ServerSetNewMoveDestination_Validate(const FVector DestLocation);
	
	
};
