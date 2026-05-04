

#include "StandState.h"
#include "WalkSprintState.h"
#include "PlayerCharacter.h"
#include "ProneState.h"
#include "JumpState.h"
#include "GameFramework/CharacterMovementComponent.h"

void StandState::EnterState(APlayerCharacter& Player)
{

	CurrentPhase = EStatePhase::Enter;
	//Set the player material for standing state
	Player.SetDebugMaterial(Player.StandMaterial);
	
	CurrentPhase = EStatePhase::Active;
}

void StandState::ExitState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Exit;
}

void StandState::HandleInput(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;

	if (Player.HasMovementInput())
	{
		Player.ChangeState(MakeUnique<WalkSprintState>(), TEXT("WalkSprintState"));
		return;
	}

	if(Player.IsProneUsed())
	{
		const float CurrentSpeed = Player.GetVelocity().Size2D();
		const bool bIsOnGround = !Player.GetCharacterMovement()->IsFalling();

		if (bIsOnGround && CurrentSpeed < 10.f)
		{
			Player.ChangeState(MakeUnique<ProneState>(), TEXT("ProneState"));
			return;
		}
	}

	if(Player.IsJumpUsed())
	{
		const bool bIsOnGround = !Player.GetCharacterMovement()->IsFalling();
		if (bIsOnGround)
		{
			Player.ChangeState(MakeUnique<JumpState>(), TEXT("JumpState"));
			return;
		}
	}
}

void StandState::Tick(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;
}