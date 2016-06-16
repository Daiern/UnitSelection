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

	//Create decal placeholder
	SelectDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Decal"));
	SelectDecal->AttachTo(RootComponent);
	SelectDecal->SetIsReplicated(true);
	SelectDecal->SetVisibility(false);

	bIsSelected = false;
	bIsDead = false;

	Team = "None";

}


void AUnitSelectionCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AUnitSelectionCharacter, bIsSelected);	
}


void AUnitSelectionCharacter::ShowDecal() {
	/*if (Role < ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Before Client call"));
		ClientShowDecal();
	} else {/*/
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Authority"));
		if (bIsSelected) {
			SelectDecal->SetVisibility(true);
		}
		else {
			SelectDecal->SetVisibility(false);
		}
	//}
}
		
void AUnitSelectionCharacter::ClientShowDecal_Implementation() {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("In Client Call"));
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

FName AUnitSelectionCharacter::GetTeam() {
	return Team;
}