#pragma once

// Reverse engineered structures for Cataclysm's new skirmish AI

struct ShipClassCountsInner
{
	ubyte unk[0x9D08];
	udword count;
};

struct ShipClassCounts
{
	udword unk;
	ShipClassCountsInner inner;
};

static_assert(offsetof(ShipClassCounts, inner.count) == 0x9D0C);