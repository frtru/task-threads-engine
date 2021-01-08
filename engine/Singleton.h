#pragma once

template <class T>
class Singleton
{
public:
  Singleton() = default;
  virtual ~Singleton() = default;

  static T* GetInstance() {
    static T instance;
    return &instance;
  }
};