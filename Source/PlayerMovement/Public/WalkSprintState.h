#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"

class APlayerCharacter;

class WalkSprintState : public BaseState
{
public:
	
	virtual void EnterState(APlayerCharacter& Player) override;
	virtual void ExitState(APlayerCharacter& Player) override;

	virtual void HandleInput(APlayerCharacter& Player, float DeltaTime) override;

	virtual void Tick(APlayerCharacter& Player, float DeltaTime) override;
};