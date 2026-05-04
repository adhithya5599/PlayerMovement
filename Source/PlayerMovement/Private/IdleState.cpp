

#include "IdleState.h"
#include "JumpState.h"
#include "ProneState.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void IdleState::EnterState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Enter;
	//Set the player material to idle material
	Player.SetDebugMaterial(Player.IdleMaterial);

	//Play idle sound
	Player.PlaySoundAtLocation(Player.IdleSound, Player.GetActorLocation());

	CurrentPhase = EStatePhase::Active;
}

void IdleState::ExitState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Exit;
	//Reset the player material
	Player.SetDebugMaterial(nullptr);
	Player.StopCurrentSound();
}

void IdleState::HandleInput(APlayerCharacter& Player, float DeltaTime)
{
	if (Player.IsJumpUsed())
	{
		const bool bIsOnGround = !Player.GetCharacterMovement()->IsFalling();
		if (bIsOnGround)
		{
			Player.ChangeState(MakeUnique<JumpState>(), TEXT("JumpState"));
			return;
		}
	}
	if (Player.IsProneUsed())
	{
		const bool bIsOnGround = !Player.GetCharacterMovement()->IsFalling();
		if (bIsOnGround)
		{
			Player.ChangeState(MakeUnique<ProneState>(), TEXT("ProneState"));
			return;
		}
	}
}

void IdleState::Tick(APlayerCharacter& Player, float DeltaTime)
{
	if(CurrentPhase == EStatePhase::Active)
	{
	}
}
