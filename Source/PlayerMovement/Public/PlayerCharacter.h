// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <memory>
#include "InputAction.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class BaseState;
class USoundBase;
class UAudioComponent;

UCLASS()
class PLAYERMOVEMENT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Material")
	UMaterialInterface* IdleMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Material")
	UMaterialInterface* StandMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Material")
	UMaterialInterface* WalkMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Material")
	UMaterialInterface* SlideMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Material")
	UMaterialInterface* ProneMaterial;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* CameraBoom;

private:

	TUniquePtr<BaseState> CurrentState;
	FVector2D MovementInput = FVector2D::ZeroVector;
	FName CurrentStateName = NAME_None;
	
	FTimerHandle IdleTimerHandle;

	UPROPERTY(EditAnywhere, Category = "State")
	float IdleTransitionTime = 4.f;


	void StartIdleTimer();
	void ResetIdleTimer();
	void OnIdleTimerExpired();

public:
	
	void ChangeState(TUniquePtr<BaseState> NewState, FName NewStateName);
	void SetDebugMaterial(UMaterialInterface* NewMaterial);

	void OnMoveInput(const FVector2D& Input);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* QuitAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* PostureAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction = nullptr;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Aim(const FInputActionValue& Value);

	UFUNCTION()
	void Quit();

	UFUNCTION()
	void SprintStarted(const FInputActionValue& Value);

	UFUNCTION()
	void SprintStopped(const FInputActionValue& Value);

	UFUNCTION()
	void PostureStarted(const FInputActionValue& Value);

	UFUNCTION()
	void PostureCompleted(const FInputActionValue& Value);

	UFUNCTION()
	void JumpStarted(const FInputActionValue& Value);

public:

	FVector2D GetMovementInput() const { return MovementInput; }
	bool HasMovementInput() const { return !MovementInput.IsNearlyZero(); }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Speed")
	float WalkSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Speed")
	float SprintSpeed = 600.f;

	bool bIsSprintHeld = false;

public:
	
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
	FORCEINLINE bool IsSprinting() const { return bIsSprintHeld; }

protected:

	bool bIsPostureHeld = false;
	bool bIsPosturePressed = false;

	UPROPERTY(EditAnywhere, Category = "Movement | Prone")
	float ProneHoldDuration = 0.35f;

	FTimerHandle PostureTimerHandle;

	bool bWantsToProne = false;
	bool bWantsToSlide = false;

	void OnPostureHoldFinished();

public:

	bool IsProneUsed();
	bool IsSlideUsed();

	FORCEINLINE void ClearSlide() { bWantsToSlide = false; }
	FORCEINLINE bool IsPostureHeld() const { return bIsPostureHeld; }

protected:

	bool bWantsToJump = false;

public:

	bool IsJumpUsed();
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetCurrentStateName(FName NewStateName) { CurrentStateName = NewStateName; }

	UFUNCTION(BlueprintCallable, Category = "State")
	FName GetCurrentStateName() const { return CurrentStateName; }

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* FootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* IdleSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* SlideSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* ProneSound;

	UAudioComponent* AudioComponent;

	void PlaySoundAtLocation(USoundBase* Sound, const FVector& Location);
	void StopCurrentSound();
};
