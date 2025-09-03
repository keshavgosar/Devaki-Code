#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_OneHandWeaponEquipped UMETA(DisplayName = "OneHandWeaponEquipped"),
	ECS_TwoHandWeaponEquipped UMETA(DisplayName = "TwoHandWeaponEquipped")
};

