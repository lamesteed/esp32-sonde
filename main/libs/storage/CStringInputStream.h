#ifndef CSTRINGINPUTSTREAM_H
#define CSTRINGINPUTSTREAM_H

#include "IInputStream.h"
#include <string>

class CStringInputStream : public IInputStream
{
public:
    /// @brief Constructor
    CStringInputStream( const std::string & data );

    /// @brief Destructor
    virtual ~CStringInputStream();

private:
    // IInputStream interface
    virtual size_t read( uint8_t * buffer, size_t size ) override;
    virtual bool eof() const override;
    virtual void close() override;

private:

    static const char * TAG; ///< Tag for logging

    std::string mData; ///< Data to read from
    size_t mPosition;  ///< Current position in the data
    size_t mSize;     ///< Size of the data
};

#endif // CSTRINGINPUTSTREAM_H