#pragma once

#include "UnrealEd.h"
#include "MovesetFactory.generated.h"

class UMovesetInputSchema;

UCLASS()
class UMovesetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};