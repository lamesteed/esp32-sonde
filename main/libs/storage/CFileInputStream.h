#ifndef CFILEINPUTSTREAM_H
#define CFILEINPUTSTREAM_H

#include "IInputStream.h"
#include "SD_MMC.h"


class CFileInputStream : public IInputStream
{
public:
    CFileInputStream( const File & file );
    virtual ~CFileInputStream();

private:
    // IInputStream interface
    size_t read( uint8_t * buffer, size_t size ) override;
    bool eof() const override;
    void close() override;

private:
    static const char * TAG; ///< Tag for logging

    File mFile; ///< File object for reading data
    size_t mPosition; ///< Current position in the file
    size_t mSize; ///< Size of the file
};


#endif // CFILEINPUTSTREAM_H