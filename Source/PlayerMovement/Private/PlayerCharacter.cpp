// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "IdleState.h"
#include "StandState.h"
#include "WalkSprintState.h"
#include "ProneState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(40.f);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ChangeState(MakeUnique<StandState>(), TEXT("StandState"));

	StartIdleTimer();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		CurrentState->HandleInput(*this, DeltaTime);
		CurrentState->Tick(*this, DeltaTime);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	if (MoveAction)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::Move);
	}

	if (LookAction)
	{
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Aim);
		EIC->BindAction(LookAction, ETriggerEvent::Completed, this, &APlayerCharacter::Aim);
	}

	if (SprintAction)
	{
		EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintStarted);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintStopped);
	}

	if (PostureAction)
	{
		EIC->BindAction(PostureAction, ETriggerEvent::Started, this, &APlayerCharacter::PostureStarted);
		EIC->BindAction(PostureAction, ETriggerEvent::Completed, this, &APlayerCharacter::PostureCompleted);
	}

	if (QuitAction)
	{
		EIC->BindAction(QuitAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Quit);
	}

	if(JumpAction)
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);
	}
}

void APlayerCharacter::SetDebugMaterial(UMaterialInterface* NewMaterial)
{
	//Set player material to the new material
	if (!NewMaterial) return;
	if (!GetMesh()) return;

	GetMesh()->SetMaterial(0, NewMaterial);
}

void APlayerCharacter::StartIdleTimer()
{
	GetWorldTimerManager().SetTimer(
		IdleTimerHandle,
		this,
		&APlayerCharacter::OnIdleTimerExpired,
		IdleTransitionTime,
		false
	);
}

void APlayerCharacter::ResetIdleTimer()
{
	GetWorldTimerManager().ClearTimer(IdleTimerHandle);
	StartIdleTimer();
}

void APlayerCharacter::OnIdleTimerExpired()
{
	if(CurrentStateName != TEXT("IdleState"))
	{
		ChangeState(MakeUnique<IdleState>(), TEXT("IdleState"));
	}
}

void APlayerCharacter::ChangeState(TUniquePtr<BaseState> NewState, FName NewStateName)
{
	if (!NewState) return;
	
	if (CurrentState)
	{
		CurrentState->ExitState(*this);
	}

	CurrentStateName = NewStateName;

	CurrentState = MoveTemp(NewState);
	CurrentState->EnterState(*this);
}

void APlayerCharacter::OnMoveInput(const FVector2D& Input)
{
	MovementInput = Input;

	if (CurrentStateName == TEXT("SlideState") ||
		CurrentStateName == TEXT("ProneState") ||
		CurrentStateName == TEXT("JumpState"))
	{
		return;
	}

	if(!MovementInput.IsNearlyZero())
	{
		ResetIdleTimer();

		if (CurrentStateName == TEXT("IdleState") || CurrentStateName == TEXT("StandState"))
		{
			ChangeState(MakeUnique<WalkSprintState>(), TEXT("WalkSprintState"));
		}
	}
	else
	{
		if (CurrentStateName == TEXT("WalkSprintState"))
		{
			ChangeState(MakeUnique<StandState>(), TEXT("StandState"));
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	OnMoveInput(Input);

	if (!Controller) return;

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, Input.Y);
	AddMovementInput(RightDirection, Input.X);
}

void APlayerCharacter::Aim(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();

	ResetIdleTimer();
	if(CurrentStateName == TEXT("IdleState"))
	{
		ChangeState(MakeUnique<StandState>(), TEXT("StandState"));
	}

	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

void APlayerCharacter::Quit()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	PlayerController->ConsoleCommand(TEXT("quit"));
}

void APlayerCharacter::SprintStarted(const FInputActionValue& Value)
{
	bIsSprintHeld = true;
	ResetIdleTimer();
}

void APlayerCharacter::SprintStopped(const FInputActionValue& Value)
{
	bIsSprintHeld = false;
	ResetIdleTimer();
}

void APlayerCharacter::PostureStarted(const FInputActionValue& Value)
{
	if(CurrentStateName == TEXT("JumpState"))
	{
		return;
	}
	
	bIsPostureHeld = true;
	ResetIdleTimer();

	bWantsToProne = false;
	bWantsToSlide = false;

	GetWorldTimerManager().ClearTimer(PostureTimerHandle);

	GetWorldTimerManager().SetTimer(
		PostureTimerHandle,
		this,
		&APlayerCharacter::OnPostureHoldFinished,
		ProneHoldDuration,
		false
	);
}

void APlayerCharacter::PostureCompleted(const FInputActionValue& Value)
{
	if (CurrentStateName == TEXT("JumpState"))
	{
		return;
	}

	ResetIdleTimer();

	if (GetWorldTimerManager().IsTimerActive(PostureTimerHandle))
	{
		bWantsToSlide = true;
	}

	bIsPostureHeld = false;
	GetWorldTimerManager().ClearTimer(PostureTimerHandle);
}

void APlayerCharacter::JumpStarted(const FInputActionValue& Value)
{
	if(CurrentStateName == TEXT("ProneState") || CurrentStateName == TEXT("SlideState"))
	{
		return;
	}

	bWantsToJump = true;
	ResetIdleTimer();
}

void APlayerCharacter::OnPostureHoldFinished()
{
	bWantsToProne = true;
	bWantsToSlide = false;
}

bool APlayerCharacter::IsProneUsed()
{
	if (!bWantsToProne) return false;
	bWantsToProne = false;
	return true;
}

bool APlayerCharacter::IsSlideUsed()
{
	if(CurrentStateName == TEXT("JumpState"))
	{
		bWantsToSlide = false;
		return false;
	}

	if (!bWantsToSlide) return false;
	bWantsToSlide = false;
	return true;
}

bool APlayerCharacter::IsJumpUsed()
{
	if(CurrentStateName == TEXT("ProneState") || CurrentStateName == TEXT("SlideState"))
	{
		bWantsToJump = false;
		return false;
	}

	if (!bWantsToJump) return false;
	bWantsToJump = false;
	return true;
}

void APlayerCharacter::PlaySoundAtLocation(USoundBase* Sound, const FVector& Location)
{
	if (!Sound) return;

	AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, Sound, Location);
}

void APlayerCharacter::StopCurrentSound()
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent = nullptr;
	}
}
