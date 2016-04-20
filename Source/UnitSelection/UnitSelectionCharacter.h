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

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Selection, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bIsSelected;

public:
	AUnitSelectionCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE bool GetIsSelected() { return bIsSelected; }

	FORCEINLINE void SetIsSelected(bool select) { bIsSelected = select; }

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
	class UDecalComponent* SelectDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
	FName team;

	UFUNCTION(Reliable, NetMulticast)
	void MCShowDecal();
	void MCShowDecal_Implementation();
	//bool ServerShowDecal_Validate();
};

