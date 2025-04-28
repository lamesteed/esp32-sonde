#ifndef IINPUTSTREAM_H
#define IINPUTSTREAM_H

#include <memory>

class IInputStream
{
public:
    using Ptr = std::shared_ptr<IInputStream>;

    virtual ~IInputStream() = default;

    /// @brief Read data from the input stream
    /// @param buffer Buffer to store the read data
    /// @param size Size of the buffer
    /// @return Number of bytes read
    virtual size_t read( uint8_t * buffer, size_t size ) = 0;

    /// @brief Check if the end of the stream is reached
    /// @return true if end of stream is reached, false otherwise
    virtual bool eof() const = 0;

    /// @brief Close the input stream
    virtual void close() = 0;
};

#endif // IINPUTSTREAM_H