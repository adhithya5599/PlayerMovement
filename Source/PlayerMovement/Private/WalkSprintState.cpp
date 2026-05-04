

#include "WalkSprintState.h"
#include "StandState.h"
#include "PlayerCharacter.h"
#include "SlideState.h"
#include "JumpState.h"
#include "GameFramework/CharacterMovementComponent.h"

void WalkSprintState::EnterState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Enter;
	//Set player material to walk material
	Player.SetDebugMaterial(Player.WalkMaterial);

	//Play walk sound
	Player.PlaySoundAtLocation(Player.FootstepSound, Player.GetActorLocation());

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Player.GetWalkSpeed();
	}

	CurrentPhase = EStatePhase::Active;
}

void WalkSprintState::ExitState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Exit;
	Player.StopCurrentSound();
}

void WalkSprintState::HandleInput(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;

	if (!Player.HasMovementInput())
	{
		Player.ClearSlide();

		Player.ChangeState(MakeUnique<StandState>(), TEXT("StandState"));
		return;
	}

	UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement();
	if (!MovementComponent) return;

	const bool bIsOnGround = !MovementComponent->IsFalling();
	if (Player.IsJumpUsed())
	{
		if (bIsOnGround)
		{
			Player.ChangeState(MakeUnique<JumpState>(), TEXT("JumpState"));
			return;
		}
	}

	if (Player.IsSprinting() && Player.IsSlideUsed() && bIsOnGround)
	{
		const float CurrentSpeed = Player.GetVelocity().Size2D();
		if (CurrentSpeed > Player.GetSprintSpeed() * 0.8f) // Check if the player is moving fast enough to slide
		{
			Player.ChangeState(MakeUnique<SlideState>(), TEXT("SlideState"));
			return;
		}
	}

	if (!Player.IsSprinting())
	{
		Player.ClearSlide();
	}
}

void WalkSprintState::Tick(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		const float TargetSpeed = Player.IsSprinting() ? Player.GetSprintSpeed() : Player.GetWalkSpeed();

		if (!FMath::IsNearlyEqual(MovementComponent->MaxWalkSpeed, TargetSpeed, 0.1f))
		{
			MovementComponent->MaxWalkSpeed = TargetSpeed;
		}
	}
}