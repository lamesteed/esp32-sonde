#include "CStoreFileCommand.h"

#include <mbedtls/base64.h> // Base64 encoding/decoding provided by espressif mbedtls library

std::string encodeBase64(const std::string &input) {

    // First call is to only figure out the size of the encoded buffer that will be needed
    size_t encoded_len;
    mbedtls_base64_encode(nullptr, 0, &encoded_len, reinterpret_cast<const unsigned char*>(input.data()), input.size());

    // Now we can create the buffer of the right size and actually encode the data
    std::string encoded(encoded_len, '\0'); // Allocate enough space for the encoded string
    mbedtls_base64_encode(reinterpret_cast<unsigned char*>(&encoded[0]), encoded_len, &encoded_len,
                          reinterpret_cast<const unsigned char*>(input.data()), input.size());
    return encoded;
}

std::string decodeBase64( const std::string & input ) {

    // First call is to only figure out the size of the decoded buffer that will be needed
    size_t decoded_len;
    mbedtls_base64_decode(nullptr, 0, &decoded_len, reinterpret_cast<const unsigned char*>(input.data()), input.size());

    // Now we can create the buffer of the right size and actually decode the data
    std::string decoded(decoded_len, '\0'); // Allocate space for the decoded string
    mbedtls_base64_decode(reinterpret_cast<unsigned char*>(&decoded[0]), decoded_len, &decoded_len,
                          reinterpret_cast<const unsigned char*>(input.data()), input.size());
    return decoded;
}

const char * CStoreFileCommand::ARG_FILENAME = "filename";
const char * CStoreFileCommand::ARG_DATA = "data";

CStoreFileCommand::CStoreFileCommand(
    const IStorageService::Ptr & storageService,
    const IDataPublisherService::Ptr publisher,
    const ICommand::CommandArgs & args )
        : mStorageService( storageService )
        , mPublisher( publisher )
        , mArgs( args )
{
}

std::string CStoreFileCommand::getDescription() const
{
    static const char * UNKNOWN = "UNKNOWN";

    // retrieve ("filename"="value") from mArgs, set to "UNKNOWN" if not found
    auto it = mArgs.find( ARG_FILENAME );
    std::string filename = ( it != mArgs.end() ) ? it->second : UNKNOWN;

    return "STOREFILE: Store file in storage: " + filename;
}

bool CStoreFileCommand::execute()
{
    // retrieve ("filename"="value") from mArgs
    auto it = mArgs.find( ARG_FILENAME );
    if ( it == mArgs.end() )
    {
        mPublisher->publishData( "Missing filename argument", true );
        return false;
    }

    std::string filename = it->second;

    // retrieve ("data"="value") from mArgs
    it = mArgs.find( ARG_DATA );
    if ( it == mArgs.end() )
    {
        mPublisher->publishData( "Missing data argument", true );
        return false;
    }

    // it is assumed that data is base64 encoded - decode it before storing
    std::string data = decodeBase64( it->second );

    // store data in storage
    bool success = mStorageService->storeData( filename, data );
    if ( !success )
    {
        mPublisher->publishData( "Failed to store file: " + filename, true );
        return false;
    }

    // publish success message to client
    return mPublisher->publishData( "File stored successfully: " + filename, true );
}
