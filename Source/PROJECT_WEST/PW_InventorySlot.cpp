// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_InventorySlot.h"

void UPW_InventorySlot::EmptyCurrentSlot()
{
	_currentItems.RemoveAt(_currentIndex);
}

bool UPW_InventorySlot::IsSlotAvailable( )
{
	bool isSlotAvailable = false;
	for (const APW_ItemObject* inventorySlot : _currentItems)
		if (inventorySlot == nullptr)
			isSlotAvailable = true;

	return isSlotAvailable;
}
