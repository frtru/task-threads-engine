#pragma once

// Those are just a few simple designs of object pools to 
// avoid reallocating memory uselessly using STL containers.
// For a more optimal implementation, you'd wanna go more
// towards aligned memory pool allocators that is on par with
// boost::pool_allocator. Maybe even implement your own alloc
// policy for your buffers to prevent fragmentation in your 
// own buffer.
//
// - DynamicPointerPool -
// Allocate on heap for every new item and register
// its pointer so this particular space can be reused.
// It can easily be tweaked or be derived from to enhance
// the properties
//
// +++  Flexibility
// ++   Error handling when creating objects
// +    Reuse of already allocated space
// ---  No cache locality (non-contiguous allocations)
// ---  Doesnt prevent fragmentation
// --   Cannot allocate blocks (although it could be implemented)

#include <stack>
#include <type_traits>
#include <vector>

template<typename Type>
class DynamicPointerPool
{
public:
  DynamicPointerPool() {}
  //delete all of the available memory chunks:
  ~DynamicPointerPool() {
    while (!m_pool.empty()) {
      delete(m_pool.top());
      m_pool.pop();
    }
  }

  DynamicPointerPool(const DynamicPointerPool &) = delete;
  DynamicPointerPool(DynamicPointerPool && other)
    : m_pool(std::move(other.m_pool)) {}

  DynamicPointerPool& operator=(const DynamicPointerPool&) = delete;
  DynamicPointerPool& operator=(DynamicPointerPool&& other) {
    this->m_pool(std::move(other.m_pool));
  }

  // Create an instance of Type:
  template<typename... Args>
  Type* Create(Args && ...args) {
    Type *place = Allocate();

    try {
      ::new (place) Type(std::forward<Args>(args)...);
    }
    catch (...) {
      m_pool.push(place);
      throw;
    }

    return place;
  }

  // Remove an instance of Type (add memory to the pool):
  void Release(Type* obj) {
    obj->~Type();
    Deallocate(obj);
  }


  std::size_t FreeChunksCount() {
    return m_pool.size();
  }

private:
  // Allocate a chunk of memory as big as Type needs:
  Type* Allocate() {
    Type *place;

    if (!m_pool.empty()) {
      place = m_pool.top();
      m_pool.pop();
    }
    else {
      place = static_cast<Type*>(malloc(sizeof(Type)));
    }

    return place;
  }

  //Keeping as a utility
  void Deallocate(Type* obj) {
    m_pool.push(obj);
  }

  //stack to hold pointers to free chunks:
  std::stack<Type*> m_pool;
};

// - ObjectPool -
// Uses a vector of objects (not pointers) as its underlying
// container. The vector guarantees contiguity of the 
// memory space used and offers better cache locality for smaller
// objects (still remains a Array of Structures implementation, so
// not as efficient as a Structure of Arrays design for caching).
// You can have a fixed or dynamic size at the cost of a IsEmpty
// check at each creation and a risk of an expensive memcpy to 
// allocate a bigger buffer (normal vector behavior for capacity
// alloc of 2 times the previous capacity).
//
// +++  Flexibility
// +++  Reuse of already allocated space
// +++  Contiguous
// +++  Prevents fragmentation
// --   Somewhat expensive copy operations to readd to pool
// --   Cannot allocate blocks (although it could be implemented)
// -    Voluntarily has no error handling (could be added though)
template<typename Type, bool DynamicSize = true>
class ObjectPool
{
public:
  template <typename... Args>
  explicit ObjectPool(std::size_t itemCount, Args&&... args)
    : m_pool(std::vector<Type>(itemCount, Type(args...))) {}
  
  // Vector can take care of deleting everything
  ~ObjectPool() = default;

  ObjectPool(const ObjectPool &) = delete;
  ObjectPool(ObjectPool && other)
    : m_pool(std::move(other.m_pool)) {}

  ObjectPool& operator=(const ObjectPool&) = delete;
  ObjectPool& operator=(ObjectPool&& other) {
    this->m_pool(std::move(other.m_pool));
  }

  template<typename... Args>
  Type* Create(Args && ...args) {
    // This is the resizeable portion. This of course
    // affects the performance, but allows for a more
    // flexible use.
    if (m_pool.empty()) {
      if constexpr (DynamicSize)
        return &m_pool.emplace(args...);
      else
        return nullptr;
    }
    return ::new (Allocate()) Type(std::forward<Args>(args)...);
  }

  // Replace the pointed object and inserts a COPY
  // of the object into the vector. Which means objects
  // must be copy-able to use this method.
  // TODO: Protect against multiple release of same object
  void Release(Type* obj) {
    obj->~Type();
    m_pool.push(*obj);
  }

  // rvalue equivalent to avoid an expensive copy
  // for objects that support it.
  void Release(Type&& obj) {
    obj.~Type();
    m_pool.push(obj);
  }

  std::size_t FreeChunksCount() {
    return m_pool.size();
  }

private:

  Type* Allocate() {
    Type *place = &m_pool.top();
    m_pool.pop();

    return place;
  }

  std::stack<Type, std::vector<Type> > m_pool;
};

template <std::size_t PoolSize, 
  typename Type,
  typename PoolType = ObjectPool<Type, false> >
class PoolableObject
{
public:

  template<typename... Args>
  static void* operator new(std::size_t count, Args&&... args) {
    return s_pool.Create(args...);
  }

  //NOTE: Just to get rid of warning C4291
  template<typename... Args>
  static void operator delete(void* object, Args&&...) {
    s_pool.Release(static_cast<Type*>(object));
  }

  static void operator delete(void* object) {
    s_pool.Release(static_cast<Type*>(object));
  }

protected:
  static PoolType s_pool;
};

template <std::size_t PoolSize,
  typename Type,
  typename PoolType = ObjectPool<Type, false> >
PoolType PoolableObject<PoolSize, Type, PoolType>::s_pool(PoolSize);