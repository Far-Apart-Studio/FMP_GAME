// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponObject.h"
#include "PW_Character.h"

void APW_WeaponObject::ApplyBindingActions(APW_Character* characterOwner)
{
	characterOwner->OnReloadButtonPressed.AddDynamic(this, &APW_WeaponObject::Reload);
	characterOwner->OnShootButtonPressed.AddDynamic(this, &APW_WeaponObject::Fire);
}

void APW_WeaponObject::RemoveBindingActions(APW_Character* characterOwner)
{
	characterOwner->OnReloadButtonPressed.RemoveDynamic(this, &APW_WeaponObject::Reload);
	characterOwner->OnShootButtonPressed.RemoveDynamic(this, &APW_WeaponObject::Fire);
}

void APW_WeaponObject::Reload()
{
	DEBUG_STRING("Reloading");
}

void APW_WeaponObject::Fire()
{
	DEBUG_STRING("Firing");
}

void APW_WeaponObject::Aim()
{
	
}
