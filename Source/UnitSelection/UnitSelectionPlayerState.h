// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "UnitSelectionPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UNITSELECTION_API AUnitSelectionPlayerState : public APlayerState
{
	GENERATED_BODY()


	AUnitSelectionPlayerState();

	virtual void BeginPlay() override;
	
};
