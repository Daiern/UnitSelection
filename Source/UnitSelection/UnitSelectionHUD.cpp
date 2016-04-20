// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitSelection.h"
#include "UnitSelectionCharacter.h"
#include "UnitSelectionPlayerController.h"
#include "UnitSelectionHUD.h"

AUnitSelectionHUD::AUnitSelectionHUD() {
	bIsSelecting = false;

	vMouseStartPos = FVector2D(0.f, 0.f);
	vCurrentMousePos = FVector2D(0.f, 0.f);
}

void AUnitSelectionHUD::OnMouseClick(){ 

}
