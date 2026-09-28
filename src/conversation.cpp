#include "core/conversation.h"
#include <cassert>

    // Empty conversation: size() == 0, no allocation yet.
    Conversation::Conversation() {
        size_ = 0;
        capacity_ = 0;
    }

    // Releases all owned Message storage. No effect if already empty
    // (e.g. moved-from).
    Conversation::~Conversation(){  // destructor
        delete [] data_;
    }

    // Deep copy: allocates its own buffer and copies every Message.
    // this->begin() must differ from other.begin() afterward.
    Conversation::Conversation(const Conversation& other){  // copy constructor
        data_ = new Message[other.capacity_];
        for(std::size_t i = 0; i < other.capacity_; i++){
            data_[i] = other.data_[i];
        }
        capacity_ = other.capacity_;
        size_ = other.size_;
    }
    Conversation& Conversation::operator=(const Conversation& other){ // copy assignment operator
        if(this != &other){
            delete [] data_;
            data_ = new Message[other.capacity_];
            for(std::size_t i = 0; i < other.capacity_; i++){
                data_[i] = other.data_[i];
            }
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
        return *this;
    }

    // Move constructor
    Conversation::Conversation(Conversation&& other) noexcept:
    data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    //Move assignment operator
    Conversation& Conversation::operator=(Conversation&& other) noexcept{
        if(this != &other){
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Appends m, growing the backing array if needed. Amortized O(1) —
    // document and justify your growth strategy in the design log
    // (see Appendix C if you want a refresher first).
    void Conversation::append(Message m){
        if(size_ < capacity_){
            data_[size_] = m;
        }
        else{
            std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            Message* newData_ = new Message[new_capacity];
            for(std::size_t i = 0; i<size_; i++){
                newData_[i] = data_[i];
            }
            delete[] data_;
            capacity_ = new_capacity;
            data_ = newData_;
            data_[size_] = m;
        }
        size_++;  //increment the size after appending
    }

    // Number of messages currently stored.
    std::size_t Conversation::size() const noexcept{
        return size_;
    }

        // Number of messages possible to store without growing array
    std::size_t Conversation::capacity() const noexcept{
        return capacity_;
    }

    // Bounds-checked access. Decide what happens on i >= size() (throw,
    // assert, whatever you pick) and test that behavior explicitly.
    const Message& Conversation::at(std::size_t i) const{
        assert(i < size_);
        return data_[i];  
    }

    // Range-for iteration, oldest message first. begin() == end() when
    // size() == 0.
    const Message* Conversation::begin() const noexcept{
        return data_;
    }
    const Message* Conversation::end()   const noexcept{
        if(size_ == 0){
            return data_;
        }
        else{
            return &data_[size_];
        }              
    }

