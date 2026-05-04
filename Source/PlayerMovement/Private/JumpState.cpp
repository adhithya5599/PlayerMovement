

#include "JumpState.h"
#include "PlayerCharacter.h"
#include "StandState.h"
#include "WalkSprintState.h"
#include "GameFramework/CharacterMovementComponent.h"

void JumpState::EnterState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Enter;

	Player.ClearSlide();
	Player.PlaySoundAtLocation(Player.JumpSound, Player.GetActorLocation());
	Player.Jump();

	CurrentPhase = EStatePhase::Active;
}

void JumpState::ExitState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Exit;
	Player.StopCurrentSound();
}

void JumpState::HandleInput(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;
}

void JumpState::Tick(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		if (!MovementComponent->IsFalling())
		{
			Player.ClearSlide();
			if(Player.HasMovementInput())
			{
				Player.ChangeState(MakeUnique<WalkSprintState>(), TEXT("WalkSprintState"));
				return;
			}
			else
			{
				Player.ChangeState(MakeUnique<StandState>(), TEXT("StandState"));
				return;
			}
		}
	}
}