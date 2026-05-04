#pragma once

#include "CoreMinimal.h"

class APlayerCharacter;

UENUM()
enum class EStatePhase : uint8
{
    Enter,
    Active,
    Exit
};

class BaseState
{

public:

    virtual ~BaseState() = default;

    virtual void EnterState(APlayerCharacter& Player) = 0;
    virtual void ExitState(APlayerCharacter& Player) = 0;

	virtual void HandleInput(APlayerCharacter& Player, float DeltaTime) = 0;

	virtual void Tick(APlayerCharacter& Player, float DeltaTime) = 0;

	EStatePhase GetCurrentPhase() const { return CurrentPhase; }

protected:
	EStatePhase CurrentPhase = EStatePhase::Enter;
};
