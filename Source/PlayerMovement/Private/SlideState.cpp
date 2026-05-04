

#include "SlideState.h"
#include "PlayerCharacter.h"
#include "WalkSprintState.h"
#include "GameFramework/CharacterMovementComponent.h"

void SlideState::EnterState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Enter;
	//Set player material for slide state
	Player.SetDebugMaterial(Player.SlideMaterial);

	Player.PlaySoundAtLocation(Player.SlideSound, Player.GetActorLocation());

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		MovementComponent->BrakingFrictionFactor = 0.f;
		MovementComponent->GroundFriction = 0.5f;
	}

	CurrentPhase = EStatePhase::Active;
}

void SlideState::ExitState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Exit;

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		MovementComponent->BrakingFrictionFactor = 2.f;
		MovementComponent->GroundFriction = 8.f;
	}
	Player.StopCurrentSound();
}

void SlideState::HandleInput(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;
}

void SlideState::Tick(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;

	RemainingSlideTime -= DeltaTime;

	if (RemainingSlideTime <= 0.f)
	{
		Player.ChangeState(MakeUnique<WalkSprintState>(), TEXT("WalkSprintState"));
		return;
	}
}