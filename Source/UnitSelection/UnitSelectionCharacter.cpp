// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnitSelection.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "UnitSelectionCharacter.h"

AUnitSelectionCharacter::AUnitSelectionCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create decal placeholder
	SelectDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Decal"));
	SelectDecal->AttachTo(RootComponent);
	SelectDecal->SetIsReplicated(true);
	SelectDecal->SetVisibility(false);

	bIsSelected = false;

	team = "None";

}


void AUnitSelectionCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AUnitSelectionCharacter, bIsSelected);	
}

void AUnitSelectionCharacter::MCShowDecal_Implementation() {
	if (bIsSelected) {
		SelectDecal->SetVisibility(true);
	}
	else {
		SelectDecal->SetVisibility(false);
	}
}

/*bool AUnitSelectionCharacter::ServerShowDecal_Validate() {
	return true;
}*/
