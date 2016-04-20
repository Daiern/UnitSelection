// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnitSelection.h"
#include "UnitSelectionPlayerController.h"
#include "UnitSelectionCharacter.h"
#include "USAIController.h"
#include "UnrealNetwork.h"
#include "Engine.h"
#include "UnitSelectionHUD.h"
#include "AI/Navigation/NavigationSystem.h"

AUnitSelectionPlayerController::AUnitSelectionPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	mouseStartPosition = FVector2D(0.f, 0.f);
	currentMousePosition = FVector2D(0.f, 0.f);
}

void AUnitSelectionPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (Cast<AUnitSelectionHUD>(GetHUD())->bIsSelecting == true) {

		ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(this->Player);

		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport) {
			if (LocalPlayer->ViewportClient->GetMousePosition(currentMousePosition) == false)
			{
				return;
			}
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Marquee Selecting"));
			Cast<AUnitSelectionHUD>(GetHUD())->vCurrentMousePos = currentMousePosition;
			if ((currentMousePosition - mouseStartPosition).Size() > 5) {
				CheckUnitUnderMarquee();
			}
		}
		
	}

}

void AUnitSelectionPlayerController::CheckUnitUnderMarquee() {
	FVector2D startPoint, endPoint, screenLocation;
	FBox2D marqueeBox;

	AUnitSelectionHUD* hud = Cast<AUnitSelectionHUD>(GetHUD());
	
	if (hud) {
		startPoint = FVector2D(FMath::Min(hud->vMouseStartPos.X, hud->vCurrentMousePos.X), FMath::Min(hud->vMouseStartPos.Y, hud->vCurrentMousePos.Y));
		endPoint = FVector2D(FMath::Max(hud->vMouseStartPos.X, hud->vCurrentMousePos.X), FMath::Max(hud->vMouseStartPos.Y, hud->vCurrentMousePos.Y));
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
				if (USChar->GetIsSelected() && !Cast<AUnitSelectionHUD>(GetHUD())->bIsMultiSelecting) {
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


}

void AUnitSelectionPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AUnitSelectionPlayerController, selectedCharacters);
}

void AUnitSelectionPlayerController::StartMarqueeSelection() {
	ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(this->Player);

	if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport) {
		if (LocalPlayer->ViewportClient->GetMousePosition(mouseStartPosition) == false)
		{
			return;
		}
	}

	Cast<AUnitSelectionHUD>(GetHUD())->vMouseStartPos = mouseStartPosition;
	Cast<AUnitSelectionHUD>(GetHUD())->bIsSelecting = true;

	
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
		//add newly selected actor to list as only actor selected and set it's selection bool to true
		selectedCharacters.Emplace(newActor);
		newActor->SetIsSelected(true);
		newActor->MCShowDecal();
	}
}

bool AUnitSelectionPlayerController::ServerAddSelection_Validate(AActor* actor) {
	return true;
}

void AUnitSelectionPlayerController::ServerRemoveSelection_Implementation(AActor* actor) {
	AUnitSelectionCharacter* newActor = Cast<AUnitSelectionCharacter>(actor);
	if (newActor) {
		//add newly selected actor to list as only actor selected and set it's selection bool to true
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
	Cast<AUnitSelectionHUD>(GetHUD())->bIsSelecting = false;
}

void AUnitSelectionPlayerController::OnMultiSelectionReleased() {
	Cast<AUnitSelectionHUD>(GetHUD())->bIsMultiSelecting = false;
	Cast<AUnitSelectionHUD>(GetHUD())->bIsSelecting = false;
}



void AUnitSelectionPlayerController::OnMultiSelectionPressed() {

	StartMarqueeSelection();
	Cast<AUnitSelectionHUD>(GetHUD())->bIsMultiSelecting = true;

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
			AUnitSelectionCharacter* movingActor;
			for (auto& testActor : selectedCharacters) {
				movingActor = Cast<AUnitSelectionCharacter>(testActor);
				if (movingActor) {
					AUSAIController* AIController = Cast<AUSAIController>(movingActor->GetController());
					if (AIController) {
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NewMoveDest"));
						AIController->SetNewMoveDestination(Hit.ImpactPoint);
					}
				}
			}
		}
	}

	if (Role < ROLE_Authority) {
		ServerOnSetDestinationPressed(Hit);
	}
}

void AUnitSelectionPlayerController::ServerOnSetDestinationPressed_Implementation(FHitResult Hit) {
	//OnSetDestinationPressed();
	if (selectedCharacters.Num() > 0) {
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, selectedCharacters[0]->GetController()->GetName());
		for (auto& testActor : selectedCharacters) {
			AUSAIController* AIController = Cast<AUSAIController>(Cast<AUnitSelectionCharacter>(testActor)->GetController());
			if (AIController) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client Move"));
				AIController->SetNewMoveDestination(Hit.ImpactPoint);
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