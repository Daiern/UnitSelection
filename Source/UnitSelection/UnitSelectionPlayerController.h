// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "UnitSelectionCharacter.h"
#include "UnitSelectionPlayerController.generated.h"

UCLASS()
class AUnitSelectionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUnitSelectionPlayerController();

	void SetCharsInMarquee(TArray<AUnitSelectionCharacter*> chars);
	TArray<AUnitSelectionCharacter*> GetCharsInMarquee();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	//array of selected units
	UPROPERTY(Replicated)
	TArray<ACharacter*> selectedCharacters;
	class AUnitSelectionCharacter* clickedCharacter;
	TArray<AUnitSelectionCharacter*> charsInMarquee;

	FVector2D mouseStartPosition;
	FVector2D currentMousePosition;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Add new pawn to selectedPawns on left-click */
	void OnSelectionPressed();
	void OnSelectionReleased();
	void OnMultiSelectionPressed();
	

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAddSelection(AActor* actor);
	void ServerAddSelection_Implementation(AActor* actor);
	bool ServerAddSelection_Validate(AActor* actor);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRemoveSelection(AActor* actor);
	void ServerRemoveSelection_Implementation(AActor* actor);
	bool ServerRemoveSelection_Validate(AActor* actor);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEmptySelection();
	void ServerEmptySelection_Implementation();
	bool ServerEmptySelection_Validate();

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerOnSetDestinationPressed(FHitResult Hit);
	void ServerOnSetDestinationPressed_Implementation(FHitResult Hit);
	bool ServerOnSetDestinationPressed_Validate(FHitResult Hit);



	void StartMarqueeSelection();
	void OnSetDestinationReleased();
	void OnMultiSelectionReleased();
	void CheckUnitUnderMarquee();

};

