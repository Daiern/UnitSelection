// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "RTSCamera.generated.h"

UCLASS()
class UNITSELECTION_API ARTSCamera : public APawn
{
	GENERATED_BODY()

	/** generic mesh for debug purposes*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LocationMesh;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//class UCharacterMovementComponent* moveComp;

	// camera keyboard control
	// set RPC functions for calling on server for replication
	void MoveForward(float delta);
	//
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerMoveForward(float delta);
	void ServerMoveForward_Implementation(float delta);
	bool ServerMoveForward_Validate(float delta);
	//
	//
	void MoveRight(float delta);
	//
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerMoveRight(float delta);
	void ServerMoveRight_Implementation(float delta);
	bool ServerMoveRight_Validate(float delta);
	//
	//
	void KeyboardRotateCamera(float delta);
	//
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerKeyboardRotateCamera(float delta);
	void ServerKeyboardRotateCamera_Implementation(float delta);
	bool ServerKeyboardRotateCamera_Validate(float delta);

	//camera mouse control
	void ScrollCameraIn();
	void ScrollCameraOut();
	
	float ZoomTilt(float length);
	void EdgePan(float delta);

	//RPC functions for network mouse controls
	void MouseRotateCamera(float delta);
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerMouseRotateCamera(float delta);
	void ServerMouseRotateCamera_Implementation(float delta);
	bool ServerMouseRotateCamera_Validate(float delta);

	void OnMouseRotationClick();
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerOnMouseRotationClick();
	void ServerOnMouseRotationClick_Implementation();
	bool ServerOnMouseRotationClick_Validate();

	void OnMouseRotationRelease();
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerOnMouseRotationRelease();
	void ServerOnMouseRotationRelease_Implementation();
	bool ServerOnMouseRotationRelease_Validate();


	uint32 iMoveSpeed; //camera speed

	UPROPERTY(Replicated)
	bool bMiddleMouse; //for middle mouse camera rotation

	uint32 iZoomSpeed;	//scroll in and out zoom speed
	uint32 iRotationSpeed; // speed of camera rotation
	uint32 iZoomLength; //interp length for smooth zoom
	FVector2D mouseScreenPosition; //position of mouse in screen space for edge pan
	int32 iPanSpeed; //speed to pan the camera for edge pan
	uint32 iEdgeSize; //distance from edge to start panning
	float fMaxZoom;
	float fMinZoom;

	float cameraPitch;

public:
	// Sets default values for this pawn's properties
	ARTSCamera();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
};
