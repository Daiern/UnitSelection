// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
#include "Engine.h"
#include "UnitSelectionCharacter.generated.h"

UCLASS(Blueprintable)
class AUnitSelectionCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Selection, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bIsSelected;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Selection, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bIsDead;



public:
	AUnitSelectionCharacter();

	FORCEINLINE bool GetIsSelected() { return bIsSelected; }

	FORCEINLINE void SetIsSelected(bool select) { bIsSelected = select; }

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
	class UDecalComponent* SelectDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
	FName Team;

	FName GetTeam();

	UFUNCTION(Reliable, NetMulticast)
	void MCShowDecal();
	void MCShowDecal_Implementation();
	//bool ServerShowDecal_Validate();
};

