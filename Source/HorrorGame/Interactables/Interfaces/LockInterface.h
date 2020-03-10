// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockInterface.generated.h"

UENUM(BlueprintType)
enum class ELockStatus : uint8
{
	UNLOCKED,
	LOCKED
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULockInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRLTSIM_API ILockInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lock")
		void OnLocked();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lock")
		void OnUnlocked();
};
