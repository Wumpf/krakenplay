#pragma once

#include "../krakenplay/messages.h"

namespace Krakenplay
{
	/// Interface for all input fetching devices.
	class InputFetchDevice
	{
	public:
		virtual ~InputFetchDevice() {}

		/// Updates the interla input state object.
		/// \return If the InputFetchDevice has a recent important change, it returns true to request an update.
		virtual bool FetchState() = 0;

		/// Returns a pointer to the current state blob. Must be a pointer to a datastructure specified in Krakenplay::StateObjects
		/// \param[out]	size	Size of the returned state blob pointer.
		/// \see ../krakenplay/messages.h
		/// \see GetStateSize()
		virtual const void* GetState(unsigned int& size) const = 0;

		/// Returns the corresponding message header to GetState.
		virtual MessageChunkHeader GetStateMessageHeader() const = 0;

		/// Returns the message for a disconnect message. This message consists only of a header.
		virtual MessageChunkHeader GetDisconnectMessageHeader() const = 0;
	};
}