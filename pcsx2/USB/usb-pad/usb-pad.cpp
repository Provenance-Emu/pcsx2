/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2020  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PrecompiledHeader.h"
#include "usb-pad.h"
#include "USB/qemu-usb/USBinternal.h"
#include "USB/USB.h"
#include "StateWrapper.h"

#ifdef SDL_BUILD
#include "USB/usb-pad/usb-pad-sdl-ff.h"
#endif

namespace usb_pad
{
	static const USBDescStrings df_desc_strings = {
		"",
		"Logitech Driving Force",
		"",
		"Logitech",
	};

	static const USBDescStrings dfp_desc_strings = {
		"",
		"Logitech Driving Force Pro",
		"",
		"Logitech",
	};

	static const USBDescStrings gtf_desc_strings = {
		"",
		"Logitech", //actual index @ 0x04
		"Logitech GT Force" //actual index @ 0x20
	};

	static const USBDescStrings rb1_desc_strings = {
		"1234567890AB",
		"Licensed by Sony Computer Entertainment America",
		"Harmonix Drum Kit for PlayStation(R)3"};

	static const USBDescStrings buzz_desc_strings = {
		"",
		"Logitech Buzz(tm) Controller V1",
		"",
		"Logitech"};

	static const USBDescStrings kbm_desc_strings = {
		"",
		"USB Multipurpose Controller",
		"",
		"KONAMI"};

	static gsl::span<const InputBindingInfo> GetWheelBindings(PS2WheelTypes wt)
	{
		switch (wt)
		{
			case WT_GENERIC:
			{
				static constexpr const InputBindingInfo bindings[] = {
					{"SteeringLeft", "Steering Left", InputBindingInfo::Type::HalfAxis, CID_STEERING_L, GenericInputBinding::LeftStickLeft},
					{"SteeringRight", "Steering Right", InputBindingInfo::Type::HalfAxis, CID_STEERING_R, GenericInputBinding::LeftStickRight},
					{"Throttle", "Throttle", InputBindingInfo::Type::HalfAxis, CID_THROTTLE, GenericInputBinding::R2},
					{"Brake", "Brake", InputBindingInfo::Type::HalfAxis, CID_BRAKE, GenericInputBinding::L2},
					{"DPadUp", "D-Pad Up", InputBindingInfo::Type::Button, CID_DPAD_UP, GenericInputBinding::DPadUp},
					{"DPadDown", "D-Pad Down", InputBindingInfo::Type::Button, CID_DPAD_DOWN, GenericInputBinding::DPadDown},
					{"DPadLeft", "D-Pad Left", InputBindingInfo::Type::Button, CID_DPAD_LEFT, GenericInputBinding::DPadLeft},
					{"DPadRight", "D-Pad Right", InputBindingInfo::Type::Button, CID_DPAD_RIGHT, GenericInputBinding::DPadRight},
					{"Cross", "Cross", InputBindingInfo::Type::Button, CID_BUTTON0, GenericInputBinding::Cross},
					{"Square", "Square", InputBindingInfo::Type::Button, CID_BUTTON1, GenericInputBinding::Square},
					{"Circle", "Circle", InputBindingInfo::Type::Button, CID_BUTTON2, GenericInputBinding::Circle},
					{"Triangle", "Triangle", InputBindingInfo::Type::Button, CID_BUTTON3, GenericInputBinding::Triangle},
					{"L1", "L1", InputBindingInfo::Type::Button, CID_BUTTON5, GenericInputBinding::L1},
					{"R1", "R1", InputBindingInfo::Type::Button, CID_BUTTON4, GenericInputBinding::R1},
					{"L2", "L2", InputBindingInfo::Type::Button, CID_BUTTON7, GenericInputBinding::Unknown}, // used L2 for brake
					{"R2", "R2", InputBindingInfo::Type::Button, CID_BUTTON6, GenericInputBinding::Unknown}, // used R2 for throttle
					{"Select", "Select", InputBindingInfo::Type::Button, CID_BUTTON8, GenericInputBinding::Select},
					{"Start", "Start", InputBindingInfo::Type::Button, CID_BUTTON9, GenericInputBinding::Start},
					{"FFDevice", "Force Feedback", InputBindingInfo::Type::Device, 0, GenericInputBinding::Unknown},
				};

				return bindings;
			}

			case WT_DRIVING_FORCE_PRO:
			case WT_DRIVING_FORCE_PRO_1102:
			{
				static constexpr const InputBindingInfo bindings[] = {
					{"SteeringLeft", "Steering Left", InputBindingInfo::Type::HalfAxis, CID_STEERING_L, GenericInputBinding::LeftStickLeft},
					{"SteeringRight", "Steering Right", InputBindingInfo::Type::HalfAxis, CID_STEERING_R, GenericInputBinding::LeftStickRight},
					{"Throttle", "Throttle", InputBindingInfo::Type::HalfAxis, CID_THROTTLE, GenericInputBinding::R2},
					{"Brake", "Brake", InputBindingInfo::Type::HalfAxis, CID_BRAKE, GenericInputBinding::L2},
					{"DPadUp", "D-Pad Up", InputBindingInfo::Type::Button, CID_DPAD_UP, GenericInputBinding::DPadUp},
					{"DPadDown", "D-Pad Down", InputBindingInfo::Type::Button, CID_DPAD_DOWN, GenericInputBinding::DPadDown},
					{"DPadLeft", "D-Pad Left", InputBindingInfo::Type::Button, CID_DPAD_LEFT, GenericInputBinding::DPadLeft},
					{"DPadRight", "D-Pad Right", InputBindingInfo::Type::Button, CID_DPAD_RIGHT, GenericInputBinding::DPadRight},
					{"Cross", "Cross", InputBindingInfo::Type::Button, CID_BUTTON0, GenericInputBinding::Cross},
					{"Square", "Square", InputBindingInfo::Type::Button, CID_BUTTON1, GenericInputBinding::Square},
					{"Circle", "Circle", InputBindingInfo::Type::Button, CID_BUTTON2, GenericInputBinding::Circle},
					{"Triangle", "Triangle", InputBindingInfo::Type::Button, CID_BUTTON3, GenericInputBinding::Triangle},
					{"R1", "Shift Up / R1", InputBindingInfo::Type::Button, CID_BUTTON4, GenericInputBinding::R1},
					{"L1", "Shift Down / L1", InputBindingInfo::Type::Button, CID_BUTTON5, GenericInputBinding::L1},
					{"Select", "Select", InputBindingInfo::Type::Button, CID_BUTTON8, GenericInputBinding::Select},
					{"Start", "Start", InputBindingInfo::Type::Button, CID_BUTTON9, GenericInputBinding::Start},
					{"L2", "L2", InputBindingInfo::Type::Button, CID_BUTTON7, GenericInputBinding::Unknown}, // used L2 for brake
					{"R2", "R2", InputBindingInfo::Type::Button, CID_BUTTON6, GenericInputBinding::Unknown}, // used R2 for throttle
					{"L3", "L3", InputBindingInfo::Type::Button, CID_BUTTON11, GenericInputBinding::L3},
					{"R3", "R3", InputBindingInfo::Type::Button, CID_BUTTON10, GenericInputBinding::R3},
					{"FFDevice", "Force Feedback", InputBindingInfo::Type::Device, 0, GenericInputBinding::Unknown},
				};

				return bindings;
			}

			case WT_GT_FORCE:
			{
				static constexpr const InputBindingInfo bindings[] = {
					{"SteeringLeft", "Steering Left", InputBindingInfo::Type::HalfAxis, CID_STEERING_L, GenericInputBinding::LeftStickLeft},
					{"SteeringRight", "Steering Right", InputBindingInfo::Type::HalfAxis, CID_STEERING_R, GenericInputBinding::LeftStickRight},
					{"Throttle", "Throttle", InputBindingInfo::Type::HalfAxis, CID_THROTTLE, GenericInputBinding::R2},
					{"Brake", "Brake", InputBindingInfo::Type::HalfAxis, CID_BRAKE, GenericInputBinding::L2},
					{"MenuUp", "Menu Up", InputBindingInfo::Type::Button, CID_BUTTON0, GenericInputBinding::DPadUp},
					{"MenuDown", "Menu Down", InputBindingInfo::Type::Button, CID_BUTTON1, GenericInputBinding::DPadDown},
					{"X", "X", InputBindingInfo::Type::Button, CID_BUTTON2, GenericInputBinding::Square},
					{"Y", "Y", InputBindingInfo::Type::Button, CID_BUTTON3, GenericInputBinding::Triangle},
					{"A", "A", InputBindingInfo::Type::Button, CID_BUTTON4, GenericInputBinding::Cross},
					{"B", "B", InputBindingInfo::Type::Button, CID_BUTTON5, GenericInputBinding::Circle},
					{"FFDevice", "Force Feedback", InputBindingInfo::Type::Device, 0, GenericInputBinding::Unknown},
				};

				return bindings;
			}

			default:
			{
				return {};
			}
		}
	}

	static gsl::span<const SettingInfo> GetWheelSettings(PS2WheelTypes wt)
	{
		if (wt <= WT_GT_FORCE)
		{
			static constexpr const SettingInfo info[] = {
				{SettingInfo::Type::Integer, "SteeringSmoothing", "Steering Smoothing",
					"Smooths out changes in steering to the specified percentage per poll. Needed for using keyboards.",
					"0", "0", "100", "1", "%d%%", nullptr, nullptr, 1.0f},
			};

			return info;
		}
		else
		{
			return {};
		}
	}

	PadState::PadState(u32 port_, PS2WheelTypes type_)
		: port(port_)
		, type(type_)
	{
		if (type_ == WT_DRIVING_FORCE_PRO || type_ == WT_DRIVING_FORCE_PRO_1102)
			steering_range = 0x3FFF >> 1;
		else if (type_ == WT_SEGA_SEAMIC)
			steering_range = 0xFF >> 1;
		else
			steering_range = 0x3FF >> 1;

		steering_step = std::numeric_limits<u16>::max();

		// steering starts in the center
		data.last_steering = steering_range;
		data.steering = steering_range;

		// throttle/brake start unpressed
		data.throttle = 255;
		data.brake = 255;

		Reset();
	}

	PadState::~PadState() = default;

	void PadState::UpdateSettings(SettingsInterface& si, const char* devname)
	{
		const s32 smoothing_percent = USB::GetConfigInt(si, port, devname, "SteeringSmoothing", 0);
		if (smoothing_percent <= 0)
		{
			// none, allow any amount of change
			steering_step = std::numeric_limits<u16>::max();
		}
		else
		{
			steering_step = static_cast<u16>(std::clamp<s32>((steering_range * smoothing_percent) / 100,
				1, std::numeric_limits<u16>::max()));
		}

		if (HasFF())
		{
			const std::string ffdevname(USB::GetConfigString(si, port, devname, "FFDevice"));
			if (ffdevname != mFFdevName)
			{
				mFFdev.reset();
				mFFdevName = std::move(ffdevname);
				OpenFFDevice();
			}
		}
	}

	void PadState::Reset()
	{
		data.steering = steering_range;
		mFFstate = {};
	}

	int PadState::TokenIn(u8* buf, int len)
	{
		// TODO: This is still pretty gross and needs cleaning up.

		struct wheel_lohi
		{
			u32 lo;
			u32 hi;
		};

		wheel_lohi* w = reinterpret_cast<wheel_lohi*>(buf);
		std::memset(w, 0, 8);

		switch (type)
		{
			case WT_GENERIC:
			{
				UpdateSteering();
				UpdateHatSwitch();

				DbgCon.WriteLn("Steering: %d Throttle: %d Brake: %d Buttons: %d",
					data.steering, data.throttle, data.brake, data.buttons);

				w->lo = data.steering & 0x3FF;
				w->lo |= (data.buttons & 0xFFF) << 10;
				w->lo |= 0xFF << 24;

				w->hi = (data.hatswitch & 0xF);
				w->hi |= (data.throttle & 0xFF) << 8;
				w->hi |= (data.brake & 0xFF) << 16;

				return len;
			}

			case WT_GT_FORCE:
			{
				UpdateSteering();
				UpdateHatSwitch();

				w->lo = data.steering & 0x3FF;
				w->lo |= (data.buttons & 0xFFF) << 10;
				w->lo |= 0xFF << 24;

				w->hi = (data.throttle & 0xFF);
				w->hi |= (data.brake & 0xFF) << 8;

				return len;
			}

			case WT_DRIVING_FORCE_PRO:
			{
				UpdateSteering();
				UpdateHatSwitch();

				w->lo = data.steering & 0x3FFF;
				w->lo |= (data.buttons & 0x3FFF) << 14;
				w->lo |= (data.hatswitch & 0xF) << 28;

				w->hi = 0x00;
				w->hi |= data.throttle << 8;
				w->hi |= data.brake << 16; //axis_rz
				w->hi |= 0x11 << 24; //enables wheel and pedals?

				return len;
			}

			case WT_DRIVING_FORCE_PRO_1102:
			{
				UpdateSteering();
				UpdateHatSwitch();

				// what's up with the bitmap?
				// xxxxxxxx xxxxxxbb bbbbbbbb bbbbhhhh ???????? ?01zzzzz 1rrrrrr1 10001000
				w->lo = data.steering & 0x3FFF;
				w->lo |= (data.buttons & 0x3FFF) << 14;
				w->lo |= (data.hatswitch & 0xF) << 28;

				w->hi = 0x00;
				//w->hi |= 0 << 9; //bit 9 must be 0
				w->hi |= (1 | (data.throttle * 0x3F) / 0xFF) << 10; //axis_z
				w->hi |= 1 << 16; //bit 16 must be 1
				w->hi |= ((0x3F - (data.brake * 0x3F) / 0xFF) & 0x3F) << 17; //axis_rz
				w->hi |= 1 << 23; //bit 23 must be 1
				w->hi |= 0x11 << 24; //enables wheel and pedals?

				return len;
			}

			case WT_ROCKBAND1_DRUMKIT:
			{
				UpdateHatSwitch();

				w->lo = (data.buttons & 0xFFF);
				w->lo |= (data.hatswitch & 0xF) << 16;

				return len;
			}

			case WT_BUZZ_CONTROLLER:
			{
				// https://gist.github.com/Lewiscowles1986/eef220dac6f0549e4702393a7b9351f6
				buf[0] = 0x7f;
				buf[1] = 0x7f;
				buf[2] = data.buttons & 0xff;
				buf[3] = (data.buttons >> 8) & 0xff;
				buf[4] = 0xf0 | ((data.buttons >> 16) & 0xf);

				return 5;
			}

			case WT_SEGA_SEAMIC:
			{
				UpdateSteering();
				UpdateHatSwitch();

				buf[0] = data.steering & 0xFF;
				buf[1] = data.throttle & 0xFF;
				buf[2] = data.brake & 0xFF;
				buf[3] = data.hatswitch & 0x0F; // 4bits?
				buf[3] |= (data.buttons & 0x0F) << 4; // 4 bits // TODO Or does it start at buf[4]?
				buf[4] = (data.buttons >> 4) & 0x3F; // 10 - 4 = 6 bits

				return len;
			}

			case WT_KEYBOARDMANIA_CONTROLLER:
			{
				buf[0] = 0x3F;
				buf[1] = data.buttons & 0xFF;
				buf[2] = (data.buttons >> 8) & 0xFF;
				buf[3] = (data.buttons >> 16) & 0xFF;
				buf[4] = (data.buttons >> 24) & 0xFF;

				return len;
			}

			default:
			{
				return len;
			}
		}
	}

	int PadState::TokenOut(const u8* data, int len)
	{
		const ff_data* ffdata = reinterpret_cast<const ff_data*>(data);
		bool hires = (type == WT_DRIVING_FORCE_PRO || type == WT_DRIVING_FORCE_PRO_1102);
		ParseFFData(ffdata, hires);
		return len;
	}

	float PadState::GetBindValue(u32 bind_index) const
	{
		switch (bind_index)
		{
			case CID_STEERING_L:
				return static_cast<float>(data.steering_left) / static_cast<float>(steering_range);

			case CID_STEERING_R:
				return static_cast<float>(data.steering_right) / static_cast<float>(steering_range);

			case CID_THROTTLE:
				return 1.0f - (static_cast<float>(data.throttle) / 255.0f);

			case CID_BRAKE:
				return 1.0f - (static_cast<float>(data.brake) / 255.0f);

			case CID_DPAD_UP:
				return static_cast<float>(data.hat_up);

			case CID_DPAD_DOWN:
				return static_cast<float>(data.hat_down);

			case CID_DPAD_LEFT:
				return static_cast<float>(data.hat_left);

			case CID_DPAD_RIGHT:
				return static_cast<float>(data.hat_right);

			case CID_BUTTON0:
			case CID_BUTTON1:
			case CID_BUTTON2:
			case CID_BUTTON3:
			case CID_BUTTON5:
			case CID_BUTTON4:
			case CID_BUTTON7:
			case CID_BUTTON6:
			case CID_BUTTON8:
			case CID_BUTTON9:
			case CID_BUTTON10:
			case CID_BUTTON11:
			case CID_BUTTON12:
			case CID_BUTTON13:
			case CID_BUTTON14:
			case CID_BUTTON15:
			case CID_BUTTON16:
			case CID_BUTTON17:
			case CID_BUTTON18:
			case CID_BUTTON19:
			case CID_BUTTON20:
			case CID_BUTTON21:
			case CID_BUTTON22:
			case CID_BUTTON23:
			case CID_BUTTON24:
			{
				const u32 mask = (1u << (bind_index - CID_BUTTON0));
				return ((data.buttons & mask) != 0u) ? 1.0f : 0.0f;
			}

			default:
				return 0.0f;
		}
	}

	void PadState::SetBindValue(u32 bind_index, float value)
	{
		switch (bind_index)
		{
			case CID_STEERING_L:
				data.steering_left = static_cast<s16>(std::lroundf(value * static_cast<float>(steering_range)));
				UpdateSteering();
				break;

			case CID_STEERING_R:
				data.steering_right = static_cast<s16>(std::lroundf(value * static_cast<float>(steering_range)));
				UpdateSteering();
				break;

			case CID_THROTTLE:
				data.throttle = static_cast<u32>(255 - std::clamp<long>(std::lroundf(value * 255.0f), 0, 255));
				break;

			case CID_BRAKE:
				data.brake = static_cast<u32>(255 - std::clamp<long>(std::lroundf(value * 255.0f), 0, 255));
				break;

			case CID_DPAD_UP:
				data.hat_up = static_cast<u8>(std::clamp<long>(std::lroundf(value * 255.0f), 0, 255));
				UpdateHatSwitch();
				break;
			case CID_DPAD_DOWN:
				data.hat_down = static_cast<u8>(std::clamp<long>(std::lroundf(value * 255.0f), 0, 255));
				UpdateHatSwitch();
				break;
			case CID_DPAD_LEFT:
				data.hat_left = static_cast<u8>(std::clamp<long>(std::lroundf(value * 255.0f), 0, 255));
				UpdateHatSwitch();
				break;
			case CID_DPAD_RIGHT:
				data.hat_right = static_cast<u8>(std::clamp<long>(std::lroundf(value * 255.0f), 0, 255));
				UpdateHatSwitch();
				break;

			case CID_BUTTON0:
			case CID_BUTTON1:
			case CID_BUTTON2:
			case CID_BUTTON3:
			case CID_BUTTON5:
			case CID_BUTTON4:
			case CID_BUTTON7:
			case CID_BUTTON6:
			case CID_BUTTON8:
			case CID_BUTTON9:
			case CID_BUTTON10:
			case CID_BUTTON11:
			case CID_BUTTON12:
			case CID_BUTTON13:
			case CID_BUTTON14:
			case CID_BUTTON15:
			case CID_BUTTON16:
			case CID_BUTTON17:
			case CID_BUTTON18:
			case CID_BUTTON19:
			case CID_BUTTON20:
			case CID_BUTTON21:
			case CID_BUTTON22:
			{
				const u32 mask = (1u << (bind_index - CID_BUTTON0));
				if (value >= 0.5f)
					data.buttons |= mask;
				else
					data.buttons &= ~mask;
			}
			break;

			default:
				break;
		}
	}

	void PadState::UpdateSteering()
	{
		u16 value;
		if (data.steering_left > 0)
			value = static_cast<u16>(std::max<int>(steering_range - data.steering_left, 0));
		else
			value = static_cast<u16>(std::min<int>(steering_range + data.steering_right, steering_range * 2));

		// TODO: Smoothing, don't jump too much
		//data.steering = value;
		if (value < data.steering)
			data.steering -= std::min<u16>(data.steering - value, steering_step);
		else if (value > data.steering)
			data.steering += std::min<u16>(value - data.steering, steering_step);
	}

	void PadState::UpdateHatSwitch()
	{
		if (data.hat_up && data.hat_right)
			data.hatswitch = 1;
		else if (data.hat_right && data.hat_down)
			data.hatswitch = 3;
		else if (data.hat_down && data.hat_left)
			data.hatswitch = 5;
		else if (data.hat_left && data.hat_up)
			data.hatswitch = 7;
		else if (data.hat_up)
			data.hatswitch = 0;
		else if (data.hat_right)
			data.hatswitch = 2;
		else if (data.hat_down)
			data.hatswitch = 4;
		else if (data.hat_left)
			data.hatswitch = 6;
		else
			data.hatswitch = 8;
	}

	bool PadState::HasFF() const
	{
		// only do force feedback for wheels...
		return (type <= WT_GT_FORCE);
	}

	void PadState::OpenFFDevice()
	{
		if (mFFdevName.empty())
			return;

		mFFdev.reset();

#ifdef SDL_BUILD
		mFFdev = SDLFFDevice::Create(mFFdevName);
#endif
	}

	static u32 gametrak_compute_key(u32* key)
	{
		u32 ret = 0;
		ret = *key << 2 & 0xFC0000;
		ret |= *key << 17 & 0x020000;
		ret ^= *key << 16 & 0xFE0000;
		ret |= *key & 0x010000;
		ret |= *key >> 9 & 0x007F7F;
		ret |= *key << 7 & 0x008080;
		*key = ret;
		return ret >> 16;
	}

	static void pad_handle_data(USBDevice* dev, USBPacket* p)
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);

		switch (p->pid)
		{
			case USB_TOKEN_IN:
				if (p->ep->nr == 1)
				{
					int ret = s->TokenIn(p->buffer_ptr, p->buffer_size);

					if (ret > 0)
						p->actual_length += std::min<u32>(static_cast<u32>(ret), p->buffer_size);
					else
						p->status = ret;
				}
				else
				{
					goto fail;
				}
				break;
			case USB_TOKEN_OUT:
				/*Console.Warning("usb-pad: data token out len=0x%X %X,%X,%X,%X,%X,%X,%X,%X\n",len,
			data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);*/
				//Console.Warning("usb-pad: data token out len=0x%X\n",len);
				s->TokenOut(p->buffer_ptr, p->buffer_size);
				break;
			default:
			fail:
				p->status = USB_RET_STALL;
				break;
		}
	}

	static void pad_handle_reset(USBDevice* dev)
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);
		s->Reset();
	}

	static void pad_handle_control(USBDevice* dev, USBPacket* p, int request, int value,
		int index, int length, uint8_t* data)
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);
		int ret = 0;

		switch (request)
		{
			case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
				ret = usb_desc_handle_control(dev, p, request, value, index, length, data);
				if (ret < 0)
					goto fail;

				break;
			case InterfaceRequest | USB_REQ_GET_DESCRIPTOR: //GT3
				switch (value >> 8)
				{
					// TODO: Move to constructor
					case USB_DT_REPORT:
						if (s->type == WT_DRIVING_FORCE_PRO || s->type == WT_DRIVING_FORCE_PRO_1102)
						{
							ret = sizeof(pad_driving_force_pro_hid_report_descriptor);
							memcpy(data, pad_driving_force_pro_hid_report_descriptor, ret);
						}
						else if (s->type == WT_GT_FORCE)
						{
							ret = sizeof(pad_gtforce_hid_report_descriptor);
							memcpy(data, pad_gtforce_hid_report_descriptor, ret);
						}
						else if (s->type == WT_KEYBOARDMANIA_CONTROLLER)
						{
							ret = sizeof(kbm_hid_report_descriptor);
							memcpy(data, kbm_hid_report_descriptor, ret);
						}
						else if (s->type == WT_GENERIC)
						{
							ret = sizeof(pad_driving_force_hid_separate_report_descriptor);
							memcpy(data, pad_driving_force_hid_separate_report_descriptor, ret);
						}
						else if (s->type == WT_BUZZ_CONTROLLER)
						{
							ret = sizeof(buzz_hid_report_descriptor);
							memcpy(data, buzz_hid_report_descriptor, ret);
						}
						p->actual_length = ret;
						break;
					default:
						goto fail;
				}
				break;

			/* hid specific requests */
			case SET_REPORT:
				// no idea, Rock Band 2 keeps spamming this
				if (length > 0)
				{
					/* 0x01: Num Lock LED
			 * 0x02: Caps Lock LED
			 * 0x04: Scroll Lock LED
			 * 0x08: Compose LED
			 * 0x10: Kana LED */
					p->actual_length = 0;
					//p->status = USB_RET_SUCCESS;
				}
				break;
			case SET_IDLE:
				break;
			default:
				ret = usb_desc_handle_control(dev, p, request, value, index, length, data);
				if (ret >= 0)
				{
					return;
				}
			fail:
				p->status = USB_RET_STALL;
				break;
		}
	}

	static void pad_handle_destroy(USBDevice* dev)
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);
		delete s;
	}

	static void pad_init(PadState* s)
	{
		s->dev.speed = USB_SPEED_FULL;
		s->dev.klass.handle_attach = usb_desc_attach;
		s->dev.klass.handle_reset = pad_handle_reset;
		s->dev.klass.handle_control = pad_handle_control;
		s->dev.klass.handle_data = pad_handle_data;
		s->dev.klass.unrealize = pad_handle_destroy;
		s->dev.klass.usb_desc = &s->desc;
		s->dev.klass.product_desc = nullptr;

		usb_desc_init(&s->dev);
		usb_ep_init(&s->dev);
		pad_handle_reset(&s->dev);
	}

	USBDevice* PadDevice::CreateDevice(SettingsInterface& si, u32 port, u32 subtype) const
	{
		if (subtype >= WT_COUNT)
			return nullptr;

		PadState* s = new PadState(port, static_cast<PS2WheelTypes>(subtype));
		s->desc.full = &s->desc_dev;
		s->desc.str = df_desc_strings;

		const uint8_t* dev_desc = df_dev_descriptor;
		int dev_desc_len = sizeof(df_dev_descriptor);
		const uint8_t* config_desc = df_config_descriptor;
		int config_desc_len = sizeof(df_config_descriptor);

		switch (s->type)
		{
			case WT_DRIVING_FORCE_PRO:
			{
				dev_desc = dfp_dev_descriptor;
				dev_desc_len = sizeof(dfp_dev_descriptor);
				config_desc = dfp_config_descriptor;
				config_desc_len = sizeof(dfp_config_descriptor);
				s->desc.str = dfp_desc_strings;
			}
			break;
			case WT_DRIVING_FORCE_PRO_1102:
			{
				dev_desc = dfp_dev_descriptor_1102;
				dev_desc_len = sizeof(dfp_dev_descriptor_1102);
				config_desc = dfp_config_descriptor;
				config_desc_len = sizeof(dfp_config_descriptor);
				s->desc.str = dfp_desc_strings;
			}
			break;
			case WT_GT_FORCE:
			{
				dev_desc = gtf_dev_descriptor;
				dev_desc_len = sizeof(gtf_dev_descriptor);
				config_desc = gtforce_config_descriptor; //TODO
				config_desc_len = sizeof(gtforce_config_descriptor);
				s->desc.str = gtf_desc_strings;
			}
			default:
				break;
		}

		if (usb_desc_parse_dev(dev_desc, dev_desc_len, s->desc, s->desc_dev) < 0)
			goto fail;
		if (usb_desc_parse_config(config_desc, config_desc_len, s->desc_dev) < 0)
			goto fail;

		s->UpdateSettings(si, TypeName());
		pad_init(s);

		return &s->dev;

	fail:
		pad_handle_destroy(&s->dev);
		return nullptr;
	}

	const char* PadDevice::Name() const
	{
		return "Wheel Device";
	}

	const char* PadDevice::TypeName() const
	{
		return "Pad";
	}

	bool PadDevice::Freeze(USBDevice* dev, StateWrapper& sw) const
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);

		if (!sw.DoMarker("PadDevice"))
			return false;

		sw.Do(&s->data.last_steering);
		sw.DoPOD(&s->mFFstate);
		return true;
	}

	void PadDevice::UpdateSettings(USBDevice* dev, SettingsInterface& si) const
	{
		USB_CONTAINER_OF(dev, PadState, dev)->UpdateSettings(si, TypeName());
	}

	float PadDevice::GetBindingValue(const USBDevice* dev, u32 bind_index) const
	{
		const PadState* s = USB_CONTAINER_OF(dev, const PadState, dev);
		return s->GetBindValue(bind_index);
	}

	void PadDevice::SetBindingValue(USBDevice* dev, u32 bind_index, float value) const
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);
		s->SetBindValue(bind_index, value);
	}

	std::vector<std::string> PadDevice::SubTypes() const
	{
		return {"Driving Force", "Driving Force Pro", "Driving Force Pro (rev11.02)", "GT Force"};
	}

	gsl::span<const InputBindingInfo> PadDevice::Bindings(u32 subtype) const
	{
		return GetWheelBindings(static_cast<PS2WheelTypes>(subtype));
	}

	gsl::span<const SettingInfo> PadDevice::Settings(u32 subtype) const
	{
		return GetWheelSettings(static_cast<PS2WheelTypes>(subtype));
	}

	void PadDevice::InputDeviceConnected(USBDevice* dev, const std::string_view& identifier) const
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);
		if (s->mFFdevName == identifier)
			s->OpenFFDevice();
	}

	void PadDevice::InputDeviceDisconnected(USBDevice* dev, const std::string_view& identifier) const
	{
		PadState* s = USB_CONTAINER_OF(dev, PadState, dev);
		if (s->mFFdevName == identifier)
			s->mFFdev.reset();
	}

	// ---- Rock Band drum kit ----

	const char* RBDrumKitDevice::Name() const
	{
		return "Rock Band Drum Kit";
	}

	const char* RBDrumKitDevice::TypeName() const
	{
		return "RBDrumKit";
	}

	USBDevice* RBDrumKitDevice::CreateDevice(SettingsInterface& si, u32 port, u32 subtype) const
	{
		PadState* s = new PadState(port, WT_ROCKBAND1_DRUMKIT);

		s->desc.full = &s->desc_dev;
		s->desc.str = rb1_desc_strings;

		if (usb_desc_parse_dev(rb1_dev_descriptor, sizeof(rb1_dev_descriptor), s->desc, s->desc_dev) < 0)
			goto fail;
		if (usb_desc_parse_config(rb1_config_descriptor, sizeof(rb1_config_descriptor), s->desc_dev) < 0)
			goto fail;

		pad_init(s);

		return &s->dev;

	fail:
		pad_handle_destroy(&s->dev);
		return nullptr;
	}

	std::vector<std::string> RBDrumKitDevice::SubTypes() const
	{
		return {};
	}

	gsl::span<const InputBindingInfo> RBDrumKitDevice::Bindings(u32 subtype) const
	{
		static constexpr const InputBindingInfo bindings[] = {
			{"Blue", "Blue", InputBindingInfo::Type::Button, CID_BUTTON0, GenericInputBinding::R1},
			{"Green", "Green", InputBindingInfo::Type::Button, CID_BUTTON1, GenericInputBinding::Triangle},
			{"Red", "Red", InputBindingInfo::Type::Button, CID_BUTTON2, GenericInputBinding::Circle},
			{"Yellow", "Yellow", InputBindingInfo::Type::Button, CID_BUTTON3, GenericInputBinding::Square},
			{"Orange", "Orange", InputBindingInfo::Type::Button, CID_BUTTON4, GenericInputBinding::Cross},
			{"Select", "Select", InputBindingInfo::Type::Button, CID_BUTTON8, GenericInputBinding::Select},
			{"Start", "Start", InputBindingInfo::Type::Button, CID_BUTTON9, GenericInputBinding::Start},
		};

		return bindings;
	}

	gsl::span<const SettingInfo> RBDrumKitDevice::Settings(u32 subtype) const
	{
		return {};
	}

	// ---- Buzz ----

	const char* BuzzDevice::Name() const
	{
		return "Buzz Controller";
	}

	const char* BuzzDevice::TypeName() const
	{
		return "BuzzDevice";
	}

	std::vector<std::string> BuzzDevice::SubTypes() const
	{
		return {};
	}

	gsl::span<const InputBindingInfo> BuzzDevice::Bindings(u32 subtype) const
	{
		static constexpr const InputBindingInfo bindings[] = {
			{"Red", "Red", InputBindingInfo::Type::Button, CID_BUTTON0, GenericInputBinding::R1},
			{"Yellow", "Yellow", InputBindingInfo::Type::Button, CID_BUTTON1, GenericInputBinding::Triangle},
			{"Green", "Green", InputBindingInfo::Type::Button, CID_BUTTON2, GenericInputBinding::Circle},
			{"Orange", "Orange", InputBindingInfo::Type::Button, CID_BUTTON3, GenericInputBinding::Square},
			{"Blue", "Blue", InputBindingInfo::Type::Button, CID_BUTTON4, GenericInputBinding::Cross},
		};

		return bindings;
	}

	gsl::span<const SettingInfo> BuzzDevice::Settings(u32 subtype) const
	{
		return {};
	}

	USBDevice* BuzzDevice::CreateDevice(SettingsInterface& si, u32 port, u32 subtype) const
	{
		PadState* s = new PadState(port, WT_BUZZ_CONTROLLER);

		s->desc.full = &s->desc_dev;
		s->desc.str = buzz_desc_strings;

		if (usb_desc_parse_dev(buzz_dev_descriptor, sizeof(buzz_dev_descriptor), s->desc, s->desc_dev) < 0)
			goto fail;
		if (usb_desc_parse_config(buzz_config_descriptor, sizeof(buzz_config_descriptor), s->desc_dev) < 0)
			goto fail;

		pad_init(s);

		return &s->dev;

	fail:
		pad_handle_destroy(&s->dev);
		return nullptr;
	}

	// ---- Keyboardmania ----

	const char* KeyboardmaniaDevice::Name() const
	{
		return "Keyboardmania";
	}

	const char* KeyboardmaniaDevice::TypeName() const
	{
		return "Keyboardmania";
	}

	std::vector<std::string> KeyboardmaniaDevice::SubTypes() const
	{
		return {};
	}

	gsl::span<const InputBindingInfo> KeyboardmaniaDevice::Bindings(u32 subtype) const
	{
		static constexpr const InputBindingInfo bindings[] = {
			{"C", "C", InputBindingInfo::Type::Button, CID_BUTTON0, GenericInputBinding::Unknown},
			{"CSharp", "C#", InputBindingInfo::Type::Button, CID_BUTTON1, GenericInputBinding::Unknown},
			{"D", "D", InputBindingInfo::Type::Button, CID_BUTTON2, GenericInputBinding::Unknown},
			{"EFlat", "Eb", InputBindingInfo::Type::Button, CID_BUTTON3, GenericInputBinding::Unknown},
			{"E", "E", InputBindingInfo::Type::Button, CID_BUTTON4, GenericInputBinding::Unknown},
			{"F", "F", InputBindingInfo::Type::Button, CID_BUTTON5, GenericInputBinding::Unknown},
			{"FSharp", "F#", InputBindingInfo::Type::Button, CID_BUTTON6, GenericInputBinding::Unknown},
			{"G", "G", InputBindingInfo::Type::Button, CID_BUTTON7, GenericInputBinding::Unknown},
			{"AFlat", "Ab", InputBindingInfo::Type::Button, CID_BUTTON8, GenericInputBinding::Unknown},
			{"A", "A", InputBindingInfo::Type::Button, CID_BUTTON9, GenericInputBinding::Unknown},
			{"BFlat", "Bb", InputBindingInfo::Type::Button, CID_BUTTON10, GenericInputBinding::Unknown},
			{"B", "B", InputBindingInfo::Type::Button, CID_BUTTON11, GenericInputBinding::Unknown},
			{"C2", "+C", InputBindingInfo::Type::Button, CID_BUTTON12, GenericInputBinding::Unknown},
			{"CSharp2", "+C#", InputBindingInfo::Type::Button, CID_BUTTON13, GenericInputBinding::Unknown},
			{"D2", "+D", InputBindingInfo::Type::Button, CID_BUTTON14, GenericInputBinding::Unknown},
			{"EFlat2", "+Eb", InputBindingInfo::Type::Button, CID_BUTTON15, GenericInputBinding::Unknown},
			{"E2", "+E", InputBindingInfo::Type::Button, CID_BUTTON16, GenericInputBinding::Unknown},
			{"F2", "+F", InputBindingInfo::Type::Button, CID_BUTTON17, GenericInputBinding::Unknown},
			{"FSharp2", "+F#", InputBindingInfo::Type::Button, CID_BUTTON18, GenericInputBinding::Unknown},
			{"G2", "+G", InputBindingInfo::Type::Button, CID_BUTTON19, GenericInputBinding::Unknown},
			{"AFlat2", "+Ab", InputBindingInfo::Type::Button, CID_BUTTON20, GenericInputBinding::Unknown},
			{"A2", "+A", InputBindingInfo::Type::Button, CID_BUTTON21, GenericInputBinding::Unknown},
			{"BFlat2", "+Bb", InputBindingInfo::Type::Button, CID_BUTTON22, GenericInputBinding::Unknown},
			{"B2", "+B", InputBindingInfo::Type::Button, CID_BUTTON23, GenericInputBinding::Unknown},
			{"C3", "++C", InputBindingInfo::Type::Button, CID_BUTTON24, GenericInputBinding::Unknown},
		};

		return bindings;
	}

	gsl::span<const SettingInfo> KeyboardmaniaDevice::Settings(u32 subtype) const
	{
		return {};
	}

	USBDevice* KeyboardmaniaDevice::CreateDevice(SettingsInterface& si, u32 port, u32 subtype) const
	{
		PadState* s = new PadState(port, WT_KEYBOARDMANIA_CONTROLLER);

		s->desc.full = &s->desc_dev;
		s->desc.str = kbm_desc_strings;

		if (usb_desc_parse_dev(kbm_dev_descriptor, sizeof(kbm_dev_descriptor), s->desc, s->desc_dev) < 0)
			goto fail;
		if (usb_desc_parse_config(kbm_config_descriptor, sizeof(kbm_config_descriptor), s->desc_dev) < 0)
			goto fail;

		pad_init(s);

		return &s->dev;

	fail:
		pad_handle_destroy(&s->dev);
		return nullptr;
	}
} // namespace usb_pad
