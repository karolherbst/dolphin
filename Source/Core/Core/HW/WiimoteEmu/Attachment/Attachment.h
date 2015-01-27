// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include "Core/HW/WiimoteEmu/WiimoteEmu.h"
#include "InputCommon/ControllerEmu.h"

namespace WiimoteEmu
{

class Attachment : public ControllerEmu
{
public:
	Attachment(const char* const _name, WiimoteEmu::ExtensionReg& _reg);

	virtual void GetState(u8* const data) {}
	void Reset();
	std::string GetName() const override;

	const char* const         name;
	WiimoteEmu::ExtensionReg& reg;

	u8 id[6];
	u8 calibration[0x10];

protected:
	// TODO: Make constexpr when VS supports it.
	//
	// Default radius for attachment analog sticks.
	static const ControlState DEFAULT_ATTACHMENT_STICK_RADIUS;
};

class NoneAttachment : public Attachment
{
public:
	NoneAttachment(WiimoteEmu::ExtensionReg& _reg);
};

}
