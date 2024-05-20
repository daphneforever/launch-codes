/*
  ==============================================================================

    ParamIDs.h
    Created: 10 May 2024 3:17:23pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

namespace ParamIDs
{

inline constexpr auto oscIO       { "Osc I/O" };
inline constexpr auto oscGain     { "Osc Gain" };
inline constexpr auto oscInitial  { "Osc Inital Pitch" };
inline constexpr auto oscPeak     { "Osc Peak Pitch" };
inline constexpr auto oscSustain  { "Osc Sustain Pitch" };
inline constexpr auto oscEnd      { "Osc End Pitch" };
inline constexpr auto oscAttack   { "Osc Attack" };
inline constexpr auto oscDecay    { "Osc Decay" };
inline constexpr auto oscRelease  { "Osc Release" };

inline constexpr auto noiseIO   { "Noise I/O" };
inline constexpr auto noiseGain { "Noise Gain" };
inline constexpr auto verbIO    { "Reverb I/O" };
inline constexpr auto size      { "Size" };
inline constexpr auto damp      { "Damp" };
inline constexpr auto width     { "Width" };
inline constexpr auto verbMix   { "Dry/Wet" };

} // namespace ParamIDs
