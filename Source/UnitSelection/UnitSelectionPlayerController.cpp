// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnitSelection.h"
#include "UnitSelectionPlayerController.h"
#include "UnitSelectionCharacter.h"
#include "USAIController.h"
#include "UnrealNetwork.h"
#include "Engine.h"
//#include "UnitSelectionHUD.h"
#include "AI/Navigation/NavigationSystem.h"

AUnitSelectionPlayerController::AUnitSelectionPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	mouseStartPosition = FVector2D(0.f, 0.f);
	currentMousePosition = FVector2D(0.f, 0.f);

	lineFormationOffset = 300.f;

	PCHud = nullptr;

	TeamName = "Blue";
}

void AUnitSelectionPlayerController::BeginPlay() {
	Super::BeginPlay();

	PCHud = Cast<AUnitSelectionHUD>(GetHUD());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TeamName.ToString());
}


void AUnitSelectionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if (PCHud){
		if (PCHud->bIsSelecting == true) {

			ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(this->Player);

			if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport) {
				if (LocalPlayer->ViewportClient->GetMousePosition(currentMousePosition) == false)
				{
					return;
				}
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Marquee Selecting"));
				PCHud->vCurrentMousePos = currentMousePosition;
				if ((currentMousePosition - mouseStartPosition).Size() > 5) {
					CheckUnitUnderMarquee();
				}
			}

		}
	}

}

void AUnitSelectionPlayerController::CheckUnitUnderMarquee() {
	FVector2D startPoint, endPoint, screenLocation;
	FBox2D marqueeBox;

	
	
	if (PCHud) {
		startPoint = FVector2D(FMath::Min(PCHud->vMouseStartPos.X, PCHud->vCurrentMousePos.X), FMath::Min(PCHud->vMouseStartPos.Y, PCHud->vCurrentMousePos.Y));
		endPoint = FVector2D(FMath::Max(PCHud->vMouseStartPos.X, PCHud->vCurrentMousePos.X), FMath::Max(PCHud->vMouseStartPos.Y, PCHud->vCurrentMousePos.Y));
		marqueeBox = FBox2D(startPoint, endPoint);

		for (TActorIterator<AUnitSelectionCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AUnitSelectionCharacter* USChar = *ActorItr;
			ProjectWorldLocationToScreen(USChar->GetActorLocation(), screenLocation);
			if (marqueeBox.IsInside(screenLocation)) {
				if (!USChar->GetIsSelected()) {
					ServerAddSelection(USChar);
				}
			}
			else {
				if (USChar->GetIsSelected() && !PCHud->bIsMultiSelecting) {
					ServerRemoveSelection(USChar);
				}
			}
		}
	}
	
	

}

void AUnitSelectionPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AUnitSelectionPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AUnitSelectionPlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("Select", IE_Pressed, this, &AUnitSelectionPlayerController::OnSelectionPressed);
	InputComponent->BindAction("Select", IE_Released, this, &AUnitSelectionPlayerController::OnSelectionReleased);
	InputComponent->BindAction("MultiSelect", IE_Pressed, this, &AUnitSelectionPlayerController::OnMultiSelectionPressed);
	InputComponent->BindAction("MultiSelect", IE_Released, this, &AUnitSelectionPlayerController::OnMultiSelectionReleased);
	InputComponent->BindAction("SetTeamRed", IE_Pressed, this, &AUnitSelectionPlayerController::SetTeamRed);
	InputComponent->BindAction("SetTeamBlue", IE_Pressed, this, &AUnitSelectionPlayerController::SetTeamBlue);


}

void AUnitSelectionPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AUnitSelectionPlayerController, selectedCharacters);
	DOREPLIFETIME(AUnitSelectionPlayerController, TeamName);
}

void AUnitSelectionPlayerController::StartMarqueeSelection() {
	ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(this->Player);

	if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport) {
		if (LocalPlayer->ViewportClient->GetMousePosition(mouseStartPosition) == false)
		{
			return;
		}
	}

	PCHud->vMouseStartPos = mouseStartPosition;
	PCHud->bIsSelecting = true;

	
}

void AUnitSelectionPlayerController::OnSelectionPressed() {

	StartMarqueeSelection();
	ServerEmptySelection();
	
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, false, Hit);

	AUnitSelectionCharacter* newActor = Cast<AUnitSelectionCharacter>(Hit.GetActor());
	if (newActor) {
		ServerEmptySelection();
		ServerAddSelection(newActor);
	}
	else {
		ServerEmptySelection();
	}

}

void AUnitSelectionPlayerController::ServerAddSelection_Implementation(AActor* actor) {
	AUnitSelectionCharacter* newActor = Cast<AUnitSelectionCharacter>(actor);
	if (newActor) {
		if (newActor->GetTeam() == TeamName) {
			//add newly selected actor to list as only actor selected and set it's selection bool to true
			selectedCharacters.Emplace(newActor);
			newActor->SetIsSelected(true);
			newActor->MCShowDecal();
		}
	}
}

bool AUnitSelectionPlayerController::ServerAddSelection_Validate(AActor* actor) {
	return true;
}

void AUnitSelectionPlayerController::ServerRemoveSelection_Implementation(AActor* actor) {
	AUnitSelectionCharacter* newActor = Cast<AUnitSelectionCharacter>(actor);
	if (newActor) {
		//remove and set actor as unselected
		selectedCharacters.Remove(newActor);
		newActor->SetIsSelected(false);
		newActor->MCShowDecal();
	}
}

bool AUnitSelectionPlayerController::ServerRemoveSelection_Validate(AActor* actor) {
	return true;
}

void AUnitSelectionPlayerController::ServerEmptySelection_Implementation() {

	//set all actors as unselected
	AUnitSelectionCharacter* unselectActor;
	for (auto& testActor : selectedCharacters) {
		unselectActor = Cast<AUnitSelectionCharacter>(testActor);
		if (unselectActor) {
			unselectActor->SetIsSelected(false);
			unselectActor->MCShowDecal();

		}
	}

	//empty the list of selected actors
	selectedCharacters.Empty();
}

bool AUnitSelectionPlayerController::ServerEmptySelection_Validate() {
	return true;
}



void AUnitSelectionPlayerController::OnSelectionReleased() {
	PCHud->bIsSelecting = false;
}

void AUnitSelectionPlayerController::OnMultiSelectionReleased() {
	PCHud->bIsMultiSelecting = false;
	PCHud->bIsSelecting = false;
}



void AUnitSelectionPlayerController::OnMultiSelectionPressed() {

	StartMarqueeSelection();
	PCHud->bIsMultiSelecting = true;

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, false, Hit);

	AUnitSelectionCharacter* newActor = Cast<AUnitSelectionCharacter>(Hit.GetActor());

	if (newActor) {
		ServerAddSelection(newActor);
	}

}



void AUnitSelectionPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	FHitResult Hit;

	if (selectedCharacters.Num() > 0) {
		
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			ServerOnSetDestinationPressed(Hit);
		}
	}
}

void AUnitSelectionPlayerController::ServerOnSetDestinationPressed_Implementation(FHitResult Hit) {
	//OnSetDestinationPressed();
	if (selectedCharacters.Num() > 0) {

		int positionCount = 1;
		
	//for (auto& testActor : selectedCharacters) {
		for (int i = 0; i < selectedCharacters.Num(); i++){
			AUSAIController* AIController = Cast<AUSAIController>(Cast<AUnitSelectionCharacter>(selectedCharacters[i])->GetController());
			if (AIController) {
				if (i == 0) {
					AIController->SetNewMoveDestination(Hit.ImpactPoint);
				}
				else if ((i % 2) == 0) {
					//DO A THING TO LINE UP THE EVENS
					/*
					hit.impactpoint + normalizedmovedir.rotatearoundZAxisBy90Deg*offset /////NEED TO MAKE OFFSET
					*/
					FVector moveLocation = Hit.ImpactPoint + FindMainMoveDirection(Hit).RotateAngleAxis(90, FVector(0, 0, 1))*lineFormationOffset*positionCount;
					AIController->SetNewMoveDestination(moveLocation);

					positionCount++;
				}
				else if ((i % 2) == 1) {
					//DO A THING TO LINE UP THE ODDS

					FVector moveLocation = Hit.ImpactPoint + FindMainMoveDirection(Hit).RotateAngleAxis(270, FVector(0, 0, 1))*lineFormationOffset*positionCount;
					AIController->SetNewMoveDestination(moveLocation);
				}
				else {
					//DO NOTHING
				}
			}
		}
	}
}

bool AUnitSelectionPlayerController::ServerOnSetDestinationPressed_Validate(FHitResult Hit) {
	return true;
}





void AUnitSelectionPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}



void AUnitSelectionPlayerController::SetCharsInMarquee(TArray<AUnitSelectionCharacter*> chars) {
	charsInMarquee = chars; 
}

TArray<AUnitSelectionCharacter*> AUnitSelectionPlayerController::GetCharsInMarquee() {
	return charsInMarquee; 
}

FVector AUnitSelectionPlayerController::FindMainMoveDirection(FHitResult Hit) {
	return (Hit.ImpactPoint - selectedCharacters[0]->GetActorLocation()).GetSafeNormal();
}

void AUnitSelectionPlayerController::SetTeamRed() {
	TeamName = "Red";

	if (Role < ROLE_Authority) {
		ServerSetTeamRed();
	}
}

void AUnitSelectionPlayerController::ServerSetTeamRed_Implementation() {
	TeamName = "Red";
	if (Role < ROLE_Authority) {

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TeamName.ToString());
	}

}

bool AUnitSelectionPlayerController::ServerSetTeamRed_Validate() {
	return true;
}

void AUnitSelectionPlayerController::SetTeamBlue() {
	TeamName = "Blue";

	if (Role < ROLE_Authority) {
		ServerSetTeamBlue();
	}
}

void AUnitSelectionPlayerController::ServerSetTeamBlue_Implementation() {
	TeamName = "Blue";
	if (Role < ROLE_Authority) {

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TeamName.ToString());
	}

}

bool AUnitSelectionPlayerController::ServerSetTeamBlue_Validate() {
	return true;
}