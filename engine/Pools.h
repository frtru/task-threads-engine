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

