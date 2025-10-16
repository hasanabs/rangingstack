#ifndef VEKTOR_H
#define VEKTOR_H

#include <cstdint> // For uint8_t
#include <cstring> // For std::memcpy

class DynamicArray
{
public:
    // Constructor to initialize the dynamic array with a given initial size
    DynamicArray(size_t initial_size = 30);

    // Destructor to clean up allocated memory. Automatic in case the variable out of scope (of function)
    ~DynamicArray();

    // Method to add a new element to the array
    void push_back(const uint8_t *element, size_t element_size);

    // Method to get the current size of the array
    size_t getSize() const;

    // Method to access the raw data of the array
    uint8_t *getData();

    // Method to clear the contents of the array without deallocating memory
    void clear();

    // Overloaded subscript operator for accessing elements (read-only)
    const uint8_t &operator[](size_t index) const;

    // Overloaded subscript operator for accessing elements (modifiable)
    uint8_t &operator[](size_t index);

private:
    size_t size;     // Current number of elements in the array
    size_t capacity; // Maximum capacity of the array
    uint8_t *data;   // Pointer to the dynamically allocated array

    // Method to resize the array to a new capacity
    void resize(size_t new_capacity);
};

template <typename T>
class Vektor
{
public:
    // Constructor
    Vektor(size_t initial_size = 30);

    // Destructor
    ~Vektor();

    // Method to add a new element to the array
    void push_back(const T &element);

    // Method to get the current size of the array
    size_t getSize() const;

    // Method to access the raw data of the array
    T *getData();

    // Method to clear the contents of the array without deallocating memory
    void clear();

    // Overloaded subscript operator for accessing elements (read-only)
    const T &operator[](size_t index) const;

    // Overloaded subscript operator for accessing elements (modifiable)
    T &operator[](size_t index);

private:
    size_t size;     // Current number of elements in the array
    size_t capacity; // Maximum capacity of the array
    T *data;         // Pointer to the dynamically allocated array

    // Method to resize the array to a new capacity
    void resize(size_t new_capacity);
};

#endif // VEKTOR_H
