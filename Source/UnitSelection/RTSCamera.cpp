// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitSelection.h"
#include "UnitSelectionPlayerController.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "RTSCamera.h"


// Sets default values
ARTSCamera::ARTSCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Replication set to true for network replication
	bReplicates = true;

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Setup debug static mesh
	LocationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugMesh"));
	RootComponent = LocationMesh;

	//create camera boom and set camera boom options
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	//CameraBoom->bAbsoluteRotation = false; //rotate arm relative to static mesh
	CameraBoom->TargetArmLength = 3000.f;
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->bUsePawnControlRotation = false;

	// Create a camera
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);

	//moveComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Movement Component"));

	iMoveSpeed = 1000;
	iZoomSpeed = 50;
	iRotationSpeed = 700;
	bMiddleMouse = false;
	iPanSpeed = 1000;
	iEdgeSize = 50;
	fMaxZoom = 2000.f;
	fMinZoom = 50.f;
	iZoomLength = fMaxZoom;

	cameraPitch = -70.f;

}

// Called when the game starts or when spawned
void ARTSCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARTSCamera::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	/*if (bMiddleMouse) {
		FRotator newRot = GetActorRotation();
		newRot.Yaw += GetInputAxisValue("CameraYaw")*iRotationSpeed;
		MouseRotateCamera(newRot);
	}*/

	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, iZoomLength, DeltaTime, 5.f);
	cameraPitch = FMath::FInterpTo(cameraPitch, ZoomTilt(CameraBoom->TargetArmLength), DeltaTime, 5);
	CameraBoom->SetRelativeRotation(FRotator(cameraPitch, 0, 0));
	
	EdgePan(DeltaTime);
}

// Called to bind functionality to input
void ARTSCamera::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	check(InputComponent);

	//keyboard camera input
	InputComponent->BindAxis("MoveForward", this, &ARTSCamera::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARTSCamera::MoveRight);
	InputComponent->BindAxis("RotateCamera", this, &ARTSCamera::KeyboardRotateCamera);

	//mouse camera input
	InputComponent->BindAction("ScrollZoomIn", IE_Pressed, this, &ARTSCamera::ScrollCameraIn);
	InputComponent->BindAction("ScrollZoomOut", IE_Pressed, this, &ARTSCamera::ScrollCameraOut);
	InputComponent->BindAction("MiddleMouseRotateCamera", IE_Pressed, this, &ARTSCamera::OnMouseRotationClick);
	InputComponent->BindAction("MiddleMouseRotateCamera", IE_Released, this, &ARTSCamera::OnMouseRotationRelease);
	InputComponent->BindAxis("CameraYaw", this, &ARTSCamera::MouseRotateCamera);


}

void ARTSCamera::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ARTSCamera, bMiddleMouse);
}


//***********Move forward server functions
void ARTSCamera::MoveForward(float delta) {
	AddActorLocalOffset(FVector(iMoveSpeed*(GetWorld()->GetDeltaSeconds())*delta, 0, 0));

	if (Role < ROLE_Authority) {
		ServerMoveForward(delta);
	}
}

bool ARTSCamera::ServerMoveForward_Validate(float delta) {
	return true;
}

void ARTSCamera::ServerMoveForward_Implementation(float delta) {

	MoveForward(delta);
}


//***********Move right server functions
void ARTSCamera::MoveRight(float delta) {
	AddActorLocalOffset(FVector(0, iMoveSpeed*(GetWorld()->GetDeltaSeconds())*delta, 0));

	if (Role < ROLE_Authority) {
		ServerMoveRight(delta);
	}
}

bool ARTSCamera::ServerMoveRight_Validate(float delta) {
	return true;
}

void ARTSCamera::ServerMoveRight_Implementation(float delta) {
	
	MoveRight(delta);
}


//***********Rotate server functions
void ARTSCamera::KeyboardRotateCamera(float delta) {
	AddActorLocalRotation(FRotator(0, iRotationSpeed*(GetWorld()->GetDeltaSeconds())*delta/4, 0));

	if (Role < ROLE_Authority) {
		ServerKeyboardRotateCamera(delta);
	}
}

void ARTSCamera::ServerKeyboardRotateCamera_Implementation(float delta) {
	KeyboardRotateCamera(delta);
}

bool ARTSCamera::ServerKeyboardRotateCamera_Validate(float delta) {
	return true;
}



void ARTSCamera::ScrollCameraIn() {
	iZoomLength -= iZoomSpeed;

	if (iZoomLength <= fMinZoom) {
		iZoomLength = fMinZoom;
	}

	GetCameraBoom()->SetRelativeRotation(FRotator(ZoomTilt(GetCameraBoom()->TargetArmLength), 0, 0));
}

void ARTSCamera::ScrollCameraOut() {
	iZoomLength += iZoomSpeed;

	if (iZoomLength > fMaxZoom) {
		iZoomLength = fMaxZoom;
	}

	GetCameraBoom()->SetRelativeRotation(FRotator(ZoomTilt(GetCameraBoom()->TargetArmLength), 0, 0));
}

float ARTSCamera::ZoomTilt(float length) {
	float newPitch = -70;

	if (length <= 700) {
		newPitch = length / -10;
	}

	return newPitch;
}


//**********Network functions for mouse rotation
void ARTSCamera::MouseRotateCamera(float delta) {
	if (bMiddleMouse) {
		AddActorWorldRotation(FRotator(0, iRotationSpeed*(GetWorld()->GetDeltaSeconds())*delta, 0));
	}

	if (Role < ROLE_Authority) {
		ServerMouseRotateCamera(delta);
	}
}

void ARTSCamera::ServerMouseRotateCamera_Implementation(float delta) {
	MouseRotateCamera(delta);
}

bool ARTSCamera::ServerMouseRotateCamera_Validate(float delta) {
	return true;
}




void ARTSCamera::OnMouseRotationClick() {
	bMiddleMouse = true;

	AUnitSelectionPlayerController* myController = Cast<AUnitSelectionPlayerController>(GetController());
	myController->bShowMouseCursor = false;

	if (Role < ROLE_Authority) {
		ServerOnMouseRotationClick();
	}
}

void ARTSCamera::ServerOnMouseRotationClick_Implementation() {
	OnMouseRotationClick();
}

bool ARTSCamera::ServerOnMouseRotationClick_Validate() {
	return true;
}




void ARTSCamera::OnMouseRotationRelease() {
	bMiddleMouse = false;

	AUnitSelectionPlayerController* myController = Cast<AUnitSelectionPlayerController>(GetController());
	myController->bShowMouseCursor = true;

	if (Role < ROLE_Authority) {
		ServerOnMouseRotationRelease();
	}
}

void ARTSCamera::ServerOnMouseRotationRelease_Implementation() {
	OnMouseRotationRelease();
}

bool ARTSCamera::ServerOnMouseRotationRelease_Validate() {
	return true;
}



void ARTSCamera::EdgePan(float delta) {

	AUnitSelectionPlayerController* myController = Cast<AUnitSelectionPlayerController>(GetController());

	if (myController) {

		ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(myController->Player);

		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport) {
			if (LocalPlayer->ViewportClient->GetMousePosition(mouseScreenPosition) == false)
			{
				return;
			}
			FVector2D screenSize = LocalPlayer->ViewportClient->Viewport->GetSizeXY();

			if (!bMiddleMouse) {
				if (mouseScreenPosition.X <= iEdgeSize) {
					//AddActorLocalOffset(FVector(0, -1 * iPanSpeed*delta, 0));
					MoveRight(-1);//-1 for "axis input" left
				}
				if (mouseScreenPosition.X >= screenSize.X - iEdgeSize) {
					//AddActorLocalOffset(FVector(0, iPanSpeed*delta, 0));
					MoveRight(1); //1 for "axis input" right
				}
				if (mouseScreenPosition.Y >= screenSize.Y - iEdgeSize) {
					//AddActorLocalOffset(FVector(-1 * iPanSpeed*delta, 0, 0));
					MoveForward(-1); // -1 for "axis input" backwards
				}
				if (mouseScreenPosition.Y <= iEdgeSize) {
					//AddActorLocalOffset(FVector(iPanSpeed*delta, 0, 0));
					MoveForward(1);
				}
			}
		}
	}
}