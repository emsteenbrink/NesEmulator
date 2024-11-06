#pragma once

class IChannel
{
public:
  virtual ~IChannel() {}

  virtual void Clock() = 0;

  virtual void DoQuarterFrame() = 0;
  virtual void DoHalfFrame() = 0;
};