

#include "ProneState.h"
#include "PlayerCharacter.h"
#include "StandState.h"
#include "GameFramework/CharacterMovementComponent.h"

void ProneState::EnterState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Enter;
	//Set player material to prone material
	Player.SetDebugMaterial(Player.ProneMaterial);
	Player.Crouch();

	//Play prone sound
	Player.PlaySoundAtLocation(Player.ProneSound, Player.GetActorLocation());

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Player.GetWalkSpeed() * 0.2f; // Reduce speed when prone
	}

	CurrentPhase = EStatePhase::Active;
}

void ProneState::ExitState(APlayerCharacter& Player)
{
	CurrentPhase = EStatePhase::Exit;
	Player.UnCrouch();

	if (UCharacterMovementComponent* MovementComponent = Player.GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Player.GetWalkSpeed(); // Reset speed when exiting prone
	}
	Player.StopCurrentSound();
}

void ProneState::HandleInput(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;

	if (!Player.IsPostureHeld())
	{
		Player.ChangeState(MakeUnique<StandState>(), TEXT("StandState"));
		return;
	}
}

void ProneState::Tick(APlayerCharacter& Player, float DeltaTime)
{
	if (CurrentPhase != EStatePhase::Active) return;
}