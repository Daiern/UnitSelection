// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "UnitSelectionHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNITSELECTION_API AUnitSelectionHUD : public AHUD
{
	GENERATED_BODY()

	AUnitSelectionHUD();

public:
	UFUNCTION(BlueprintCallable, Category = Marquee)
	void OnMouseClick();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Marquee)
	bool bIsSelecting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Marquee)
	bool bIsMultiSelecting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Marquee)
	FVector2D vMouseStartPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Marquee)
	FVector2D vCurrentMousePos;
};
