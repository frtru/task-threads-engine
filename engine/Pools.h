#pragma once

// NOTE: Flexible but costly pool and cannot allocate 
// in blocks, in a contiguous and efficient manner. It
// can easily be tweaked or be derived from to enhance
// the properties
template<typename Type>
class ResizeablePointerPool
{
public:
  ResizeablePointerPool() {}
  //delete all of the available memory chunks:
  ~ResizeablePointerPool() {
    while (!m_pool.empty()) {
      ::operator delete(m_pool.top());
      m_pool.pop();
    }
  }

  ResizeablePointerPool(const ResizeablePointerPool &) = delete;
  ResizeablePointerPool(ResizeablePointerPool && other)
    : m_pool(std::move(other.m_pool)) {}

  ResizeablePointerPool& operator=(const ResizeablePointerPool&) = delete;
  ResizeablePointerPool& operator=(ResizeablePointerPool&& other) {
    this->m_pool(std::move(other.m_pool));
  }

  // Create an instance of Type:
  template<typename... Args>
  Type* Create(Args && ...args) {
    Type *place = (Type*)(Allocate());

    try {
      new (place) Type(std::forward<Args>(args)...);
    }
    catch (...) {
      m_pool.push(place);
      throw;
    }

    return place;
  }

  // Remove an instance of Type (add memory to the pool):
  void Remove(Type* obj) {
    obj->~Type();
    m_pool.push(obj);
  }

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

  std::size_t FreeChunksCount() {
    return m_pool.size();
  }

private:

  //stack to hold pointers to free chunks:
  std::stack<Type*> m_pool;
};

// NOTE: For simplicity, Type will require a default
// constructor implementation. TODO: check with args forwarding
// if we can do something
template<typename Type>
class ResizableContiguousPool
{
public:
  ResizableContiguousPool(std::size_t itemCount)
    : m_pool(std::vector<Type>(itemCount)) {}
  //delete all of the available memory chunks:
  ~ResizableContiguousPool() {
    // Let the vector do its job
  }

  ResizableContiguousPool(const ResizableContiguousPool &) = delete;
  ResizableContiguousPool(ResizableContiguousPool && other)
    : m_pool(std::move(other.m_pool)) {}

  ResizableContiguousPool& operator=(const ResizableContiguousPool&) = delete;
  ResizableContiguousPool& operator=(ResizableContiguousPool&& other) {
    this->m_pool(std::move(other.m_pool));
  }

  // Create an instance of Type:
  template<typename... Args>
  Type* Create(Args && ...args) {
    Type *place = (Type*)(Allocate());

    try {
      new (place) Type(std::forward<Args>(args)...);
    }
    catch (...) {
      m_pool.push(*place);
      throw;
    }

    return place;
  }

  // Remove an instance of Type (add memory to the pool):
  void Remove(Type* obj) {
    // TODO:
  }

  // Allocate a chunk of memory as big as Type needs:
  Type* Allocate() {
    Type *place;

    // TODO:

    return place;
  }

  //Keeping as a utility
  void Deallocate(Type* obj) {
    // TODO:
  }

  std::size_t FreeChunksCount() {
    return m_pool.size();
  }

private:

  std::stack<Type, std::vector<Type> > m_pool;
};