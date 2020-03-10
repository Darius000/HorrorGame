// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/BaseInteractable.h"
#include "InventoryItem.generated.h"

class UInventoryItem;
class AInventoryWorldItem;

UCLASS()
class UTextUnilities : public UObject
{
	GENERATED_BODY()

public:
	template<typename TEnum>
	static FORCEINLINE const FString GetEnumAsString(const TCHAR* EnumName, TEnum value)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, EnumName, true);
		if (!EnumPtr) return FString("Invalid!");

#if WITH_EDITOR
		return EnumPtr->GetDisplayNameTextByIndex((int64)value).ToString();
#else
		return EnumPtr->GetEnumName(value);
#endif
	}
};

#define EnumToString(EnumClassName, Value) UTextUnilities::GetEnumAsString<EnumClassName>(TEXT(#EnumClassName) , Value)
#define EnumToName(EnumClassName, Value) FName(*EnumToString(EnumClassName, Value))
#define EnumToText(EnumClassName, Value) FText::FromString(EnumToString(EnumClassName, Value))

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	None	UMETA(DisplayName = "None"),
	Weapon	UMETA(DisplayName = "Weapon"),
	Armor	UMETA(DisplayName = "Armor"),
	Cloth	UMETA(DisplayName = "Cloth"),
	Person  UMETA(DisplayName = "Person")
};

UENUM(BlueprintType)
enum class ESocketName : uint8
{
	None			UMETA(DisplayName = "None"),
	Head			UMETA(DisplayName = "Head"),
	Torso			UMETA(DisplayName = "Torso"),
	Hips			UMETA(DisplayName = "Hips"),
	ArmRight		UMETA(DisplayName = "ArmRight"),
	LegRight		UMETA(DisplayName = "LegRight"),
	ArmLeft			UMETA(DisplayName = "ArmLeft"),
	LegLeft			UMETA(DisplayName = "LegLeft"),
	FootLeft		UMETA(DisplayName = "FootLeft"),
	FootRight		UMETA(DisplayName = "FootRight"),
	Back			UMETA(DisplayName = "Back"),
	LowerBack		UMETA(DisplayName = "LowerBack"),
	UpperBack		UMETA(DisplayName = "UpperBack"),
	HandRight		UMETA(DisplayName = "HandRight"),
	HandLeft		UMETA(DisplayName = "HandLeft"),
	PrimaryRight	UMETA(DisplayName = "PrimaryRight"),
	PrimaryLeft		UMETA(DisplayName = "PrimaryLeft"),
	SecondaryRight	UMETA(DisplayName = "SecondaryRight"),
	SecondaryLeft	UMETA(DisplayName = "SecondaryLeft")
};


USTRUCT(BlueprintType)
struct FInventoryGridSize
{
	GENERATED_BODY()

	FInventoryGridSize()
	{
		Width = 0;
		Height = 0;
	}

	FInventoryGridSize(int32 width, int32 height)
	{
		Width = width;
		Height = height;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height;
};

USTRUCT(BlueprintType)
struct FInventoryItemData : public FTableRowBase
{
	GENERATED_BODY()

	FInventoryItemData()
	{
		Name = FText::FromString("No Name");
		Weight = 1.0;
		ItemType = EInventoryItemType::None;
		Description = FText::FromString("No Description");
		MaxStackSize = 1;
		bStackable = true;
		bConsumable = false;
		bEquippable = true;
		bDisplayable = true;
		bCanOnlyHaveOneInstance = false;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EInventoryItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bStackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bStackable"))
		int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FInventoryGridSize SpaceNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bConsumable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bEquippable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDisplayable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDroppable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanOnlyHaveOneInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEquippable"))
		ESocketName EquipSocketName;

	/*Which should the item attach to if not equipped*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bDisplayable"))
		ESocketName UnEquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;

	/* object class to generate for inventory, class includes a use and drop function*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "InventoryItemInterface"))
		TSubclassOf<UInventoryItem> InventoryItemClass;

	/*actor class that represents it in the world*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "InventoryWorldItemInterface"))
		TSubclassOf<AInventoryWorldItem> WorldItemClass;

	bool operator==(const FInventoryItemData& other) const
	{
		return (ID == other.ID);
	}
};


UCLASS(Blueprintable, BlueprintType, meta = (Category))
class CRLTSIM_API UAction : public UObject
{

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBeginExecuteSigniture, UObject*, Instigator);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExecuteSigniture, UObject*, Instigator);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndBeginExecuteSigniture, UObject*, Instigator);

	GENERATED_BODY()

public:	
	UAction();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void Execute(UObject* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnBeginExecute(UObject* Instigator);
	void OnBeginExecute_Implementation(UObject* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnExecute(UObject* Instigator);
	void OnExecute_Implementation(UObject* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void OnEndExecute(UObject* Instigator);
	void OnEndExecute_Implementation(UObject* Instigator);

	FBeginExecuteSigniture OnBeginExecution;
	FExecuteSigniture OnExecution;
	FEndBeginExecuteSigniture OnEndExecution;
};

UCLASS()
class CRLTSIM_API AInventoryWorldItem : public ABaseInteractable
{
	GENERATED_BODY()

	AInventoryWorldItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FName GetItemID() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetStackSize() const;

protected:
	virtual void OnInteractWith_Implementation(class AActor* actor) override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 StackSize;

};

UCLASS(BlueprintType, Blueprintable, editinlinenew)
class CRLTSIM_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInventoryItem();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
		TArray<TSubclassOf<UAction>> Abilities;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		FName GetItemID() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void Use(AActor* Instigator, AActor* OtherActor, UAction* ActionToExecute);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void Drop(AActor* WorldItemClass, const FTransform& Transform);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void OnUse(AActor* Instigator, AActor* OtherActor, UAction* ActionToExecute);
	void OnUse_Implementation(AActor* Instigator, AActor* OtherActor, UAction* ActionToExecute);

	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void OnDrop(AActor* WorldItemClass, const FTransform& Transform);
	void OnDrop_Implementation(AActor* WorldItemClass, const FTransform& Transform);
};
