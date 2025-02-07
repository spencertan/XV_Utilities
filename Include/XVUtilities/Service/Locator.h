#pragma once
#include <cassert>
#include "../STL_C/UnorderedMap.h"
#include "../STL_C/Vector.h"
#include "../Enumerate.h"
#include "Descriptor.h"

namespace XV::Service
{

  class Locator
  {
    inline static vector<Ptr<byte>> services;
    inline static vector<Ptr<const Service::Descriptor>> descriptors;

  public:
    template <typename TService, typename... Args>
    static TService &Register(Args &&...args)
    {
#ifdef XV_DEBUG
      for (const auto &ptr : descriptors)
        assert(ptr->id == Service::info<TService>.id);
#endif
      descriptors.push_back(&Service::info<TService>);
      return *std::bit_cast<Ptr<TService>>(services.emplace_back(std::bit_cast<Ptr<byte>>(new TService(std::forward<Args>(args)...))));
    }

    template <typename TService>
    static TService &Register()
    {
#ifdef XV_DEBUG
      for (const auto &ptr : descriptors)
        assert(ptr->id == Service::info<TService>.id);
#endif
      descriptors.push_back(&Service::info<TService>);
      return *std::bit_cast<Ptr<TService>>(services.emplace_back(std::bit_cast<Ptr<byte>>(new TService())));
    }

    template <typename TService>
    static TService &Get()
    {
      Ptr<TService> result{nullptr};
      for (const auto &[i, ptr] : Enumerate(descriptors))
        if (ptr->id == Service::info<TService>.id)
        {
          result = std::bit_cast<Ptr<TService>>(services[i]);
          assert(result != nullptr) break;
        }
      return *result;
    }

    static void Shutdown()
    {
      for (const auto &[i, ptr] : REnumerate(descriptors))
      {
        if (ptr->destructor)
          ptr->destructor(services[i]);
        delete services[i];
      }
    }
  };

}