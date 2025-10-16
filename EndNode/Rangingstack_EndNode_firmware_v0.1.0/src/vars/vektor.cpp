#include "vektor.h"

// Constructor: Initializes the dynamic array with a given initial size
DynamicArray::DynamicArray(size_t initial_size)
    : size(0), capacity(initial_size)
{
    data = new uint8_t[capacity]; // Allocate memory for the array
}

// Destructor: Cleans up the allocated memory. Automatic in case the variable out of scope (of function)
DynamicArray::~DynamicArray()
{
    delete[] data; // Deallocate memory
}

// Method to add a new element to the array
void DynamicArray::push_back(const uint8_t *element, size_t element_size)
{
    // Resize the array if necessary
    if (size + element_size > capacity)
    {
        resize(capacity * 2);
    }
    // Copy the element into the data array
    std::memcpy(data + size, element, element_size);
    size += element_size; // Update the size
}

// Method to get the current size of the array
size_t DynamicArray::getSize() const
{
    return size;
}

// Method to access the raw data of the array
uint8_t *DynamicArray::getData()
{
    return data;
}

// Method to clear the contents of the array without deallocating memory
void DynamicArray::clear()
{
    size = 0; // Reset size to 0, effectively clearing the contents
}

// Overloaded subscript operator for accessing elements (read-only)
const uint8_t &DynamicArray::operator[](size_t index) const
{
    return data[index];
}

// Overloaded subscript operator for accessing elements (modifiable)
uint8_t &DynamicArray::operator[](size_t index)
{
    return data[index];
}

// Private method to resize the array to a new capacity
void DynamicArray::resize(size_t new_capacity)
{
    uint8_t *new_data = new uint8_t[new_capacity]; // Allocate new memory
    std::memcpy(new_data, data, size);             // Copy old data to new memory
    delete[] data;                                 // Free the old memory
    data = new_data;                               // Point to the new memory
    capacity = new_capacity;                       // Update the capacity
}

// Std-vector like
//  Constructor
template <typename T>
Vektor<T>::Vektor(size_t initial_size)
    : size(0), capacity(initial_size)
{
    data = new T[capacity]; // Allocate memory for the array
}

// Destructor
template <typename T>
Vektor<T>::~Vektor()
{
    delete[] data; // Deallocate memory
}

// Method to add a new element to the array
template <typename T>
void Vektor<T>::push_back(const T &element)
{
    // Resize the array if necessary
    if (size >= capacity)
    {
        resize(capacity * 2); // Double the capacity
    }
    data[size++] = element; // Copy the element and update the size
}

// Method to get the current size of the array
template <typename T>
size_t Vektor<T>::getSize() const
{
    return size;
}

// Method to access the raw data of the array
template <typename T>
T *Vektor<T>::getData()
{
    return data;
}

// Method to clear the contents of the array without deallocating memory
template <typename T>
void Vektor<T>::clear()
{
    size = 0; // Reset size to 0, effectively clearing the contents
}

// Overloaded subscript operator for accessing elements (read-only)
template <typename T>
const T &Vektor<T>::operator[](size_t index) const
{
    return data[index];
}

// Overloaded subscript operator for accessing elements (modifiable)
template <typename T>
T &Vektor<T>::operator[](size_t index)
{
    return data[index];
}

// Private method to resize the array to a new capacity
template <typename T>
void Vektor<T>::resize(size_t new_capacity)
{
    T *new_data = new T[new_capacity]; // Allocate new memory
    for (size_t i = 0; i < size; ++i)
    {
        new_data[i] = data[i]; // Copy old data to new memory
    }
    delete[] data;           // Free the old memory
    data = new_data;         // Point to the new memory
    capacity = new_capacity; // Update the capacity
}

// Explicit template instantiation for common types
template class Vektor<int>;
template class Vektor<int8_t>;
template class Vektor<uint8_t>;
template class Vektor<int16_t>;
template class Vektor<uint16_t>;
template class Vektor<int32_t>;
template class Vektor<uint32_t>;
template class Vektor<float>;
template class Vektor<double>;